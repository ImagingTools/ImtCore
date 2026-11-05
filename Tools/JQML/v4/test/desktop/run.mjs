// Compares this engine with desktop Qt the way JQML's tests/index.js does:
// each case is a folder with Main.qml that prints through console.log and calls Qt.quit().
// The same file is run here (no browser) and with `qml`. The `qml:` lines must match.
// Kept apart from test/run.mjs, which only checks that the samples in qml/ load.
//
//   node test/desktop/run.mjs [Name]
//   QT_QML_RUNNER=C:\Qt\...\bin\qml.exe node test/desktop/run.mjs
//
// test.json in a case folder may list "dirs": extra QML import paths, relative to the case.

import { spawn } from 'node:child_process';
import { existsSync } from 'node:fs';
import { appendFile, readFile, readdir, stat } from 'node:fs/promises';
import { dirname, join, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';
import { Engine } from '../../src/engine.js';
import { Project } from '../../src/project.js';

const here = dirname(fileURLToPath(import.meta.url));
const roots = [here, join(here, '..', 'jqml')];
const TIMEOUT_MS = 5000;
const MAX_FRAMES = 180;

const color = {
	reset: '\x1b[0m',
	red: '\x1b[31m',
	green: '\x1b[32m',
	yellow: '\x1b[33m',
	cyan: '\x1b[36m',
	gray: '\x1b[90m',
};

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

// Qt on Windows writes the ANSI code page. ASCII and real UTF-8 stay as they are; anything else is cp1251.
function decodeQt(buffer) {
	try {
		return new TextDecoder('utf-8', { fatal: true }).decode(buffer);
	} catch {
		return new TextDecoder('windows-1251').decode(buffer);
	}
}

function qmlLines(raw) {
	const lines = [];
	for (const line of String(raw).split(/\r?\n/)) {
		const match = /^qml:\s+(?!warning:)(.*)$/.exec(line);
		if (match) lines.push(match[1]);
	}
	return lines;
}

function sameLines(a, b) {
	if (a.length !== b.length) return false;
	for (let i = 0; i < a.length; i++) if (a[i] !== b[i]) return false;
	return true;
}

// JQML test.json files use ${IMTCOREDIR}. When the variable is unset, the tree next to JQML is the same fallback the original runner derives from its own path.
function fillEnv(source) {
	const extra = {};
	if (!process.env.IMTCOREDIR && existsSync('d:/Work/IT/ImtCore')) extra.IMTCOREDIR = 'd:/Work/IT/ImtCore';
	return String(source).replace(/\$\{([A-Za-z0-9_]+)\}/g, (_, key) => process.env[key] ?? extra[key] ?? '');
}

// `dirs` of test.json, plus the configs it includes. Missing folders are skipped.
async function readImportDirs(testDir) {
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

function runDesktop(qml, mainFile, importDirs) {
	const args = [];
	for (const dir of importDirs) args.push('-I', dir);
	args.push('-f', mainFile);
	const env = { ...process.env, QT_FORCE_STDERR_LOGGING: '1' };
	if (!env.QT_QPA_PLATFORM) env.QT_QPA_PLATFORM = 'offscreen';

	return new Promise((resolvePromise, reject) => {
		const child = spawn(qml, args, { cwd: dirname(mainFile), env, windowsHide: true });
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
		child.on('close', (code) => {
			clearTimeout(timer);
			const raw = decodeQt(Buffer.concat(chunks));
			const lines = qmlLines(raw);
			let error = '';
			if (code !== 0 && lines.length === 0) {
				error = raw.split(/\r?\n/).map((line) => line.trim()).filter(Boolean).slice(0, 8).join(' | ');
			}
			resolvePromise({ lines, error });
		});
	});
}

// One layout and binding pass. The root is not parented and is not stretched:
// desktop `qml` reports the root's parent as null and keeps the root's own size.
function settle(engine, root) {
	for (let pass = 0; pass < 10; pass++) {
		root.$updateRequested = false;
		root.$layout();
		engine.evaluateBindings();
		if (!root.$updateRequested) break;
	}
}

function paintCanvases(item) {
	if (typeof item.$ensurePainted === 'function') item.$ensurePainted();
	const children = item.$children || [];
	for (let i = 0; i < children.length; i++) paintCanvases(children[i]);
}

function timersRunning(engine) {
	for (const timer of engine.timers) {
		if (!timer.$destroyed && timer.$running) return true;
	}
	return false;
}

async function runWeb(testDir, importDirs) {
	const engine = new Engine();
	const logs = [];
	let quit = false;
	engine.onLog = (line) => logs.push(line);
	engine.onQuit = () => {
		quit = true;
	};
	// Repeater, ListModel, Loader and Component are created in this mode.
	engine.lenient = true;

	try {
	const project = new Project(new DirFS(testDir), { importPaths: importDirs.map((dir) => resolve(testDir, dir)) });
	await project.load('Main.qml');

	engine.reset();
	engine.lenient = true;
	engine.project = project;
	engine.host = null;
	engine.setupSingletons();
	const doc = project.docs.get(project.mainPath);
	engine.documentScope = engine.newScope(null, doc.path);
	const root = engine.createItem(doc.def, engine.documentScope, null, [], undefined, 0);
	settle(engine, root);
	engine.runCompletedFrom(0);
	// qml.exe with `import QtTest` returns as soon as the first completion batch ends, so a timer
	// started there never runs. The log is whatever onCompleted already printed.
	if (!quit && (doc.def.modules || []).some((mod) => mod.name === 'QtTest')) quit = true;
	// A script in onCompleted may have written a property. Bindings and change handlers
	// catch up on the next pass; a log that already ran stays as it was.
	if (!quit) settle(engine, root);
	// Desktop `qml` paints after requestPaint(); the comparison runner has no frame loop, so
	// Canvas.onPaint still has to run for cases that quit from there.
	if (!quit) paintCanvases(root);

	let seconds = 0;
	const started = Date.now();
	for (let frame = 0; frame < MAX_FRAMES && !quit && Date.now() - started < TIMEOUT_MS; frame++) {
		if (!timersRunning(engine) && !root.$updateRequested) break;
		// Like a real frame, lets pending file reads (Qt.createComponent of a file that is not loaded yet) finish.
		await new Promise((resolvePromise) => setImmediate(resolvePromise));
		seconds += 0.016;
		root.$updateRequested = false;
		root.$layout();
		engine.evaluateBindings();
		engine.setTime(seconds);
		if (!quit) settle(engine, root);
	}
	return { logs, errors: engine.errors.slice(), threw: '' };
	} catch (error) {
		return { logs, errors: engine.errors.concat(error.message), threw: error.message };
	}
}

async function cases(selected) {
	const found = [];
	for (const root of roots) {
		let names = [];
		try {
			names = (await readdir(root)).sort();
		} catch {
			continue;
		}
		for (const name of names) {
			if (selected && name !== selected) continue;
			const dir = join(root, name);
			try {
				if (!(await stat(dir)).isDirectory()) continue;
				await stat(join(dir, 'Main.qml'));
			} catch {
				continue;
			}
			found.push({ name, dir });
		}
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
	const qml = process.env.QT_QML_RUNNER || 'qml';
	const reportPath = join(here, 'last-run.jsonl');
	await appendFile(reportPath, '');
	const { writeFile } = await import('node:fs/promises');
	await writeFile(reportPath, '');
	let passed = 0;

	for (const { name, dir } of list) {
		const mainFile = join(dir, 'Main.qml');
		let desktop = [];
		let web = [];
		let desktopError = '';
		let webError = '';
		let webErrors = [];
		const importDirs = await readImportDirs(dir);
		try {
			const result = await runDesktop(qml, mainFile, importDirs);
			desktop = result.lines;
			desktopError = result.error;
		} catch (error) {
			desktopError = error.message;
		}
		try {
			const result = await runWeb(dir, importDirs);
			web = result.logs;
			webErrors = result.errors;
			webError = result.threw;
		} catch (error) {
			webError = error.message;
		}
		const ok = !desktopError && !webError && sameLines(desktop, web);
		if (ok) passed++;
		const mark = ok ? `${color.green}[+]${color.reset}` : `${color.red}[-]${color.reset}`;
		console.log(`${mark} ${name}`);
		await appendFile(reportPath, `${JSON.stringify({ name, ok, desktop, web, desktopError, webError, webErrors })}\n`);
	}

	console.log(`${color.yellow}[i] ${passed}/${list.length}${color.reset}`);
	process.exit(passed === list.length && list.length > 0 ? 0 : 1);
}

main();
