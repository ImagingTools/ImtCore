// The entry of a compiled application (compiler/compiler.js). The bundle ends with `bootstrap(spec)`;
// the specification carries the sources of the project (the compiler validated them), the module
// directories of the config, the fonts, the translations and the URL root of the pictures.
//
// spec: {
//   name, root, main, lenient, maxErrors, language, incremental,
//   files: { path: text },        JavaScript files, qmldir files, and the QML documents of a debug build
//   docs: { path: text },         the QML documents of a compiled application, as packed trees (src/packed.js)
//   scans: { path: { f, t, q, e } }  what the text of a packed document names (see createProject)
//   docsDeflate: base64,           --compress-docs of the compiler: docs and scans deflated (raw) together, in their place
//   debug: true                   the text of the documents is there, and window.qmlWeb gives the engine
//   binaries: { path: base64 },   fonts
//   modules: { name: [dir] },     module name -> directories with a qmldir
//   translations: { language: { source: translation or [plural forms] } },
// }

import { Engine } from './engine.js';
import { attachCanvas } from './host.js';
import { MapFS, Project } from './project.js';
import { Flickable, ListView } from './items.js';
import { Renderer } from './renderer.js';
import { Scene } from './scene.js';
import { createTestApi, trackNetwork } from './testapi.js';

export function decodeBase64(text) {
	if (typeof atob === 'function') {
		const binary = atob(text);
		const bytes = new Uint8Array(binary.length);
		for (let i = 0; i < binary.length; i++) bytes[i] = binary.charCodeAt(i);
		return bytes.buffer;
	}
	const data = Buffer.from(text, 'base64');
	return data.buffer.slice(data.byteOffset, data.byteOffset + data.byteLength);
}

// The URL root of the pictures the way the JQML runtime computes it: the part of the page path up to and
// including `root` when the page lives below it, otherwise `root` itself. Always ends with a slash.
export function resourceRoot(root, location) {
	let result = root || '/';
	if (location && typeof location.pathname === 'string') {
		const at = location.pathname.indexOf(result);
		if (at >= 0) result = location.origin + location.pathname.slice(0, at + result.length);
	}
	return result.endsWith('/') ? result : `${result}/`;
}

// The documents of a compiled application are packed trees, without their text (compiler/compiler.js); a scan lists the font
// files `f`, the types made by createObject `t` and the QML files `q` a document names, and `e` when it declares an enum.
function addCompiledDocuments(project, docs, scans) {
	for (const [path, record] of Object.entries(docs || {})) {
		const scan = (scans || {})[path] || {};
		project.addCompiled(path, record, { fonts: scan.f || [], types: scan.t || [], files: scan.q || [], enums: scan.e === 1 });
	}
}

// `spec.docsDeflate` (--compress-docs of the compiler): `{ docs, scans }` deflated (raw) and base64-encoded, one block instead of
// the usual plain JSON of `spec.docs`/`spec.scans`. DecompressionStream is a browser/Node API (not in every very old browser); a
// build made with the flag needs it, and says so rather than silently doing nothing.
async function inflateDocs(base64) {
	if (typeof DecompressionStream !== 'function') {
		throw new Error('This browser has no DecompressionStream; it cannot start a build made with --compress-docs.');
	}
	const stream = new Blob([decodeBase64(base64)]).stream().pipeThrough(new DecompressionStream('deflate-raw'));
	return JSON.parse(await new Response(stream).text());
}

// A project over the embedded files; load() parses them again in the browser. The documents of a build made with
// --compress-docs are not there yet (they are still deflated): addDeflatedDocs adds them, and createApplication's
// start() calls it before the project loads its main document.
export function createProject(spec) {
	const files = new Map(Object.entries(spec.files || {}));
	for (const [path, base64] of Object.entries(spec.binaries || {})) files.set(path, decodeBase64(base64));
	const project = new Project(new MapFS(files), { modules: spec.modules || {}, trusted: spec.trusted === true });
	addCompiledDocuments(project, spec.docs, spec.scans);
	return project;
}

// Inflates spec.docsDeflate (if there is one) and adds its documents to `project`; a no-op otherwise.
export async function addDeflatedDocs(project, spec) {
	if (!spec.docsDeflate) return;
	const { docs, scans } = await inflateDocs(spec.docsDeflate);
	addCompiledDocuments(project, docs, scans);
}

function pickLanguage(spec) {
	if (spec.language) return spec.language;
	const languages = Object.keys(spec.translations || {});
	if (languages.length === 0 || typeof navigator === 'undefined') return '';
	return (navigator.language || '').replace('-', '_');
}

// Engine, scene and renderer for the canvas; `start()` loads the main document below the scene root.
export function createApplication(spec, canvas, location = null) {
	const renderer = new Renderer(canvas);
	const engine = new Engine();
	engine.lenient = spec.lenient !== false;
	// The scripts of the QML files get a name and a source map, so the debugger stops in the .qml file. A debug build does it always, any
	// other one with `?qmlsource=1` in the address: it costs time at the start (every script is compiled on its own, and the expressions
	// run as written, without src/resolve.js).
	const wantsSources = spec.debug === true || (location && typeof location.search === 'string' && /[?&]qmlsource=1/.test(location.search));
	if (typeof spec.sourceBase === 'string' && wantsSources) engine.sourceBase = spec.sourceBase;
	engine.incremental = spec.incremental !== false;
	// A debug build: `?noresolve=1` runs the expressions through `with` as before, `?verify=1` runs both forms and compares them.
	if (spec.debug === true && location && typeof location.search === 'string') {
		if (/[?&]noresolve=1/.test(location.search)) engine.resolveNames = false;
		if (/[?&]verify=1/.test(location.search)) engine.verifyResolution = true;
	}
	if (spec.maxErrors) engine.maxErrors = spec.maxErrors;
	engine.translations = spec.translations || {};
	engine.language = pickLanguage(spec);
	engine.resourceRoot = resourceRoot(spec.root, location);
	const scene = new Scene(renderer, engine);
	scene.frameHandler = (seconds) => {
		engine.setTime(seconds);
		if (engine.isAnimated) scene.root.$update();
	};
	const project = createProject(spec);
	const start = async () => {
		await addDeflatedDocs(project, spec);
		await project.load(spec.main);
		await project.loadEmbedded();
		return engine.load(project, scene.root);
	};
	return { engine, scene, renderer, project, start };
}

function showFatal(error) {
	const message = error instanceof Error ? error.message : String(error);
	console.error(message);
	if (typeof document === 'undefined') return;
	const panel = document.createElement('pre');
	panel.style.cssText = 'position:absolute;left:0;top:0;margin:0;padding:12px;color:#ff8c8c;background:#101014;font:13px ui-monospace,Menlo,monospace;white-space:pre-wrap';
	panel.textContent = message;
	document.body.append(panel);
}

// Creates a canvas filling the page and starts the application once the page is loaded.
// `window.qmlWebApplication` exposes the specification and the factories before that (devtools, tests).
export function bootstrap(spec) {
	// The engine, the project and the documents are reached through these only in a debug build.
	if (typeof window !== 'undefined' && spec.debug === true) window.qmlWebApplication = { spec, createApplication, createProject };
	if (typeof document === 'undefined') return;
	const run = () => {
		const style = document.createElement('style');
		style.textContent = 'html,body{margin:0;height:100%;overflow:hidden;background:#fff}canvas.qmlweb{display:block;width:100%;height:100%}';
		document.head.append(style);
		const canvas = document.createElement('canvas');
		canvas.className = 'qmlweb';
		document.body.append(canvas);
		const application = createApplication(spec, canvas, window.location);
		attachCanvas(canvas, application.scene, application.engine);
		if (spec.debug === true) window.qmlWeb = application;
		// Every build has it, release ones too: the tests find and drive the items through it (see testapi.js).
		const testApi = createTestApi({ engine: application.engine, scene: application.scene, canvas, network: trackNetwork(window), classes: { Flickable, ListView } });
		window.qmlTest = testApi;
		application.start().then(() => testApi.markStarted(), showFatal);
	};
	if (document.readyState === 'complete') run();
	else window.addEventListener('load', run);
}
