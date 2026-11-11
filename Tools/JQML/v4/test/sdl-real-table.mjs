// The way a table of an application gets its data: the reply of the server (GetMeasurementViewData) is read into generated SDL objects
// (BaseClass.fromObject), the rows of the SDL table and the measurements that they point to are put into the model of the real `Table`
// control of imtcontrols, and the table shows them and scrolls. Every phase is timed.
//
//   node test/sdl-real-table.mjs [reply.json]      (a reply of the server; the default is a reply made here, 1512 rows)
//   QML_BASE=1 node test/sdl-real-table.mjs        (with the QML files of BaseClass and BaseModel instead of the classes of the engine)
import { existsSync, readFileSync } from 'node:fs';
import { readFile } from 'node:fs/promises';
import { dirname, join, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';
import { Engine } from '../src/engine.js';
import { BUILTIN_TYPES } from '../src/items.js';
import { Project } from '../src/project.js';
import { Renderer } from '../src/renderer.js';
import { Scene } from '../src/scene.js';
import { Viewer } from '../src/viewer.js';
import { makeReply } from './sdl-table/reply.mjs';

const here = dirname(fileURLToPath(import.meta.url));
const imtCore = resolve(here, '..', '..', '..', '..');
const modules = [join(imtCore, 'Qml'), join(here, 'sdl-table')];
const iot = join(imtCore, '..', 'IotPlatform', 'Include', 'iotqml', 'Qml');
if (existsSync(iot)) modules.push(iot);
if (!modules.slice(0, 2).every((dir) => existsSync(dir))) {
	console.log('skipped: the folders of the modules are missing');
	process.exit(0);
}

const MAIN = `
import QtQuick
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import sdlTable 1.0

Item {
	id: root
	width: 1200
	height: 700
	property alias table: table
	property alias headers: headerModel
	property alias rows: rowModel
	property alias view: view

	MeasurementViewData { id: view }
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

const COLUMNS = [['n', '#'], ['id', 'Id'], ['cyan', 'Cyan'], ['magenta', 'Magenta'], ['yellow', 'Yellow'], ['black', 'Black'], ['inactive', 'Inactive'], ['lab', 'Lab'], ['chroma', 'Chroma'], ['hue', 'Hue'], ['color', 'Color']];

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

function findAll(item, test, out = [], seen = new Set()) {
	if (item === null || item === undefined || seen.has(item)) return out;
	seen.add(item);
	if (test(item)) out.push(item);
	for (const child of item.$children || []) findAll(child, test, out, seen);
	return out;
}

// What the application does with the SDL objects: a row of the table joined with its measurement, a text for each column.
function tableRows(view) {
	const condition = view.m_table.m_conditions.get(0).item;
	const measurements = view.m_measurements.m_conditionList.get(0).item.m_measurementList;
	const byId = new Map();
	for (let i = 0; i < measurements.count; i++) {
		const measurement = measurements.get(i).item;
		byId.set(measurement.m_measurementId, measurement);
	}
	const rows = [];
	for (let i = 0; i < condition.m_rows.count; i++) {
		const row = condition.m_rows.get(i).item;
		const measurement = byId.get(row.m_id);
		const lab = measurement.m_view.m_lab;
		const index = row.m_index;
		rows.push({
			id: row.m_id,
			n: String(i + 1),
			cyan: String(index[0]),
			magenta: String(index[1]),
			yellow: String(index[2]),
			black: String(index[3]),
			inactive: row.m_inactive ? 'yes' : 'no',
			lab: `${lab.m_l} ${lab.m_a} ${lab.m_b}`,
			chroma: measurement.m_view.m_chroma.toFixed(1),
			hue: measurement.m_view.m_hue.toFixed(1),
			color: measurement.m_view.m_preview.m_color,
		});
	}
	return rows;
}

if (process.env.QML_BASE) for (const name of ['BaseClass', 'BaseModel']) BUILTIN_TYPES.delete(name);
const ms = (value) => `${value.toFixed(0)} ms`;
const file = process.argv[2];
const reply = file !== undefined && existsSync(file) ? JSON.parse(readFileSync(file, 'utf8')).data.GetMeasurementViewData : makeReply(1512);

const engine = new Engine();
engine.lenient = true;
const scene = new Scene(new Renderer(null), engine);
const viewer = new Viewer(scene, engine, () => {});
scene.resize(1200, 700, 1);
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
root.headers.createFromJson(JSON.stringify(COLUMNS.map(([id, name]) => ({ id, name }))));
root.table.headers = root.headers;
frames(10);

const result = {};
let start = performance.now();
result.read = root.view.fromObject(reply);
result.readTime = performance.now() - start;

start = performance.now();
const rows = tableRows(root.view);
result.joinTime = performance.now() - start;

start = performance.now();
root.rows.clear();
root.rows.createFromJson(JSON.stringify(rows));
result.fillTime = performance.now() - start;

start = performance.now();
const shown = frames(12);
result.showTime = shown.reduce((a, b) => a + b, 0);
const list = root.table.elementsList;
const first = list.$delegates.get(0);
const texts = first === undefined ? [] : findAll(first, (item) => item.$typeName === 'Text').map((item) => String(item.text));

// Scrolling: jumps and the wheel.
const maxY = list.contentHeight - list.height;
const jumps = [];
for (let i = 0; i < 10; i++) {
	list.contentY = (i / 9) * maxY;
	jumps.push(frames(1)[0]);
	frames(2);
}
const steps = [];
list.contentY = 0;
frames(3);
for (let i = 0; i < 30; i++) {
	scene.wheel(600, 300, -1);
	steps.push(frames(1)[0]);
}
const last = list.$delegates.get(list.count - 1);
list.contentY = maxY;
frames(4);
const lastRow = [...list.$delegates.values()].map((delegate) => findAll(delegate, (item) => item.$typeName === 'Text').map((item) => String(item.text)));
const lastId = rows[rows.length - 1].id;
const errors = engine.errors.filter((error) => !/ImtbaseSettingsSdlCommandIds/.test(String(error)));

console.log(`read the reply into SDL objects ${ms(result.readTime)}, join the rows ${ms(result.joinTime)}, fill the table model ${ms(result.fillTime)}, show (12 frames) ${ms(result.showTime)}`);
console.log(`the table: ${list.count} rows, ${list.$delegates.size} delegates, jump ${ms(Math.max(...jumps))} worst, wheel ${ms(Math.max(...steps))} worst`);

let failures = 0;
const check = (name, ok) => {
	if (!ok) failures++;
	console.log(`${ok ? '[+]' : '[-]'} ${name}`);
};
check('the reply is read', result.read === true);
check('the rows of the SDL table are all in the table', list.count === rows.length && rows.length === root.view.m_table.m_conditions.get(0).item.m_rows.count);
check('the first row shows its own values', texts.includes(rows[0].id) && texts.includes(rows[0].lab) && texts.includes(rows[0].color));
check('the table builds the delegates of the rows in view only', list.$delegates.size < 100);
check('the last row is shown at the end of the scroll', lastRow.some((row) => row.includes(lastId)));
check('nothing failed', errors.length === 0);
if (!process.env.QML_BASE) check('the reply is read in under 2 s', result.readTime < 2000);
console.log(failures === 0 ? 'All good' : `${failures} failed`);
if (errors.length > 0) console.log(errors.slice(0, 5).map(String).join('\n'));
process.exit(failures === 0 ? 0 : 1);
