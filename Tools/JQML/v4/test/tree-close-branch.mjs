// How long it takes to open and to close a branch of the address tree of IotPlatform (TreeViewGql, the tree of AddressTreeView) when the
// branch has many elements. The tree is the real control, the rows are made here the way the reply of the address request makes them.
//
//   node test/tree-close-branch.mjs [count ...]      (the numbers of elements of the branch to time; the default is 10 100 1000)
import { existsSync } from 'node:fs';
import { readFile } from 'node:fs/promises';
import { dirname, join, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';
import { Engine } from '../src/engine.js';
import { Project } from '../src/project.js';
import { Renderer } from '../src/renderer.js';
import { Scene } from '../src/scene.js';
import { Viewer } from '../src/viewer.js';

const here = dirname(fileURLToPath(import.meta.url));
const imtCore = resolve(here, '..', '..', '..', '..');
const modules = [join(imtCore, 'Qml'), join(imtCore, '..', 'IotPlatform', 'Include', 'iotqml', 'Qml')];
if (!modules.every((dir) => existsSync(dir))) {
	console.log(`The tree needs the folders ${modules.join(' and ')}: skipped`);
	process.exit(0);
}

const MAIN = `
import QtQuick
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

Item {
	id: root
	width: 400
	height: 800
	property alias tree: tree
	property int kids: 1000
	property int requests: 0

	TreeItemModel { id: reply }

	// What the reply of the address request is turned into: the rows of the branch are inserted under the row that was opened.
	function fill(index) {
		root.requests++
		var items = []
		for (var i = 0; i < root.kids; i++) {
			items.push({ id: "house" + i, name: "House " + i, fullAddress: "Street " + index + ", house " + i, hasChildren__: false, parentIds: "" })
		}
		reply.clear()
		reply.createFromJson(JSON.stringify(items))
		tree.insertTree(index, reply)
		reply.clear()
	}

	TreeViewGql {
		id: tree
		anchors.fill: parent
		hasSelection: true
		scrollSize: 8
		onRequestSignal: root.fill(index)
	}

	Component.onCompleted: {
		var streets = []
		for (var i = 0; i < 20; i++) {
			streets.push({ id: "street" + i, name: "Street " + i, fullAddress: "Street " + i, hasChildren__: true, parentIds: "" })
		}
		reply.createFromJson(JSON.stringify(streets))
		tree.insertTree(-1, reply)
		reply.clear()
	}
}
`;

class DirFS {
	async readText(path) {
		if (path === 'Main.qml') return MAIN;
		try {
			return await readFile(path, 'utf8');
		} catch {
			return null;
		}
	}

	async readBytes(path) {
		try {
			const data = await readFile(path);
			return data.buffer.slice(data.byteOffset, data.byteOffset + data.byteLength);
		} catch {
			return null;
		}
	}
}

async function open() {
	const engine = new Engine();
	engine.lenient = true;
	const scene = new Scene(new Renderer(null), engine);
	const viewer = new Viewer(scene, engine, () => {});
	scene.resize(400, 800, 1);
	const project = new Project(new DirFS(), { importPaths: modules });
	await project.load('Main.qml');
	viewer.load(project);
	let t = 0;
	const frames = (n = 6) => {
		const times = [];
		for (let i = 0; i < n; i++) {
			scene.root.$updateRequested = true;
			const start = performance.now();
			scene.drawFrame((t += 0.016));
			times.push(performance.now() - start);
		}
		return times;
	};
	frames(10);
	return { engine, viewer, frames, root: viewer.content.$children[0] };
}

function findAll(item, test, out = [], seen = new Set()) {
	if (item === null || item === undefined || seen.has(item)) return out;
	seen.add(item);
	if (test(item)) out.push(item);
	for (const child of item.$children || []) findAll(child, test, out, seen);
	return out;
}

// The arrow of the row that is at `index`: the button of its delegate, which opens or closes the branch.
function arrowOf(tree, index) {
	const list = findAll(tree, (item) => item.$typeName === 'ListView')[0];
	const delegate = list.$delegates.get(index);
	if (delegate === undefined) throw new Error(`the row ${index} has no delegate (rows: ${list.$delegates.size})`);
	const button = findAll(delegate, (item) => typeof item.openButtonClicked === 'function')[0];
	if (button === undefined) throw new Error(`the row ${index} has no button`);
	return { button, list };
}

const ms = (value) => `${value.toFixed(1)} ms`;

async function measure(kids) {
	const { engine, root, frames } = await open();
	root.kids = kids;
	const tree = root.tree;
	const row = 3;
	const result = { kids };

	// Opens the branch: the request is answered at once, so the rows are inserted inside the click.
	let start = performance.now();
	arrowOf(tree, row).button.openButtonClicked();
	result.open = performance.now() - start;
	result.openFrames = frames(4);
	result.rowsOpen = tree.model.getItemsCount();

	// Closes it: more than ten visible rows are deleted from the model, fewer are hidden.
	start = performance.now();
	arrowOf(tree, row).button.openButtonClicked();
	result.close = performance.now() - start;
	result.closeFrames = frames(4);
	result.rowsClosed = tree.model.getItemsCount();

	// Opens it again and closes it again.
	start = performance.now();
	arrowOf(tree, row).button.openButtonClicked();
	result.reopen = performance.now() - start;
	frames(4);
	start = performance.now();
	arrowOf(tree, row).button.openButtonClicked();
	result.close2 = performance.now() - start;
	result.close2Frames = frames(4);
	result.errors = engine.errors.length;
	return result;
}

const counts = process.argv.slice(2).map(Number).filter((n) => n > 0);
const explicit = counts.length > 0;
if (!explicit) counts.push(10, 100, 1000);
let failures = 0;
for (const kids of counts) {
	const r = await measure(kids);
	console.log(
		`${String(kids).padStart(5)} elements: open ${ms(r.open)} (+ frames ${r.openFrames.map(ms).join(', ')}), rows ${r.rowsOpen}\n` +
			`        close ${ms(r.close)} (+ frames ${r.closeFrames.map(ms).join(', ')}), rows ${r.rowsClosed}\n` +
			`        reopen ${ms(r.reopen)}, close again ${ms(r.close2)} (+ frames ${r.close2Frames.map(ms).join(', ')})` +
			`${r.errors ? `, ${r.errors} errors` : ''}`,
	);
	if (kids === 1000 && !explicit) {
		// A script that changes a model thousands of times marks the readers of the model once, not once for each change: opening a
		// branch of 1000 rows took 7 seconds the second time, while the readers of the model were marked for each of its 10000 changes.
		for (const [name, value, limit] of [['open', r.open, 1500], ['reopen', r.reopen, 1500], ['close', r.close, 2500]]) {
			const ok = value < limit;
			if (!ok) failures++;
			console.log(`${ok ? '[+]' : '[-]'} a branch of 1000 elements: ${name} takes under ${limit / 1000} s${ok ? '' : ` (${ms(value)})`}`);
		}
		if (r.errors) failures++;
		// Closing deletes the branch row by row and reads a size after each row: the list is arranged once, in the next frame (it was
		// arranged, with all its delegates rebound, for every row: 4.7 s).
	}
}
console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
