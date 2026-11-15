// Compares this engine with desktop Qt on input: each case is a folder with Main.qml that holds a QtTest TestCase. Qt runs it with
// `qmltestrunner`, the engine runs the `test_*` functions of the same file in a Scene, where `mouseWheel` of the TestCase sends the
// event to the scene. The lines that the functions print through console.log must match.
//
//   node test/input/run.mjs [Name]
//   QT_QMLTESTRUNNER=C:\Qt\...\bin\qmltestrunner.exe node test/input/run.mjs

import { spawn } from 'node:child_process';
import { readFile, readdir, stat } from 'node:fs/promises';
import { dirname, isAbsolute, join } from 'node:path';
import { fileURLToPath } from 'node:url';
import { Engine } from '../../src/engine.js';
import { Project } from '../../src/project.js';
import { Renderer } from '../../src/renderer.js';
import { Scene } from '../../src/scene.js';
import { Viewer } from '../../src/viewer.js';

const here = dirname(fileURLToPath(import.meta.url));
const TIMEOUT_MS = 120000;

const color = { reset: '\x1b[0m', red: '\x1b[31m', green: '\x1b[32m', yellow: '\x1b[33m' };

class DirFS {
	constructor(dir) {
		this.dir = dir;
	}

	async readText(path) {
		try {
			return await readFile(isAbsolute(path) ? path : join(this.dir, path), 'utf8');
		} catch {
			return null;
		}
	}

	async readBytes() {
		return null;
	}
}

function qmlLines(raw) {
	const lines = [];
	for (const line of String(raw).split(/\r?\n/)) {
		const match = /\bqml:\s+(?!warning:)(.*)$/.exec(line);
		if (match) lines.push(match[1]);
	}
	return lines;
}

function runDesktop(runner, mainFile) {
	const env = { ...process.env, QT_ASSUME_STDERR_HAS_CONSOLE: '1' };
	if (!env.QT_QPA_PLATFORM) env.QT_QPA_PLATFORM = 'offscreen';
	return new Promise((resolvePromise, reject) => {
		const child = spawn(runner, ['-input', mainFile], { cwd: dirname(mainFile), env, windowsHide: true });
		const chunks = [];
		child.stdout.on('data', (chunk) => chunks.push(chunk));
		child.stderr.on('data', (chunk) => chunks.push(chunk));
		const timer = setTimeout(() => {
			child.kill();
			reject(new Error(`timed out after ${TIMEOUT_MS} ms`));
		}, TIMEOUT_MS);
		child.on('error', (error) => {
			clearTimeout(timer);
			reject(error);
		});
		child.on('close', () => {
			clearTimeout(timer);
			const raw = Buffer.concat(chunks).toString('utf8');
			const failed = raw.split(/\r?\n/).filter((line) => /^FAIL!?\s*:/.test(line)).join(' | ');
			resolvePromise({ lines: qmlLines(raw), error: failed });
		});
	});
}

// The TestCase of a file: the engine knows no QtTest, so the runner gives its object what the functions call.
function shim(testCase, scene, clock, docRoot) {
	const frames = (seconds) => {
		const count = Math.max(1, Math.round(seconds / 0.016));
		for (let i = 0; i < count; i++) {
			scene.root.$updateRequested = true;
			scene.drawFrame((clock.t += 0.016));
		}
	};
	testCase.mouseWheel = (item, x, y, xDelta, yDelta) => {
		// mapToItem(null) is relative to the root of the document, which the viewer does not put at the corner of the scene.
		const point = item.mapToItem(null, x, y);
		scene.wheel(point.x + docRoot.$sx, point.y + docRoot.$sy, yDelta / 120, xDelta / 120);
		frames(0.032);
	};
	testCase.wait = (ms) => frames(ms / 1000);
	frames(0.1);
}

async function runWeb(dir) {
	const logs = [];
	const engine = new Engine();
	engine.lenient = true;
	engine.onLog = (line) => logs.push(line);
	const scene = new Scene(new Renderer(null), engine);
	const viewer = new Viewer(scene, engine, () => {});
	scene.resize(2400, 600, 1);
	const project = new Project(new DirFS(dir), {});
	try {
		await project.load('Main.qml');
		viewer.load(project);
		const clock = { t: 0 };
		for (let i = 0; i < 6; i++) {
			scene.root.$updateRequested = true;
			scene.drawFrame((clock.t += 0.016));
		}
		const found = [];
		for (const value of Object.values(engine.documentScope.ids)) {
			if (value && value.$typeName === 'TestCase') found.push(value);
		}
		for (const testCase of found) {
			shim(testCase, scene, clock, viewer.content.$children[0]);
			const names = Object.keys(testCase).filter((name) => /^test_/.test(name));
			for (const name of names) testCase[name]();
		}
		// `when: windowShown` only means something to qmltestrunner.
		const errors = engine.errors.map(String).filter((text) => !/'when'.*windowShown/.test(text));
		return { lines: logs, error: errors.slice(0, 3).join(' | ') };
	} catch (error) {
		return { lines: logs, error: error.stack || error.message };
	}
}

async function cases(selected) {
	const found = [];
	for (const name of (await readdir(here)).sort()) {
		if (selected && name !== selected) continue;
		const dir = join(here, name);
		try {
			if (!(await stat(dir)).isDirectory()) continue;
			await stat(join(dir, 'Main.qml'));
		} catch {
			continue;
		}
		found.push({ name, dir });
	}
	return found;
}

async function main() {
	const selected = (process.argv[2] || '').trim();
	const list = await cases(selected);
	if (selected && list.length === 0) {
		console.error(`${color.red}Test not found: ${selected}${color.reset}`);
		process.exit(1);
	}
	const runner = process.env.QT_QMLTESTRUNNER || 'qmltestrunner';
	let passed = 0;
	for (const { name, dir } of list) {
		let desktop = { lines: [], error: '' };
		try {
			desktop = await runDesktop(runner, join(dir, 'Main.qml'));
		} catch (error) {
			desktop.error = error.message;
		}
		const web = await runWeb(dir);
		const same = desktop.lines.length === web.lines.length && desktop.lines.every((line, i) => line === web.lines[i]);
		const ok = !desktop.error && !web.error && same && desktop.lines.length > 0;
		if (ok) passed++;
		console.log(`${ok ? `${color.green}[+]` : `${color.red}[-]`}${color.reset} ${name}`);
		if (!ok) {
			for (let i = 0; i < Math.max(desktop.lines.length, web.lines.length); i++) {
				if (desktop.lines[i] !== web.lines[i]) console.log(`    qt : ${desktop.lines[i]}\n    web: ${web.lines[i]}`);
			}
			if (desktop.error) console.log(`    qt error: ${desktop.error}`);
			if (web.error) console.log(`    web error: ${web.error}`);
		}
	}
	console.log(`${color.yellow}[i] ${passed}/${list.length}${color.reset}`);
	process.exit(passed === list.length && list.length > 0 ? 0 : 1);
}

main();
