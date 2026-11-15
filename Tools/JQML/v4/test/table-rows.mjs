// A table like the device table of IotPlatform (the Table control, 24 columns, 40 px rows, cache buffer 500) that shows rows made here, not
// asked from a server: how long it takes to put 500, 5000 or 50000 rows into it, and how it scrolls then. The table builds the delegates of the
// rows in view only, so what it costs to scroll should not depend on how many rows the model has; only making the rows should.
//
//   node test/table-rows.mjs [count ...]      (the numbers of rows; the default is 500 5000 50000)
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
	console.log(`The table needs the folders ${modules.join(' and ')}: skipped`);
	process.exit(0);
}

// The columns of the device table that are shown or that a row holds: the ids are the keys of the rows.
const COLUMNS = [
	'n', 'deviceNumber', 'modemNumber', 'accountNumber', 'fullAddress', 'readingsDate', 'charge', 'modelName', 'systemType', 'resourceType',
	'serialNumber', 'status', 'rssi_snr', 'valveState', 'waterTemperature', 'primaryValue', 'readings', 'installDate', 'verifDate',
	'nextVerifDate', 'abonentName', 'abonentPhone', 'manufacturer', 'imei',
];

const MAIN = `
import QtQuick
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

Item {
	id: root
	width: 1500
	height: 900
	property alias table: table
	property alias headers: headerModel
	property alias rows: rowModel

	TreeItemModel { id: headerModel }
	TreeItemModel { id: rowModel }

	Table {
		id: table
		anchors.fill: parent
		itemHeight: 40
		headerHeight: 40
		headerGap: 16
		isFlickable: true
		enableAlternating: true
		cacheBuffer: 500
		elements: rowModel
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

const ms = (value) => `${value.toFixed(1)} ms`;
const median = (values) => [...values].sort((a, b) => a - b)[Math.floor(values.length / 2)];

// The rows the way the reply of the server makes them: a text for each column.
function makeRows(count) {
	const rows = new Array(count);
	for (let i = 0; i < count; i++) {
		const row = {};
		for (let c = 0; c < COLUMNS.length; c++) row[COLUMNS[c]] = c === 0 ? String(i + 1) : `${COLUMNS[c]} ${i}`;
		row.id = `device${i}`;
		rows[i] = row;
	}
	return rows;
}

async function open() {
	const engine = new Engine();
	engine.lenient = true;
	const scene = new Scene(new Renderer(null), engine);
	const viewer = new Viewer(scene, engine, () => {});
	scene.resize(1500, 900, 1);
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
	frames(6);
	const root = viewer.content.$children[0];
	const headers = COLUMNS.map((id) => ({ id, name: id }));
	root.headers.createFromJson(JSON.stringify(headers));
	// The table counts its columns when it gets the headers: they are given once they are there, as the page does.
	root.table.headers = root.headers;
	frames(10);
	return { engine, scene, frames, root, table: root.table };
}

function findAll(item, test, out = [], seen = new Set()) {
	if (item === null || item === undefined || seen.has(item)) return out;
	seen.add(item);
	if (test(item)) out.push(item);
	for (const child of item.$children || []) findAll(child, test, out, seen);
	return out;
}

async function measure(count, setup) {
	const { engine, scene, frames, root, table } = setup;
	const result = { count };
	// A page starts at its first row.
	table.elementsList.contentY = 0;
	frames(3);

	// The model: the rows are made, then put into the model (what the data controller of the collection does with the reply).
	let start = performance.now();
	const rows = makeRows(count);
	result.make = performance.now() - start;
	start = performance.now();
	const json = JSON.stringify(rows);
	result.stringify = performance.now() - start;
	start = performance.now();
	root.rows.clear();
	root.rows.createFromJson(json);
	result.fill = performance.now() - start;

	// The table shows them: the frames that follow.
	const shown = frames(12);
	result.show = shown.reduce((a, b) => a + b, 0);
	result.firstFrame = shown[0];
	const list = table.elementsList;
	result.rows = list.count;
	result.delegates = list.$delegates.size;
	// The first row shows its own texts (a table that shows nothing would be fast too).
	const first = list.$delegates.get(0);
	const texts = first === undefined ? [] : findAll(first, (item) => item.$typeName === 'Text').map((item) => String(item.text));
	result.shows = texts.includes('deviceNumber 0') && texts.includes('fullAddress 0');
	result.idle = median(frames(30));
	result.bindings = engine.bindings.size;

	// Scrolling: jumps (the scroll bar dragged) and steps (the wheel).
	const maxY = list.contentHeight - list.height;
	const jump = [];
	let seed = 12345;
	const random = () => (seed = (seed * 1103515245 + 12345) & 0x7fffffff) / 0x7fffffff;
	for (let i = 0; i < 20; i++) {
		list.contentY = i === 0 ? 0 : random() * maxY;
		jump.push(frames(1)[0]);
		frames(3);
	}
	result.jump = median(jump);
	result.jumpWorst = Math.max(...jump);
	const step = [];
	list.contentY = 0;
	frames(3);
	for (let i = 0; i < 60; i++) {
		scene.wheel(600, 300, -1);
		step.push(frames(1)[0]);
	}
	result.step = median(step);
	result.stepWorst = Math.max(...step);
	// The module of the client style asks for a command id that this test does not load.
	result.errors = engine.errors.filter((error) => !/ImtbaseSettingsSdlCommandIds/.test(String(error))).length;
	return result;
}

const counts = process.argv.slice(2).map(Number).filter((n) => n > 0);
// The first page builds the table itself; the pages after it are the switch of the sizes. The sizes go up, then down again.
if (counts.length === 0) counts.push(500, 500, 5000, 50000, 5000, 500);

const setup = await open();
const table = setup.table;
console.log(`the table: ${table.headers.getItemsCount()} columns, ${table.itemHeight} px rows`);
let page = 0;
const results = [];
for (const count of counts) {
	const r = await measure(count, setup);
	results.push(r);
	console.log(
		`${String(count).padStart(6)} rows${page++ === 0 ? ' (the table is built)' : ''}: make ${ms(r.make)}, json ${ms(r.stringify)}, into the model ${ms(r.fill)}\n` +
			`        shown in ${ms(r.show)} (first frame ${ms(r.firstFrame)}), ${r.rows} rows, ${r.delegates} delegates, ${r.bindings} bindings, idle frame ${ms(r.idle)}\n` +
			`        scroll: jump ${ms(r.jump)} (worst ${ms(r.jumpWorst)}), wheel step ${ms(r.step)} (worst ${ms(r.stepWorst)})` +
			`${r.shows ? '' : ', THE FIRST ROW SHOWS NO DATA'}${r.errors ? `, ${r.errors} errors` : ''}`,
	);
}

let failures = 0;
const check = (name, ok, detail = '') => {
	if (!ok) failures++;
	console.log(`${ok ? '[+]' : '[-]'} ${name}${ok ? '' : `: ${detail}`}`);
};
check('every row shows its data and nothing failed', results.every((r) => r.shows && r.errors === 0));
if (!process.argv[2]) {
	// Pages 2 to 6, the table is built already: 500, 5000, 50000, 5000, 500 rows.
	const pages = results.slice(1);
	const small = pages[0];
	const big = pages[2];
	check('the table builds the same number of delegates and bindings for any number of rows', pages.every((r) => r.delegates === small.delegates && r.bindings === small.bindings), pages.map((r) => `${r.delegates}/${r.bindings}`).join(' '));
	check('50000 rows are shown in about the time of 500 (not 100 times)', big.show < small.show * 3 + 200, `${ms(big.show)} against ${ms(small.show)}`);
	check('a jump of the scroll bar costs the same', big.jump < small.jump * 3 + 20, `${ms(big.jump)} against ${ms(small.jump)}`);
	check('so does a step of the wheel', big.step < small.step * 3 + 20, `${ms(big.step)} against ${ms(small.step)}`);
	check('so does a frame that does nothing', big.idle < small.idle * 3 + 10, `${ms(big.idle)} against ${ms(small.idle)}`);
}
console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
