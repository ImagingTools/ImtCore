// Checks the incremental evaluation of bindings (a pass runs only the bindings whose inputs changed) against the
// full sweep it replaces:
//   - every sample, every case of test/desktop and test/jqml ends in the same state in both modes;
//   - after each frame an audit runs every binding once more: none may change (a change is a missed update);
//   - the pages of the controls gallery pass the audit while they are clicked through and scrolled.
//
//   node test/incremental.mjs [Name ...]
import { existsSync } from 'node:fs';
import { readFile, readdir, stat } from 'node:fs/promises';
import { dirname, join, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';
import { Engine } from '../src/engine.js';
import { Project } from '../src/project.js';
import { Renderer } from '../src/renderer.js';
import { Scene } from '../src/scene.js';
import { Viewer } from '../src/viewer.js';

const here = dirname(fileURLToPath(import.meta.url));
const root = join(here, '..');
const MAX_FRAMES = 60;

class DirFS {
	constructor(dir) {
		this.dir = dir;
	}

	async readText(path) {
		try {
			return await readFile(join(this.dir, path), 'utf8');
		} catch {
			return null;
		}
	}

	async readBytes(path) {
		try {
			const data = await readFile(join(this.dir, path));
			return data.buffer.slice(data.byteOffset, data.byteOffset + data.byteLength);
		} catch {
			return null;
		}
	}
}

function fillEnv(source) {
	return String(source).replace(/\$\{([A-Za-z0-9_]+)\}/g, (_, key) => process.env[key] ?? '');
}

// `dirs` of test.json plus the configs it includes; missing folders are skipped.
async function importDirs(testDir) {
	const found = [];
	const seen = new Set();
	async function collect(configPath) {
		const absolute = resolve(configPath);
		if (seen.has(absolute)) return;
		seen.add(absolute);
		let config;
		try {
			config = JSON.parse(fillEnv(await readFile(absolute, 'utf8')));
		} catch {
			return;
		}
		const base = dirname(absolute);
		for (const dir of config.dirs || []) {
			const full = resolve(base, fillEnv(dir));
			if (existsSync(full)) found.push(full);
		}
		for (const include of config.includes || []) await collect(resolve(base, fillEnv(include)));
	}
	await collect(join(testDir, 'test.json'));
	return found;
}

// What a user could see: the geometry, the visibility and the declared properties of every item.
function digest(item, seen = new Set()) {
	if (item === null || item === undefined || seen.has(item)) return '';
	seen.add(item);
	const parts = [item.$typeName, item.$ax, item.$ay, item.$aw, item.$ah, item.$visible, item.$text];
	if (item.$custom !== null && item.$custom !== undefined) {
		for (const key of Object.keys(item.$custom)) {
			const value = item.$custom[key];
			parts.push(`${key}=${value !== null && typeof value === 'object' ? '[object]' : String(value)}`);
		}
	}
	let text = parts.join('|');
	for (const child of item.$children) text += `(${digest(child, seen)})`;
	return text;
}

function newEngine(incremental, audit) {
	const engine = new Engine();
	engine.lenient = true;
	engine.maxErrors = 400;
	engine.incremental = incremental;
	engine.auditMode = audit;
	return engine;
}

function misses(engine) {
	return [...engine.auditMisses].map(([key, value]) => `${key} ${JSON.stringify(value)}`);
}

// ----- a case of test/desktop: Main.qml prints through console.log -----

function settle(engine, root, audit) {
	for (let pass = 0; pass < 10; pass++) {
		root.$updateRequested = false;
		root.$layout();
		engine.evaluateBindings();
		if (!root.$updateRequested && engine.dirty.size === 0) break;
	}
	if (audit) engine.auditSweep();
}

async function runCase(dir, incremental, audit) {
	const engine = newEngine(incremental, audit);
	const logs = [];
	let quit = false;
	engine.onLog = (line) => logs.push(line);
	engine.onQuit = () => {
		quit = true;
	};
	try {
		const dirs = await importDirs(dir);
		const project = new Project(new DirFS(dir), { importPaths: dirs });
		await project.load('Main.qml');
		engine.reset();
		engine.lenient = true;
		engine.project = project;
		engine.host = null;
		engine.setupSingletons();
		const doc = project.docs.get(project.mainPath);
		engine.documentScope = engine.newScope(null, doc.path);
		const item = engine.createItem(doc.def, engine.documentScope, null, [], undefined, 0);
		settle(engine, item, audit);
		engine.runCompletedFrom(0);
		if (!quit) settle(engine, item, audit);
		let seconds = 0;
		for (let frame = 0; frame < MAX_FRAMES && !quit; frame++) {
			let running = false;
			for (const timer of engine.timers) if (!timer.$destroyed && timer.$running) running = true;
			if (!running && !item.$updateRequested && engine.dirty.size === 0) break;
			await new Promise((resolvePromise) => setImmediate(resolvePromise));
			seconds += 0.016;
			item.$updateRequested = false;
			item.$layout();
			engine.evaluateBindings();
			engine.setTime(seconds);
			if (!quit) settle(engine, item, audit);
		}
		return { logs, errors: engine.errors.slice(), state: digest(item), misses: misses(engine), threw: '' };
	} catch (error) {
		return { logs, errors: [], state: '', misses: misses(engine), threw: error.message };
	}
}

async function caseNames() {
	const found = [];
	for (const base of [here + '/desktop', here + '/jqml']) {
		let names = [];
		try {
			names = (await readdir(base)).sort();
		} catch {
			continue;
		}
		for (const name of names) {
			const dir = join(base, name);
			try {
				if ((await stat(dir)).isDirectory() && (await stat(join(dir, 'Main.qml'))).isFile()) found.push({ name, dir });
			} catch {
				// not a case
			}
		}
	}
	return found;
}

// ----- a sample of qml/ and the pages of the gallery, shown through the viewer -----

async function runViewer(base, main, importPaths, incremental, audit, drive) {
	const engine = newEngine(incremental, audit);
	const scene = new Scene(new Renderer(null), engine);
	const viewer = new Viewer(scene, engine, () => {});
	scene.resize(1280, 900, 1);
	const project = new Project(new DirFS(base), { importPaths });
	await project.load(main);
	viewer.load(project);
	const frames = (count) => {
		for (let i = 0; i < count; i++) {
			frames.clock += 0.016;
			scene.root.$update();
			scene.drawFrame(frames.clock);
		}
	};
	frames.clock = 0;
	frames(8);
	if (drive !== undefined) drive(scene, frames);
	return { errors: engine.errors.length, state: digest(viewer.content), misses: misses(engine) };
}

// Clicks the entries of the gallery menu one after the other, scrolls and moves the pointer over each page.
function clickThrough(scene, frames) {
	const position = (item) => {
		let x = 0;
		let y = 0;
		for (let p = item; p; p = p.$parent) {
			x += p.$ax || 0;
			y += p.$ay || 0;
		}
		return { x, y };
	};
	const find = (name) => {
		let found = null;
		const seen = new Set();
		const walk = (item, depth) => {
			if (!item || seen.has(item) || depth > 150) return;
			seen.add(item);
			if (item.$typeName === 'Text' && item.$text === name && item.$visible) {
				const at = position(item);
				if (at.x < 300 && at.y > 0 && at.y < 880) found = at;
			}
			for (const child of item.$children || []) walk(child, depth + 1);
		};
		walk(scene.root, 0);
		return found;
	};
	for (const name of ['Buttons', 'Inputs', 'Views', 'Indicators', 'Dialogs', 'Table', 'Canvas', 'Reports']) {
		const at = find(name);
		if (at === null) continue;
		const x = at.x + 30;
		const y = at.y + 7;
		scene.pointerMove(x, y);
		scene.pointerDown(x, y, 0);
		scene.pointerUp(x, y);
		frames(6);
		for (let k = 0; k < 4; k++) {
			scene.wheel(700, 400, -1.6);
			frames(1);
		}
		for (let k = 0; k < 3; k++) {
			scene.pointerMove(500 + k * 60, 300 + k * 40);
			frames(1);
		}
	}
}

// A change nobody announces must be reported, or the audit proves nothing.
async function auditDetectsMissedUpdate() {
	const engine = newEngine(true, true);
	const files = new Map([['Main.qml', 'import QtQuick 2.0\nItem { id: r; property int a: 1; width: 100; height: 100\n Text { id: t; text: "a" + r.a }\n}\n']]);
	const fs = { readText: async (path) => files.get(path) ?? null, readBytes: async () => null };
	const scene = new Scene(new Renderer(null), engine);
	const viewer = new Viewer(scene, engine, () => {});
	scene.resize(400, 300, 1);
	const project = new Project(fs);
	await project.load('Main.qml');
	viewer.load(project);
	for (let i = 0; i < 4; i++) scene.drawFrame(i * 0.016);
	if (engine.auditMisses.size !== 0) return 'the audit reports a miss before anything was changed';
	let item = null;
	const walk = (it) => {
		if (it.$children.some((child) => child.$typeName === 'Text' && child.$bindings !== null)) item = it;
		for (const child of it.$children) walk(child);
	};
	walk(viewer.content);
	if (item === null) return 'the sample item was not found';
	item.$custom.a = 7; // no notification: the Text binding must be missed
	scene.root.$update();
	scene.drawFrame(1);
	return engine.auditMisses.size > 0 ? '' : 'the audit did not see an unannounced change';
}

// ----- scenarios: what a script, the pointer or the tree changes while bindings follow it -----

const SCENARIOS = [
	{
		name: 'ListModel rows reach the delegates and the readers of the model',
		qml: `import QtQuick 2.0
Item { id: top; width: 400; height: 300
	ListModel { id: lm; ListElement { name: "a"; n: 1 } ListElement { name: "b"; n: 2 } }
	Text { id: summary; text: "count " + lm.count + " first " + (lm.count > 0 ? lm.get(0).name : "-") }
	Column { id: col
		Repeater { id: rep; model: lm
			Text { text: model.name + ":" + (model.n * 2) }
		}
	}
	ListView { id: view; y: 100; width: 200; height: 100; model: lm
		delegate: Rectangle { width: 100; height: 20; color: model.n > 1 ? "red" : "blue"
			Text { text: name }
		}
	}
}`,
		steps: [
			(ids) => ids.lm.append({ name: 'c', n: 3 }),
			(ids) => ids.lm.setProperty(0, 'name', 'A'),
			(ids) => (ids.lm.get(1).name = 'B'),
			(ids) => ids.lm.set(2, { name: 'C', n: 30 }),
			(ids) => ids.lm.move(0, 2, 1),
			(ids) => ids.lm.remove(1),
			(ids) => ids.lm.clear(),
			(ids) => ids.lm.append({ name: 'z', n: 9 }),
		],
		expect: (ids) => ids.summary.text === 'count 1 first z',
	},
	{
		name: 'TreeItemModel data reaches the bindings that read it',
		qml: `import QtQuick 2.0
Item { id: top; width: 300; height: 200
	TreeItemModel { id: tm }
	Text { id: label; text: tm.getItemsCount() + ":" + tm.getData("name", 0) + ":" + tm.isValidData("name", 0) }
	Text { id: cnt; text: "count " + tm.count }
}`,
		steps: [
			(ids) => ids.tm.insertNewItem(0),
			(ids) => ids.tm.setData('name', 'first', 0),
			(ids) => ids.tm.setData('name', 'second', 0),
			(ids) => ids.tm.insertNewItem(1),
			(ids) => ids.tm.removeItem(0),
			(ids) => (ids.tm.get(0).name = 'raw'),
		],
		expect: (ids) => ids.label.text === '1:raw:true',
	},
	{
		name: 'visible and enabled follow the ancestors, also after a reparent',
		qml: `import QtQuick 2.0
Item { id: top; width: 300; height: 200
	Item { id: a; Item { id: b; Rectangle { id: leaf; width: 10; height: 10 } } }
	Item { id: other; visible: false }
	Text { id: state; text: "v" + leaf.visible + " e" + leaf.enabled }
	MouseArea { id: area; enabled: b.enabled; anchors.fill: parent }
	Text { id: areaState; text: "a" + area.enabled + area.visible }
}`,
		steps: [
			(ids) => (ids.a.visible = false),
			(ids) => (ids.a.visible = true),
			(ids) => (ids.b.enabled = false),
			(ids) => (ids.leaf.parent = ids.other),
			(ids) => (ids.leaf.parent = ids.b),
			(ids) => (ids.b.enabled = true),
			(ids) => (ids.top.visible = false),
		],
		expect: (ids) => ids.state.text === 'vfalse etrue',
	},
	{
		name: 'the pointer drives containsMouse, pressed and what reads them',
		qml: `import QtQuick 2.0
Item { id: top; width: 300; height: 200
	MouseArea { id: area; hoverEnabled: true; x: 10; y: 10; width: 100; height: 100 }
	Rectangle { id: box; x: 150; width: 50; height: 50; color: area.containsMouse ? (area.pressed ? "red" : "green") : "blue" }
	Text { id: info; y: 100; text: "m" + area.containsMouse + " p" + area.pressed + " " + box.color }
}`,
		pointer: [
			['move', 50, 50],
			['down', 50, 50],
			['up', 50, 50],
			['move', 250, 150],
		],
		expect: (ids) => ids.info.text === 'mfalse pfalse #0000ff' || ids.info.text.startsWith('mfalse pfalse'),
	},
	{
		name: 'the focus and what reads it',
		qml: `import QtQuick 2.0
Item { id: top; width: 300; height: 200
	FocusScope { id: scope; width: 100; height: 100
		Item { id: first; focus: true; width: 10; height: 10 }
		Item { id: second; x: 20; width: 10; height: 10 }
	}
	Text { id: info; text: "f" + first.focus + second.focus + " a" + first.activeFocus + second.activeFocus }
}`,
		steps: [
			(ids) => ids.second.forceActiveFocus(),
			(ids) => ids.first.forceActiveFocus(),
			(ids) => (ids.first.focus = false),
		],
		expect: (ids) => typeof ids.info.text === 'string' && ids.info.text.startsWith('f'),
	},
	{
		name: 'geometry readers follow anchors, sizes and the content of a Row',
		qml: `import QtQuick 2.0
Item { id: top; width: 400; height: 300
	Rectangle { id: panel; x: 10; y: 10; width: 200; height: 100 }
	Rectangle { id: fill; anchors.fill: panel }
	Row { id: row; y: 150; spacing: 5
		Rectangle { id: r1; width: 20; height: 10 }
		Rectangle { id: r2; width: 30; height: 10 }
	}
	Text { id: info; y: 200; text: fill.width + "x" + fill.height + " row " + row.width + " x " + fill.x }
	Rectangle { id: follower; width: row.width / 2; height: panel.height / 4; x: panel.x + panel.width }
}`,
		steps: [
			(ids) => (ids.panel.width = 300),
			(ids) => (ids.r1.width = 60),
			(ids) => (ids.panel.x = 40),
			(ids) => (ids.r2.visible = false),
			(ids) => (ids.fill.anchors.margins = 5),
			(ids) => (ids.panel.height = 50),
		],
		expect: (ids) => ids.info.text.startsWith('290x40'),
	},
	{
		name: 'bindings replaced and restored by a Binding item',
		qml: `import QtQuick 2.0
Item { id: top; width: 400; height: 300
	property int source: 10
	property int wanted: source * 2
	Item { id: target; width: wanted }
	Binding { target: target; property: "width"; value: top.source + 1; when: top.source > 20 }
	Text { id: info; text: "w" + target.width }
}`,
		steps: [
			(ids) => (ids.top.source = 25),
			(ids) => (ids.top.source = 12),
			(ids) => (ids.top.source = 30),
		],
		expect: (ids) => ids.info.text === 'w31',
	},
	{
		name: 'a binding that failed starts to work when what it needs appears',
		qml: `import QtQuick 2.0
Item { id: top; width: 300; height: 200
	property var thing: null
	Text { id: info; text: "t" + (thing ? thing.width : "none") }
}`,
		steps: [(ids) => (ids.top.thing = ids.top), (ids) => (ids.top.width = 123), (ids) => (ids.top.thing = null)],
		expect: (ids) => ids.info.text === 'tnone',
	},
];

async function runScenario(scenario, incremental, audit) {
	const engine = newEngine(incremental, audit);
	const files = new Map([['Main.qml', scenario.qml]]);
	const fs = { readText: async (path) => files.get(path) ?? null, readBytes: async () => null };
	const scene = new Scene(new Renderer(null), engine);
	const viewer = new Viewer(scene, engine, () => {});
	scene.resize(500, 400, 1);
	const project = new Project(fs);
	await project.load('Main.qml');
	viewer.load(project);
	let clock = 0;
	const frames = (count) => {
		for (let i = 0; i < count; i++) {
			clock += 0.016;
			scene.root.$update();
			scene.drawFrame(clock);
		}
	};
	frames(6);
	const ids = viewer.content.$children[0].$scope.ids;
	const trail = [];
	for (const step of scenario.steps || []) {
		step(ids);
		frames(3);
		trail.push(digest(viewer.content));
	}
	for (const [kind, x, y] of scenario.pointer || []) {
		if (kind === 'move') scene.pointerMove(x, y);
		else if (kind === 'down') scene.pointerDown(x, y, 0);
		else scene.pointerUp(x, y);
		frames(3);
		trail.push(digest(viewer.content));
	}
	return { trail, misses: misses(engine), expected: scenario.expect(ids), errors: engine.errors.slice() };
}

async function main() {
	const selected = process.argv.slice(2);
	let failed = 0;
	const report = (name, problems) => {
		if (problems.length === 0) return;
		failed++;
		console.log(`[-] ${name}`);
		for (const problem of problems.slice(0, 5)) console.log(`      ${problem}`);
	};

	const selfTest = await auditDetectsMissedUpdate();
	if (selfTest !== '') {
		failed++;
		console.log(`[-] audit self test: ${selfTest}`);
	} else {
		console.log('[+] the audit sees an unannounced change');
	}

	let count = 0;
	for (const sample of (await readdir(join(root, 'qml'))).filter((name) => name.endsWith('.qml') && !/error/.test(name)).sort()) {
		if (selected.length > 0 && !selected.includes(sample)) continue;
		count++;
		const full = await runViewer(root, `qml/${sample}`, [], false, false);
		const fast = await runViewer(root, `qml/${sample}`, [], true, true);
		const problems = fast.misses.map((miss) => `missed update: ${miss}`);
		if (full.state !== fast.state) problems.push('the state differs from the full sweep');
		report(`qml/${sample}`, problems);
	}

	for (const { name, dir } of await caseNames()) {
		if (selected.length > 0 && !selected.includes(name)) continue;
		count++;
		const full = await runCase(dir, false, false);
		const fast = await runCase(dir, true, true);
		const problems = fast.misses.map((miss) => `missed update: ${miss}`);
		if (JSON.stringify(full.logs) !== JSON.stringify(fast.logs)) problems.push(`the log differs: ${JSON.stringify(full.logs).slice(0, 120)} / ${JSON.stringify(fast.logs).slice(0, 120)}`);
		if (full.state !== fast.state) problems.push('the state differs from the full sweep');
		if (full.threw !== fast.threw) problems.push(`threw '${fast.threw}' instead of '${full.threw}'`);
		report(name, problems);
	}

	for (const scenario of SCENARIOS) {
		if (selected.length > 0 && !selected.includes('scenarios')) continue;
		count++;
		const full = await runScenario(scenario, false, false);
		const fast = await runScenario(scenario, true, true);
		const problems = fast.misses.map((miss) => `missed update: ${miss}`);
		for (let i = 0; i < full.trail.length; i++) {
			if (full.trail[i] !== fast.trail[i]) {
				problems.push(`the state after step ${i + 1} differs from the full sweep`);
				break;
			}
		}
		if (!full.expected) problems.push('the full sweep does not reach the expected state: the scenario is stale');
		if (!fast.expected) problems.push('the incremental evaluation does not reach the expected state');
		if (fast.errors.length !== full.errors.length) problems.push(`${fast.errors.length} errors instead of ${full.errors.length}: ${fast.errors.join(' | ').slice(0, 200)}`);
		report(scenario.name, problems);
	}

	const gallery = join(root, 'controlsgalleryqml');
	if (existsSync(gallery) && (selected.length === 0 || selected.includes('gallery'))) {
		count++;
		const importPaths = ['qml/ImtCoreQml', 'qml'];
		const full = await runViewer(gallery, 'qml/ImtControlsGalleryOffline.qml', importPaths, false, false, clickThrough);
		const fast = await runViewer(gallery, 'qml/ImtControlsGalleryOffline.qml', importPaths, true, true, clickThrough);
		const problems = fast.misses.map((miss) => `missed update: ${miss}`);
		if (full.state !== fast.state) problems.push('the state differs from the full sweep');
		if (fast.errors !== full.errors) problems.push(`${fast.errors} errors instead of ${full.errors}`);
		report('gallery pages', problems);
	}

	console.log(failed === 0 ? `All good (${count} cases)` : `${failed} failed`);
	process.exit(failed === 0 ? 0 : 1);
}

main();
