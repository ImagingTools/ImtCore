// The entry of a compiled application (compiler/compiler.js). The bundle ends with `bootstrap(spec)`;
// the specification carries the sources of the project (the compiler validated them), the module
// directories of the config, the fonts, the translations and the URL root of the pictures.
//
// spec: {
//   name, root, main, lenient, maxErrors, language, incremental,
//   files: { path: text },        QML documents, JavaScript files, qmldir files
//   binaries: { path: base64 },   fonts
//   modules: { name: [dir] },     module name -> directories with a qmldir
//   translations: { language: { source: translation } },
// }

import { Engine } from './engine.js';
import { attachCanvas } from './host.js';
import { MapFS, Project } from './project.js';
import { Renderer } from './renderer.js';
import { Scene } from './scene.js';

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

// A project over the embedded files; load() parses them again in the browser.
export function createProject(spec) {
	const files = new Map(Object.entries(spec.files || {}));
	for (const [path, base64] of Object.entries(spec.binaries || {})) files.set(path, decodeBase64(base64));
	return new Project(new MapFS(files), { modules: spec.modules || {} });
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
	engine.incremental = spec.incremental !== false;
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
	if (typeof window !== 'undefined') window.qmlWebApplication = { spec, createApplication, createProject };
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
		window.qmlWeb = application;
		application.start().catch(showFatal);
	};
	if (document.readyState === 'complete') run();
	else window.addEventListener('load', run);
}
