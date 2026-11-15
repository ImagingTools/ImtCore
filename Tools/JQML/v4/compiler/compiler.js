#!/usr/bin/env node
// QmlWeb compiler: turns the QML sources of an application into an HTML page plus one JavaScript file.
// The command line and the config files are those of the JQML v3 compiler of ImtCore
// (Tools/JQML/v3/compiler), the parser, the engine and the items are the ones of this repository (src/).
//
//   node compiler/compiler.js -c <config.json> -e <Main.qml> -o <dir> -n <name> [-r <root>] -m js
//   node compiler/compiler.js -o <dir> -n <name> [-i <icon>] -m html
//   node compiler/compiler.js -c <config.json> [-e <Main.qml>] --list-qml
//
// `-m js` validates every document (a syntax error stops the compilation), follows the imports of the
// entry file and embeds the sources it reaches, the modules of the config and the fonts into
// <dir>/<name>.js together with the runtime of src/. `-m html` writes the page that loads that file.

import { mkdirSync, writeFileSync } from 'node:fs';
import { readdir, readFile } from 'node:fs/promises';
import path from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';
import { packDocument } from '../src/packed.js';
import { parseQmlDocument } from '../src/parser.js';
import { Project, scanSource } from '../src/project.js';
import { QmlError } from '../src/lexer.js';
import { minifyScript } from './minify.js';
import { bundleRuntime } from './bundle.js';
import { entryDirectory, listQmldirQmlFiles, loadCompilerConfig, readQmldir, toPosix } from './config.js';
import { loadTranslationCatalogs } from './translations.js';

const here = path.dirname(fileURLToPath(import.meta.url));
const SRC_DIR = path.join(here, '..', 'src');
const PREFIX = 'QmlWeb:';

export const USAGE = `Usage: node compiler/compiler.js [options]

  -c, --config <path>   config file path (dirs, includes, translation_dir)
  -o, --output <path>   output dir path
  -e, --entry <path>    entry qml file path (relative to the config directory)
  -i, --icon <path>     icon file path for compile mode html
  -n, --name <string>   application name (default: "")
  -r, --root <string>   root path of the pictures (default: "/")
  -m, --mode <string>   compile mode: "js" | "html"
      --debug           keep the text of the QML documents in the bundle and publish window.qmlWeb
      --list-qml        print qml files declared by config qmldirs, one path per line
  -h, --help            show this help
`;

const OPTIONS = {
	'-c': 'config',
	'--config': 'config',
	'-o': 'output',
	'--output': 'output',
	'-e': 'entry',
	'--entry': 'entry',
	'-i': 'icon',
	'--icon': 'icon',
	'-n': 'name',
	'--name': 'name',
	'-r': 'root',
	'--root': 'root',
	'-m': 'mode',
	'--mode': 'mode',
};

// Command line -> options; throws on an unknown option or a missing value.
export function parseArgs(argv) {
	const options = { name: '', root: '/', listQml: false, help: false };
	for (let i = 0; i < argv.length; i++) {
		let arg = argv[i];
		let value;
		const equals = arg.indexOf('=');
		if (arg.startsWith('--') && equals > 0) {
			value = arg.slice(equals + 1);
			arg = arg.slice(0, equals);
		}
		if (arg === '--list-qml') {
			options.listQml = true;
			continue;
		}
		if (arg === '--debug') {
			options.debug = true;
			continue;
		}
		if (arg === '-h' || arg === '--help') {
			options.help = true;
			continue;
		}
		const key = OPTIONS[arg];
		if (key === undefined) throw new Error(`unknown option '${arg}'`);
		if (value === undefined) {
			if (i + 1 >= argv.length) throw new Error(`option '${arg}' requires a value`);
			value = argv[++i];
		}
		options[key] = value;
	}
	if (options.mode !== undefined && options.mode !== 'js' && options.mode !== 'html') {
		throw new Error(`option '-m, --mode' must be "js" or "html"`);
	}
	return options;
}

// ----- html -----

function escapeHtml(text) {
	return String(text).replace(/[&<>"']/g, (c) => ({ '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;', "'": '&#39;' })[c]);
}

export function htmlPage(options) {
	const name = options.name || '';
	const icon = options.icon ? `\n\t<link rel="icon" href="${escapeHtml(options.icon)}">` : '';
	return `<!DOCTYPE html>
<html lang="en">
<head>
\t<meta charset="UTF-8">
\t<meta name="viewport" content="width=device-width, initial-scale=1.0">
\t<title>${escapeHtml(name)}</title>${icon}
\t<script src="${escapeHtml(name)}.js"></script>
</head>
<body>
</body>
</html>
`;
}

function compileHtml(options, log) {
	if (!options.output) throw new Error('-o, --output is required');
	const started = Date.now();
	mkdirSync(options.output, { recursive: true });
	const file = path.join(options.output, `${options.name}.html`);
	writeFileSync(file, htmlPage(options));
	log(`${PREFIX} ${file} (${Date.now() - started} ms)`);
	return { html: file };
}

// ----- js -----

// Paths inside the compile-time project are absolute POSIX paths (`/C:/x/y` on Windows), the files are
// read from the disk and every successful read is remembered: that is exactly the set of files to embed.
export function toVirtual(filePath) {
	const posix = toPosix(path.resolve(filePath));
	return posix.startsWith('/') ? posix : `/${posix}`;
}

function toDisk(virtualPath) {
	return /^\/[A-Za-z]:\//.test(virtualPath) ? virtualPath.slice(1) : virtualPath;
}

class RecordingFS {
	constructor() {
		this.texts = new Map();
		this.bytes = new Map();
		this.failure = '';
	}

	async readText(virtualPath) {
		try {
			const text = await readFile(toDisk(virtualPath), 'utf8');
			this.texts.set(virtualPath, text);
			return text;
		} catch (error) {
			this.failure = `${toDisk(virtualPath)} -> ${error.code || error.message}`;
			return null;
		}
	}

	async readBytes(virtualPath) {
		try {
			const data = await readFile(toDisk(virtualPath));
			this.bytes.set(virtualPath, data);
			return data.buffer.slice(data.byteOffset, data.byteOffset + data.byteLength);
		} catch (error) {
			this.failure = `${toDisk(virtualPath)} -> ${error.code || error.message}`;
			return null;
		}
	}
}

// The longest common directory of a list of POSIX paths.
export function commonDirectory(paths) {
	if (paths.length === 0) return '/';
	let parts = path.posix.dirname(paths[0]).split('/');
	for (const file of paths.slice(1)) {
		const other = path.posix.dirname(file).split('/');
		let n = 0;
		while (n < parts.length && n < other.length && parts[n] === other[n]) n++;
		parts = parts.slice(0, n);
	}
	const dir = parts.join('/');
	return dir === '' ? '/' : dir;
}

function relativeTo(root, virtualPath) {
	if (root === '/') return virtualPath.replace(/^\/+/, '');
	if (virtualPath === root) return '';
	return virtualPath.startsWith(`${root}/`) ? virtualPath.slice(root.length + 1) : virtualPath.replace(/^\/+/, '');
}

function location(error, file) {
	const where = error && error.line > 0 ? `${file}:${error.line}:${error.column || 1}` : file;
	return `${where}: error: ${error && error.message ? error.message : error}`;
}

async function drain(project) {
	while (project.pending.length > 0) await Promise.all(project.pending.splice(0));
}

// Reads the modules of the config:
// { modules: { name: [virtual dir] }, moduleDirs: Map(virtual dir -> name), entries: [...], qmldirs: Map(virtual path -> text) }.
function collectModules(config, log) {
	const modules = {};
	const moduleDirs = new Map();
	const entries = [];
	const qmldirs = new Map();
	for (const dirPath of config.dirs) {
		const qmldir = readQmldir(dirPath);
		const dir = toVirtual(qmldir.dir);
		if (moduleDirs.has(dir)) continue;
		const name = qmldir.moduleName;
		if (!name) throw new Error(`${qmldir.dir}/qmldir: no 'module <Name>' line`);
		moduleDirs.set(dir, name);
		(modules[name] ||= []).push(dir);
		qmldirs.set(`${dir}/qmldir`, qmldir.text);
		for (const entry of qmldir.entries) entries.push({ ...entry, path: toVirtual(entry.path), module: name });
		log(`${PREFIX} module ${name} (${qmldir.entries.length} files) ${qmldir.dir}`);
	}
	return { modules, moduleDirs, entries, qmldirs };
}

async function compileJs(options, env, log, warn) {
	if (!options.entry) throw new Error('-e, --entry is required with -m js');
	const started = Date.now();

	let config = { dirs: [] };
	let configDirPath = process.cwd();
	let translationDirs = [];
	if (options.config) {
		const loaded = loadCompilerConfig(options.config, env);
		config = loaded.config;
		configDirPath = loaded.configDirPath;
		translationDirs = loaded.translationDirs;
	}

	let translations = {};
	if (translationDirs.length > 0) {
		const embedded = loadTranslationCatalogs(translationDirs, (message) => warn(`${PREFIX} ${message}`));
		translations = embedded.catalogs;
		log(`${PREFIX} embedded ${embedded.fileCount} translation file(s)`);
	}

	const entryFile = path.resolve(configDirPath, options.entry);
	const entryDir = toVirtual(entryDirectory(configDirPath, options.entry));
	const main = toVirtual(entryFile);
	const { modules, moduleDirs, entries, qmldirs } = collectModules(config, log);
	if (moduleDirs.has(entryDir)) log(`${PREFIX} entry point is ${moduleDirs.get(entryDir)} module`);

	// The entry document and everything it reaches.
	const fs = new RecordingFS();
	const project = new Project(fs, { modules });
	try {
		await project.load(main);
	} catch (error) {
		throw new Error(location(error, toDisk(main)));
	}

	// Every file of every module of the config, like the JQML compiler, and the .qml files next to the entry.
	for (const entry of entries) {
		if (entry.path.endsWith('.js')) await project.addScript(entry.path);
		else await project.addSingleton(entry.path);
		const loaded = entry.path.endsWith('.js') ? project.scripts.has(entry.path) : project.docs.has(entry.path);
		if (!loaded) warn(`${PREFIX} ${toDisk(entry.path)}: listed in the qmldir of ${entry.module} but cannot be read`);
	}
	if (!moduleDirs.has(entryDir)) {
		const names = await readdir(toDisk(entryDir));
		for (const name of names.sort()) {
			if (name.endsWith('.qml')) await project.addSingleton(`${entryDir}/${name}`);
		}
	}
	await drain(project);

	const errors = [];
	for (const doc of project.docs.values()) {
		if (doc.error) errors.push(location(doc.error, toDisk(doc.path)));
	}
	if (errors.length > 0) throw new Error(errors.join('\n'));

	// The qmldir of a module nobody imports is still needed by Qt.createObject("Type") at run time.
	for (const [file, text] of qmldirs) if (!fs.texts.has(file)) fs.texts.set(file, text);
	const root = commonDirectory([...fs.texts.keys(), ...fs.bytes.keys(), main]);

	const spec = {
		name: options.name,
		root: options.root,
		main: relativeTo(root, main),
		lenient: true,
		// every document was parsed above with the script checks on
		trusted: true,
		files: {},
		binaries: {},
		modules: {},
		translations,
	};
	if (options.debug) spec.debug = true;
	else {
		spec.docs = {};
		spec.scans = {};
	}
	for (const [file, text] of [...fs.texts].sort((a, b) => a[0].localeCompare(b[0]))) {
		if (!file.endsWith('.qml') || options.debug) {
			spec.files[relativeTo(root, file)] = text;
			continue;
		}
		// A document is carried as its packed tree, not as its text.
		const name = relativeTo(root, file);
		spec.docs[name] = packedRecord(project.docs.get(file), text);
		const scan = packedScan(scanSource(text));
		if (scan !== null) spec.scans[name] = scan;
	}
	for (const [file, data] of [...fs.bytes].sort((a, b) => a[0].localeCompare(b[0]))) spec.binaries[relativeTo(root, file)] = data.toString('base64');
	for (const [name, dirs] of Object.entries(modules)) spec.modules[name] = dirs.map((dir) => relativeTo(root, dir));

	const fileName = `${options.name}.js`;
	const banner = `// ${options.name || 'QmlWeb application'} — generated by QmlWeb compiler/compiler.js from ${toDisk(main)}`;
	const tail = `__modules["runtime.js"].bootstrap(${JSON.stringify(spec)});`;
	const bundle = bundleRuntime({ srcDir: SRC_DIR, entry: 'runtime.js', banner, tail, file: fileName, sourceRoot: toPosix(path.relative(options.output || '.', SRC_DIR)) });

	let written = null;
	if (options.output) {
		mkdirSync(options.output, { recursive: true });
		written = path.join(options.output, fileName);
		writeFileSync(written, bundle.code);
		writeFileSync(`${written}.map`, JSON.stringify(bundle.map));
	}

	const docs = [...project.docs.values()].length;
	log(`${PREFIX} ${docs} documents, ${project.scripts.size} scripts, ${fs.bytes.size} fonts, ${Object.keys(modules).length} modules; root ${toDisk(root)}`);
	if (written) log(`${PREFIX} ${written} (${(bundle.code.length / 1024).toFixed(0)} KB, ${Date.now() - started} ms)`);
	return { js: written, spec, code: bundle.code, map: bundle.map, project };
}

// options: see parseArgs. Returns what was produced; throws on errors.
// The record of a document of a compiled application (see CompiledDocument): its tree packed with smaller scripts, as JSON text. A
// document that does not parse is carried as its error: the application only fails when it uses that document.
function packedRecord(doc, text) {
	let def = doc !== undefined && !doc.error ? doc.def : null;
	if (def === null) {
		try {
			def = parseQmlDocument(text);
		} catch (error) {
			if (!(error instanceof QmlError)) throw error;
			return JSON.stringify({ error: [error.message, error.line || 0, error.column || 0] });
		}
	}
	return JSON.stringify(packDocument(def, minifyScript));
}

// The scan of a document in the short form of the spec, or null when it has nothing in it.
function packedScan(scan) {
	const packed = {};
	if (scan.fonts.length > 0) packed.f = scan.fonts;
	if (scan.types.length > 0) packed.t = scan.types;
	if (scan.files.length > 0) packed.q = scan.files;
	if (scan.enums) packed.e = 1;
	return Object.keys(packed).length > 0 ? packed : null;
}

export async function compile(options, { env = process.env, log = console.log, warn = console.warn } = {}) {
	if (options.mode === 'html') return compileHtml(options, log);
	if (options.mode === 'js') return compileJs(options, env, log, warn);
	throw new Error('-m, --mode must be "js" or "html"');
}

export { listQmldirQmlFiles };

async function main(argv) {
	let options;
	try {
		options = parseArgs(argv);
	} catch (error) {
		console.error(`${PREFIX} ${error.message}`);
		console.error(USAGE);
		return 1;
	}
	if (options.help) {
		console.log(USAGE);
		return 0;
	}
	try {
		if (options.listQml) {
			if (!options.config) throw new Error('--config is required with --list-qml');
			const files = listQmldirQmlFiles(options.config, process.env, options.entry || '');
			process.stdout.write(files.join('\n') + (files.length ? '\n' : ''));
			return 0;
		}
		if (options.mode === 'js' || options.mode === 'html') {
			await compile(options);
			return 0;
		}
		console.log(USAGE);
		return 0;
	} catch (error) {
		console.error(`${PREFIX} ${error && error.message ? error.message : error}`);
		return 1;
	}
}

const invokedDirectly = process.argv[1] && pathToFileURL(path.resolve(process.argv[1])).href === import.meta.url;
if (invokedDirectly) {
	// process.exit() would cut the output still buffered for a pipe (`--list-qml | wc -l`).
	main(process.argv.slice(2)).then((code) => {
		process.exitCode = code;
	});
}
