// Loads every sample without a browser (no WebGL, no fonts) and checks that it loads, lays out and
// paints. `node test/run.mjs [file.qml ...]`
import { readFile, readdir } from 'node:fs/promises';
import { dirname, join, relative } from 'node:path';
import { fileURLToPath } from 'node:url';
import { Engine } from '../src/engine.js';
import { Project } from '../src/project.js';
import { Renderer } from '../src/renderer.js';
import { Scene } from '../src/scene.js';
import { Viewer } from '../src/viewer.js';

const root = join(dirname(fileURLToPath(import.meta.url)), '..');

class DiskFS {
	async readText(path) {
		try {
			return await readFile(join(root, path), 'utf8');
		} catch {
			return null;
		}
	}

	async readBytes(path) {
		try {
			const data = await readFile(join(root, path));
			return data.buffer.slice(data.byteOffset, data.byteOffset + data.byteLength);
		} catch {
			return null;
		}
	}
}

// Samples that must fail to load, with a part of the expected message.
const EXPECTED_ERRORS = { 'qml/error.qml': '', 'qml/import_error.qml': '' };

function walk(item, visit, depth = 0) {
	visit(item, depth);
	for (const child of item.$children) walk(child, visit, depth + 1);
}

async function check(path) {
	const engine = new Engine();
	const scene = new Scene(new Renderer(null), engine);
	const viewer = new Viewer(scene, engine, () => {});
	scene.resize(1000, 700, 1);

	const project = new Project(new DiskFS());
	let status = '';
	try {
		await project.load(path);
		viewer.load(project);
		status = viewer.statusLabel.$text;
	} catch (error) {
		status = `Error at ${error.line}:${error.column}: ${error.message}`;
		console.log(`${path}: load error: ${status}`);
		return path in EXPECTED_ERRORS;
	}

	for (let i = 0; i < 5; i++) scene.drawFrame(i * 0.016);
	scene.drawFrame(1);
	const counts = { items: 0 };
	walk(viewer.content, () => counts.items++);
	const failed = status.startsWith('Error') || engine.errors.length > 0;
	console.log(`${path}: ${status}; items now ${counts.items}; errors ${engine.errors.length}; layout ${scene.layoutMs.toFixed(1)} ms`);
	for (const message of engine.errors) console.log(`    ${message}`);
	if (path in EXPECTED_ERRORS) return failed;
	return !failed;
}

async function main() {
	let files = process.argv.slice(2);
	if (files.length === 0) {
		files = (await readdir(join(root, 'qml'))).filter((f) => f.endsWith('.qml')).map((f) => `qml/${f}`).sort();
	}
	let bad = 0;
	for (const file of files) {
		if (!(await check(relative(root, join(root, file))))) {
			bad++;
			console.log(`  FAILED: ${file}`);
		}
	}
	console.log(bad === 0 ? 'All good' : `${bad} failed`);
	process.exit(bad === 0 ? 0 : 1);
}

main();
