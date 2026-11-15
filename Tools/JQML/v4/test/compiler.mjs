// Checks the compiler (compiler/compiler.js) without a browser: the file list of --list-qml, the html
// page, and the js bundle, which is evaluated and run headless (no WebGL, no fonts) like test/run.mjs
// does with the samples. `node test/compiler.mjs`
import assert from 'node:assert/strict';
import { existsSync, mkdtempSync, readFileSync, rmSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';
import vm from 'node:vm';
import { commonDirectory, compile, htmlPage, listQmldirQmlFiles, parseArgs } from '../compiler/compiler.js';
import { createApplication as createCompiledApplication } from '../src/runtime.js';
import { parseTsCatalog } from '../compiler/translations.js';

const here = dirname(fileURLToPath(import.meta.url));
const fixture = join(here, 'compiler');
const env = { ...process.env, QMLWEB_TEST_BASE: fixture };
const quiet = { env, log: () => {}, warn: () => {} };

function walk(item, visit) {
	visit(item);
	for (const child of item.$children) walk(child, visit);
}

// Evaluates a compiled bundle the way a page would, but without a DOM: bootstrap() only publishes
// window.qmlWebApplication then, and only in a debug build.
function evaluateBundle(code) {
	const context = vm.createContext({ console, TextEncoder, TextDecoder, setTimeout, clearTimeout, Buffer, performance });
	context.window = context;
	context.globalThis = context;
	vm.runInContext(code, context, { filename: 'bundle.js' });
	return context.window.qmlWebApplication;
}

const checks = {
	'command line'() {
		const options = parseArgs(['-c', 'a.json', '--output=out', '-e', 'Main.qml', '-n', 'app', '-m', 'js', '--list-qml']);
		assert.deepEqual(options, { config: 'a.json', output: 'out', entry: 'Main.qml', name: 'app', root: '/', mode: 'js', listQml: true, help: false });
		assert.throws(() => parseArgs(['-m', 'xml']), /must be "js" or "html"/);
		assert.throws(() => parseArgs(['--bogus']), /unknown option/);
		assert.throws(() => parseArgs(['-o']), /requires a value/);
	},

	'list-qml'() {
		const files = listQmldirQmlFiles(join(fixture, 'app.json'), env, 'app/Main.qml').map((file) => file.slice(fixture.length + 1));
		assert.deepEqual(files.sort(), ['app/Main.qml', 'app/Sibling.qml', 'app/Unused.qml', 'modules/Widgets/Base/Theme.qml', 'modules/Widgets/Widget.qml']);
		// The entry directory is a module: its files come from the qmldir only.
		const moduleFiles = listQmldirQmlFiles(join(fixture, 'app.json'), env, 'modules/Widgets/Widget.qml');
		assert.equal(moduleFiles.length, 2);
		assert.throws(() => listQmldirQmlFiles(join(fixture, 'app.json'), { ...env, QMLWEB_TEST_BASE: undefined }), /cannot read config/);
	},

	'translations'() {
		const catalog = parseTsCatalog(readFileSync(join(fixture, 'translations', 'app_de_DE.ts'), 'utf8'));
		assert.deepEqual(catalog, { Hello: 'Hallo' });
		assert.equal(commonDirectory(['/a/b/c.qml', '/a/b/d/e.qml', '/a/f.qml']), '/a');
		assert.equal(commonDirectory(['/x.qml']), '/');
	},

	async 'html mode'() {
		const out = mkdtempSync(join(tmpdir(), 'qmlweb-html-'));
		try {
			await compile({ mode: 'html', output: out, name: 'index', icon: 'app.ico' }, quiet);
			const html = readFileSync(join(out, 'index.html'), 'utf8');
			assert.match(html, /<script src="index.js"><\/script>/);
			assert.match(html, /<link rel="icon" href="app.ico">/);
			assert.match(html, /<title>index<\/title>/);
			assert.equal(html, htmlPage({ name: 'index', icon: 'app.ico' }));
		} finally {
			rmSync(out, { recursive: true, force: true });
		}
	},

	async 'js mode'() {
		const out = mkdtempSync(join(tmpdir(), 'qmlweb-js-'));
		try {
			const result = await compile({ mode: 'js', config: join(fixture, 'app.json'), entry: 'app/Main.qml', output: out, name: 'index', root: '/data/', debug: true }, quiet);
			assert.ok(existsSync(join(out, 'index.js')));
			assert.ok(existsSync(join(out, 'index.js.map')));
			const map = JSON.parse(readFileSync(join(out, 'index.js.map'), 'utf8'));
			assert.equal(map.version, 3);
			assert.ok(map.sources.some((s) => s.endsWith('runtime.js')) && map.sources.some((s) => s.endsWith('engine.js')));

			const application = evaluateBundle(result.code);
			// Objects of the vm realm have foreign prototypes: compare them through JSON.
			const spec = JSON.parse(JSON.stringify(application.spec));
			assert.equal(spec.main, 'app/Main.qml');
			assert.equal(spec.root, '/data/');
			assert.deepEqual(spec.modules, { Widgets: ['modules/Widgets'] });
			assert.deepEqual(Object.keys(spec.files).sort(), ['app/Main.qml', 'app/Sibling.qml', 'app/Unused.qml', 'modules/Widgets/Base/Theme.qml', 'modules/Widgets/Widget.qml', 'modules/Widgets/qmldir', 'modules/Widgets/util.js']);
			assert.deepEqual(spec.translations, { de_DE: { Hello: 'Hallo' } });

			// Runs the application headless, in German.
			const app = application.createApplication({ ...spec, language: 'de-DE' }, null);
			assert.equal(app.engine.resourceRoot, '/data/');
			app.scene.resize(800, 600, 1);
			const root = await app.start();
			for (let i = 0; i < 3; i++) app.scene.drawFrame(i * 0.016);
			const names = [];
			walk(root, (item) => names.push(item.objectName));
			assert.ok(names.includes('widget') && names.includes('sibling') && names.includes('widgetText'));
			assert.equal(root.greeting, 'Hallo');
			assert.equal(root.sum, 5);
			assert.equal(root.color, '#FF101010');
			let widgetText = null;
			walk(root, (item) => {
				if (item.objectName === 'widgetText') widgetText = item;
			});
			assert.equal(widgetText.text, 'Hallo');
			assert.equal(app.engine.errors.length, 0, app.engine.errors.join("\n"));

			// The page root decides the language when the specification does not.
			const english = application.createApplication(spec, null);
			english.scene.resize(800, 600, 1);
			assert.equal((await english.start()).greeting, 'Hello');

			// Documents nobody instantiated stay unparsed until the engine asks for their tree.
			const unused = english.project.docs.get('app/Unused.qml');
			assert.equal(unused.parsed, false);
			assert.ok(unused.def !== null && unused.error === null);
			assert.equal(unused.parsed, true);
		} finally {
			rmSync(out, { recursive: true, force: true });
		}
	},

	// The default build carries the documents as packed trees: no QML text, no comments, nothing to reach the engine through.
	async 'compiled mode'() {
		const result = await compile({ mode: 'js', config: join(fixture, 'app.json'), entry: 'app/Main.qml', name: 'index', root: '/data/' }, quiet);
		const spec = JSON.parse(JSON.stringify(result.spec));
		assert.equal(spec.debug, undefined);
		assert.deepEqual(Object.keys(spec.files).sort(), ['modules/Widgets/qmldir', 'modules/Widgets/util.js']);
		assert.deepEqual(Object.keys(spec.docs).sort(), ['app/Main.qml', 'app/Sibling.qml', 'app/Unused.qml', 'modules/Widgets/Base/Theme.qml', 'modules/Widgets/Widget.qml']);
		for (const text of [result.code, JSON.stringify(spec.docs)]) {
			assert.ok(!text.includes('import QtQuick'), 'the text of a document is in the bundle');
			assert.ok(!text.includes('The entry of the compiler test'), 'a comment of a document is in the bundle');
			assert.ok(!text.includes('property string greeting'), 'a declaration is in the bundle as text');
		}
		// A page does not reach the engine, the project or the documents through its window.
		assert.equal(evaluateBundle(result.code), undefined);

		const app = createCompiledApplication({ ...spec, language: 'de-DE' }, null);
		app.scene.resize(800, 600, 1);
		const root = await app.start();
		for (let i = 0; i < 3; i++) app.scene.drawFrame(i * 0.016);
		assert.equal(root.greeting, 'Hallo');
		assert.equal(root.sum, 5);
		assert.equal(root.color, '#FF101010');
		const names = [];
		walk(root, (item) => names.push(item.objectName));
		assert.ok(names.includes('widget') && names.includes('sibling') && names.includes('widgetText'));
		assert.equal(app.engine.errors.length, 0, app.engine.errors.join('\n'));
		const unused = app.project.docs.get('app/Unused.qml');
		assert.equal(unused.parsed, false);
		assert.ok(unused.def !== null && unused.error === null);
		assert.equal(unused.parsed, true);
	},

	async 'syntax error'() {
		await assert.rejects(compile({ mode: 'js', entry: join(fixture, 'broken', 'Main.qml'), name: 'broken' }, quiet), (error) => {
			assert.match(error.message, /broken\/Main.qml:5:13: error: /);
			return true;
		});
		await assert.rejects(compile({ mode: 'js', name: 'x' }, quiet), /-e, --entry is required/);
	},
};

let failed = 0;
for (const [name, check] of Object.entries(checks)) {
	try {
		await check();
		console.log(`ok   ${name}`);
	} catch (error) {
		failed++;
		console.log(`FAIL ${name}\n    ${error.stack || error}`);
	}
}
console.log(failed === 0 ? 'All good' : `${failed} failed`);
process.exitCode = failed === 0 ? 0 : 1;
