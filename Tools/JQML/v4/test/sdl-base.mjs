// BaseClass and BaseModel are QML files on the desktop (Qml/imtcontrols/Base) and classes of the engine in the browser.
// This runs the same script over the generated SDL objects of test/sdl-table with the classes of the engine and with the QML files and
// compares what the two did: the results, the JSON, the GraphQL and the changes that the objects reported.
//
//   node test/sdl-base.mjs
import { existsSync } from 'node:fs';
import { readFile } from 'node:fs/promises';
import { dirname, join, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';
import { Engine } from '../src/engine.js';
import { BUILTIN_TYPES } from '../src/items.js';
import { Project } from '../src/project.js';
import { Renderer } from '../src/renderer.js';
import { Scene } from '../src/scene.js';
import { Viewer } from '../src/viewer.js';

const here = dirname(fileURLToPath(import.meta.url));
const imtCore = resolve(here, '..', '..', '..', '..');
const modules = [join(imtCore, 'Qml'), join(here, 'sdl-table')];
if (!modules.every((dir) => existsSync(dir))) {
	console.log('skipped: the folders of the modules are missing');
	process.exit(0);
}

const MAIN = `
import QtQuick
import imtcontrols 1.0
import sdlTable 1.0

Item {
	id: root
	width: 100
	height: 100
	property alias view: view
	property alias other: other
	MeasurementViewData { id: view }
	MeasurementViewData { id: other }
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

	async readBytes() {
		return null;
	}
}

const lab = { __typename: 'Lab', L: 62, a: -44.5, b: -50 };
const preview = { __typename: 'RgbColorHex', Color: '#00adee' };
const colorant = (name) => ({ __typename: 'ColorantItem', colorantName: name, colorPreview: { __typename: 'ColorantProperties', chroma: 66.6, hue: 228.6, lab, preview } });
const reply = (rows) => ({
	__typename: 'MeasurementViewData',
	measurements: {
		__typename: 'MeasurementContainer',
		colorSpecification: { __typename: 'SpectralColorMeasurementSpecification', aperture: '', backing: 'Sub"strate\\', calibrationStandard: 'XRGA', deviceInfo: { __typename: 'DeviceInfo', deviceClass: 'NotSet', deviceModel: 'i1', devicePolarization: 'NotSet', filterType: 'NotSet', illuminationType: 'Incandescent', manufacturer: 'X-Rite', serialNumber: '1\n2' }, measurementType: 'Reflectance', spectrumInfo: { __typename: 'SpectrumInfo', end: 730, start: 380, step: 10 }, spectrumType: 'Reflective' },
		colorantList: [colorant('Cyan'), colorant('Magenta')],
		conditionList: [{ __typename: 'Condition', conditionId: 'M0', measurementList: Array.from({ length: rows }, (_, i) => ({ __typename: 'MeasurementItem', measurementId: `id-${i}`, value: { __typename: 'MeasurementValue', index: [0.1 * i, 0.2], value: [0.5, 1.5, i] }, view: { __typename: 'MeasurementValueView', chroma: 33.9, hue: 120.5, lab, preview } })) }],
	},
	table: { __typename: 'MeasurementTableViewData', colorantList: [colorant('Cyan')], conditions: [{ __typename: 'TableViewCondition', id: 'M0', rows: Array.from({ length: rows }, (_, i) => ({ __typename: 'TableViewRow', id: `id-${i}`, inactive: i % 2 === 0, index: [0.1 * i, 0.2, 'x'], measurementIndex: i })) }], inkSetPreview: [preview, preview], type: 'Press' },
});

// What an object reports: the names of the changed properties and the types of the objects that report them.
const names = (changeSet) => (Array.isArray(changeSet) ? changeSet.map((c) => `${c.name}@${c.sender && c.sender.__typename}`) : changeSet);

async function scenario() {
	const engine = new Engine();
	engine.lenient = true;
	const scene = new Scene(new Renderer(null), engine);
	const viewer = new Viewer(scene, engine, () => {});
	scene.resize(100, 100, 1);
	const project = new Project(new DirFS(), { importPaths: modules });
	await project.load('Main.qml');
	viewer.load(project);
	for (let i = 0; i < 4; i++) scene.drawFrame(i * 0.016);
	const root = viewer.content.$children[0];
	const log = [];
	const step = (name, fn) => {
		try {
			log.push([name, fn()]);
		} catch (error) {
			log.push([name, `throws ${error && error.message}`]);
		}
	};
	const view = root.view;
	const events = [];
	view.modelChanged.connect((changeSet) => events.push(names(changeSet)));
	const flush = () => events.splice(0);
	const rowsOf = () => view.m_table.m_conditions.get(0).item.m_rows;

	step('empty toJson', () => view.toJson());
	step('fromObject', () => view.fromObject(reply(3)));
	step('events of fromObject', flush);
	step('keys', () => view.getProperties());
	step('toJson', () => view.toJson());
	step('toGraphQL', () => view.toGraphQL());
	step('rows', () => `${view.m_table.m_conditions.count},${rowsOf().count}`);
	step('owner of a row', () => rowsOf().get(1).item.owner.__typename);
	step('write of a property', () => {
		view.m_table.m_type = 'Other';
		return flush();
	});
	step('same value', () => {
		view.m_table.m_type = 'Other';
		return flush();
	});
	step('write deep', () => {
		view.m_measurements.m_colorSpecification.m_deviceInfo.m_manufacturer = 'M';
		return flush();
	});
	step('notifications off', () => {
		view.m_table.enableNotifications = false;
		view.m_table.m_type = 'Third';
		view.m_table.enableNotifications = true;
		return flush();
	});
	step('transaction', () => {
		view.beginChanges();
		view.m_table.m_type = 'A';
		view.m_table.m_type = 'B';
		view.endChanges();
		return flush();
	});
	step('transaction twice', () => {
		view.beginChanges();
		view.beginChanges();
		view.endChanges();
		view.endChanges();
		return flush();
	});
	step('model element appended', () => {
		const rows = rowsOf();
		const copy = rows.get(0).item.copyMe();
		rows.appendElement(copy);
		return [rows.count, flush(), copy.owner === view.m_table.m_conditions.get(0).item];
	});
	step('model element removed', () => {
		const rows = rowsOf();
		rows.removeElement(0);
		return [rows.count, flush()];
	});
	step('model getData', () => {
		const rows = rowsOf();
		return [rows.getData('m_id', 0), rows.getData('m_id', 99), rows.containsKey('m_id', 0), rows.containsKey('m_nothing', 0), rows.getItemsCount()];
	});
	step('model setProperty', () => {
		const rows = rowsOf();
		rows.setProperty(0, 'm_id', 'changed');
		return [rows.getData('m_id', 0), flush()];
	});
	step('model swapItems', () => {
		const rows = rowsOf();
		const before = [rows.getData('m_id', 0), rows.getData('m_id', 1)];
		const ok = rows.swapItems(0, 1);
		return [before, ok, rows.getData('m_id', 0), rows.getData('m_id', 1), rows.swapItems(0, 99)];
	});
	step('model json', () => rowsOf().toJson());
	step('model graphql', () => rowsOf().toGraphQL());
	step('copyMe', () => {
		const copy = view.copyMe();
		return [copy.toJson() === view.toJson(), view.isEqualWithModel(copy), copy.m_table.m_type];
	});
	step('copy differs', () => {
		const copy = view.copyMe();
		copy.m_table.m_type = 'Differs';
		return [view.isEqualWithModel(copy), copy.isEqualWithModel(view)];
	});
	step('copyFrom', () => `${root.other.copyFrom(view)}:${root.other.toJson() === view.toJson()}`);
	step('copyTo', () => `${view.m_table.copyTo(root.other.m_table)}:${root.other.m_table.m_type}`);
	step('fromJSON', () => {
		const target = view.createMe();
		return [target.fromJSON(view.toJson()), target.toJson() === view.toJson()];
	});
	step('fromJSON of a broken text', () => view.fromJSON('{broken'));
	step('model fromJSON', () => {
		const rows = rowsOf();
		const model = rows.copyMe();
		const json = rows.toJson();
		model.fromJSON(json);
		return [model.count, model.toJson() === json, model.isEqualWithModel(rows)];
	});
	step('removeKey', () => {
		view.m_table.removeKey('m_type');
		return [view.m_table.m_type, view.m_table._internal.removed, view.m_table.toJson()];
	});
	step('a removed key comes back', () => {
		view.m_table.fromObject({ type: 'Back', conditions: [], colorantList: [], inkSetPreview: [] });
		return [view.m_table._internal.removed, view.m_table.m_type, view.m_table.toJson()];
	});
	step('null clears an object', () => {
		view.fromObject({ measurements: null, table: null });
		return [view.m_measurements, view.m_table, view.toJson()];
	});
	step('a required array may not be null', () => view.fromObject({ table: { conditions: null } }));
	step('an array with a null', () => {
		const ok = view.fromObject({ table: { __typename: 'MeasurementTableViewData', conditions: [null, { __typename: 'TableViewCondition', id: 'x', rows: [] }], colorantList: [], inkSetPreview: [] } });
		return [ok, view.m_table.m_conditions.count, view.m_table.m_conditions.get(0).item];
	});
	step('missing keys clear what is not in the reply', () => {
		view.fromObject(reply(2));
		view.fromObject({ table: { __typename: 'MeasurementTableViewData', conditions: [], colorantList: [], inkSetPreview: [], type: 'Only' } });
		return [view.m_measurements === null, view.m_table.m_type, view.toJson()];
	});
	step('a value of the wrong kind in an array', () => view.fromObject({ table: { rows: 5, conditions: [3] } }));
	step('finished is emitted', () => {
		let count = 0;
		root.other.finished.connect(() => count++);
		root.other.fromObject(reply(1));
		return count;
	});
	step('errors', () => engine.errors.length);
	return log;
}

const toCompare = (log) => log.map(([name, value]) => `${name}: ${JSON.stringify(value)}`);

async function run(native) {
	const saved = ['BaseClass', 'BaseModel'].map((name) => [name, BUILTIN_TYPES.get(name)]);
	if (!native) for (const [name] of saved) BUILTIN_TYPES.delete(name);
	try {
		return toCompare(await scenario());
	} finally {
		for (const [name, type] of saved) BUILTIN_TYPES.set(name, type);
	}
}

const qml = await run(false);
const native = await run(true);
if (process.env.DUMP) console.log(native.map((line) => line.slice(0, 160)).join(String.fromCharCode(10)));
let failures = 0;
for (let i = 0; i < Math.max(qml.length, native.length); i++) {
	if (qml[i] === native[i]) continue;
	failures++;
	console.log(`[-] ${String(qml[i] || native[i]).split(':')[0]}\n      QML:    ${String(qml[i]).slice(0, 400)}\n      engine: ${String(native[i]).slice(0, 400)}`);
}
console.log(`${qml.length} steps, ${failures} differ`);
console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
