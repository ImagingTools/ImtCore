// How long it takes to turn the reply of a table request into the generated SDL objects (BaseClass.fromObject): the reply has
// thousands of nested objects (a row of the table, a measurement with its values, colors), each of them is a QML object.
//
//   node test/sdl-table.mjs [reply.json]       (a reply of GetMeasurementViewData; the default is a reply made here)
import { existsSync, readFileSync } from 'node:fs';
import { readFile } from 'node:fs/promises';
import { dirname, join, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';
import { Engine } from '../src/engine.js';
import { Project } from '../src/project.js';
import { Renderer } from '../src/renderer.js';
import { Scene } from '../src/scene.js';
import { Viewer } from '../src/viewer.js';
import { makeReply } from './sdl-table/reply.mjs';

const here = dirname(fileURLToPath(import.meta.url));
const imtCore = resolve(here, '..', '..', '..', '..');
const modules = [join(imtCore, 'Qml'), join(here, 'sdl-table')];

const MAIN = `
import QtQuick
import imtcontrols 1.0
import sdlTable 1.0

Item {
	id: root
	width: 400
	height: 400
	property alias view: view
	MeasurementViewData { id: view }
	function fill(data) { return view.fromObject(data) }
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

const ms = (value) => `${value.toFixed(0)} ms`;

async function measure(reply) {
	const engine = new Engine();
	engine.lenient = true;
	const scene = new Scene(new Renderer(null), engine);
	const viewer = new Viewer(scene, engine, () => {});
	scene.resize(400, 400, 1);
	const project = new Project(new DirFS(), { importPaths: modules });
	await project.load('Main.qml');
	viewer.load(project);
	for (let i = 0; i < 4; i++) scene.drawFrame(i * 0.016);
	const root = viewer.content.$children[0];
	const result = {};
	let start = performance.now();
	result.ok = root.fill(reply);
	result.fill = performance.now() - start;
	start = performance.now();
	for (let i = 0; i < 4; i++) {
		scene.root.$updateRequested = true;
		scene.drawFrame(1 + i * 0.016);
	}
	result.frames = performance.now() - start;
	result.rows = root.view.m_table.m_conditions.count === undefined ? -1 : root.view.m_table.m_conditions.get(0).item.m_rows.count;
	result.errors = engine.errors.length;
	return result;
}

const file = process.argv[2];
const reply = file !== undefined && existsSync(file) ? JSON.parse(readFileSync(file, 'utf8')).data.GetMeasurementViewData : makeReply(file === undefined ? 1512 : 0);
const r = await measure(reply);
console.log(`fromObject ${ms(r.fill)} (ok: ${r.ok}), 4 frames after it ${ms(r.frames)}, rows ${r.rows}, errors ${r.errors}`);

// The SDL objects are made from the BaseClass and BaseModel of the engine: 3.1 s with the QML files, under 1 s now. The limit is loose, it
// is there to catch a return of the cost, not to time a machine.
let failures = 0;
for (const [name, ok] of [['the reply is read', r.ok === true], ['all the rows are in the table', r.rows === (file === undefined ? 1512 : r.rows)], ['nothing failed', r.errors === 0], ['fromObject takes under 2 s', r.fill < 2000]]) {
	if (!ok) failures++;
	console.log(`${ok ? '[+]' : '[-]'} ${name}`);
}
console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
