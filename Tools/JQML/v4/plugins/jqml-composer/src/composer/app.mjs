// Writes the application of a schema to a folder and compiles it with the JQML 4 compiler:
//
//   <out>/src/<Type>.qml          the QML of every schema that the entry needs
//   <out>/constructor.config.json the compiler config (the includes of the modules, see `includes`)
//   <out>/app/<name>.js, .html    the result of compiler/compiler.js (mode js and mode html)

import { spawnSync } from 'node:child_process';
import fs from 'node:fs';
import path from 'node:path';
import { generate } from './generate.mjs';

export function sanitizeName(name) {
	const clean = String(name || '').trim().replace(/[^A-Za-z0-9_-]/g, '');
	return clean || 'app';
}

// Writes the QML files and the config. Returns { entry, configPath, srcDir }.
export function writeSources(entry, resolve, outDir, includes = []) {
	const result = generate(entry, resolve);
	const srcDir = path.join(outDir, 'src');
	fs.rmSync(srcDir, { recursive: true, force: true });
	fs.mkdirSync(srcDir, { recursive: true });
	for (const [name, text] of result.files) fs.writeFileSync(path.join(srcDir, name), text);
	const configPath = path.join(outDir, 'constructor.config.json');
	fs.writeFileSync(configPath, JSON.stringify({ includes: includes.map((item) => path.resolve(item)), dirs: [] }, null, 2) + '\n');
	return { entry: path.join(srcDir, result.entryName), configPath, srcDir, files: [...result.files.keys()] };
}

function runCompiler(compiler, args, env) {
	const run = spawnSync(process.execPath, [compiler, ...args], { cwd: path.dirname(path.dirname(compiler)), env: { ...process.env, ...env }, encoding: 'utf8', maxBuffer: 64 * 1024 * 1024 });
	if (run.error) throw run.error;
	if (run.status !== 0) {
		const lines = [run.stdout, run.stderr].filter(Boolean).join('\n').split(/\r?\n/);
		throw new Error(`The JQML compiler failed (${args[args.indexOf('-m') + 1]})\n${lines.slice(-40).join('\n')}`);
	}
}

// options: { entry: { key, schema }, resolve, outDir, name, enginePath, includes, env }
export function generateApplication(options) {
	const written = writeSources(options.entry, options.resolve, options.outDir, options.includes || []);
	const name = sanitizeName(options.name);
	const compiler = path.join(options.enginePath || '', 'compiler', 'compiler.js');
	if (!options.enginePath || !fs.existsSync(compiler)) return { ...written, compiled: false, name };
	const appDir = path.join(options.outDir, 'app');
	fs.mkdirSync(appDir, { recursive: true });
	runCompiler(compiler, ['-c', written.configPath, '-e', written.entry, '-o', appDir, '-n', name, '-r', '/', '-m', 'js'], options.env);
	runCompiler(compiler, ['-o', appDir, '-n', name, '-m', 'html'], options.env);
	return { ...written, compiled: true, name, jsPath: path.join(appDir, `${name}.js`), htmlPath: path.join(appDir, `${name}.html`) };
}
