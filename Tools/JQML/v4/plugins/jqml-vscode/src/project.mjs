// Where types come from: the engine index (built-in types), qmldir modules from the compiler config, import paths
// and indexed qmldir files, imported directories and the directory of the file. The rules follow Project in
// v4/src/project.js: the later import wins, every import wins over the directory of the file, the newest version
// that an import allows is taken.

import fs from 'node:fs';
import path from 'node:path';
import { envFill, resolveConfigRef, toPosix } from '../lib/config.js';
import { scanText } from './scanner.mjs';

const QMLDIR_RE = /^(singleton\s+)?([A-Z]\w*)\s+(\d+)(?:\.(\d+))?\s+(\S+\.(?:qml|js))\s*$/;
const MODULE_RE = /^module\s+(\S+)\s*$/;

function readText(file) {
	try {
		return fs.readFileSync(file, 'utf8');
	} catch {
		return null;
	}
}

function pickVersion(entries, version) {
	if (entries === undefined || entries.length === 0) return undefined;
	let pool = entries;
	if (version !== null && version !== undefined) {
		const fitting = entries.filter((entry) => entry.major === version.major && entry.minor <= version.minor);
		if (fitting.length > 0) pool = fitting;
	}
	let best = pool[0];
	for (const entry of pool) if (entry.major > best.major || (entry.major === best.major && entry.minor > best.minor)) best = entry;
	return best;
}

export function parseQmldirText(dir, source) {
	const types = new Map(); // name -> [{ major, minor, path, singleton }]
	const scripts = new Map(); // name -> path
	let moduleName = '';
	for (const raw of source.split(/\r?\n/)) {
		const line = raw.trim();
		const moduleMatch = MODULE_RE.exec(line);
		if (moduleMatch !== null) {
			moduleName = moduleMatch[1];
			continue;
		}
		const match = QMLDIR_RE.exec(line);
		if (match === null) continue;
		const file = toPosix(path.resolve(dir, match[5]));
		if (file.endsWith('.js')) {
			scripts.set(match[2], file);
			continue;
		}
		if (!types.has(match[2])) types.set(match[2], []);
		types.get(match[2]).push({ major: Number(match[3]), minor: Number(match[4] || 0), path: file, singleton: Boolean(match[1]) });
	}
	return { dir: toPosix(dir), moduleName, types, scripts };
}

export class ProjectIndex {
	constructor() {
		this.moduleDirs = new Map(); // module name -> [directories], the later wins
		this.importPaths = [];
		this.modules = new Map(); // module name -> { types, scripts } | null
		this.directories = new Map(); // directory -> parsed qmldir | null
		this.issues = []; // { severity, message } from the config
		this.configured = false;
	}

	clear() {
		this.moduleDirs.clear();
		this.modules.clear();
		this.directories.clear();
		this.issues = [];
		this.configured = false;
	}

	invalidate() {
		this.modules.clear();
		this.directories.clear();
	}

	// A module directory (with a qmldir); the module name is read from its `module` line.
	addModuleDir(dir) {
		const parsed = this.readDirectory(dir);
		if (parsed === null || parsed.moduleName === '') return false;
		const list = this.moduleDirs.get(parsed.moduleName) || [];
		if (!list.includes(parsed.dir)) list.push(parsed.dir);
		this.moduleDirs.set(parsed.moduleName, list);
		this.modules.delete(parsed.moduleName);
		return true;
	}

	// ${VAR} placeholders that are missing in the environment are guessed from the config path: IMTCOREDIR is the
	// closest parent folder that holds Qml/imtcontrols.
	loadConfig(configPath, env) {
		this.issues = [];
		const fill = { ...env };
		if (!fill.IMTCOREDIR) {
			for (let dir = path.dirname(path.resolve(configPath)); dir !== path.dirname(dir); dir = path.dirname(dir)) {
				if (fs.existsSync(path.join(dir, 'Qml', 'imtcontrols', 'qmldir'))) {
					fill.IMTCOREDIR = toPosix(dir);
					break;
				}
			}
		}
		const dirs = [];
		const visit = (file, seen) => {
			const key = path.resolve(file);
			if (seen.has(key)) return;
			seen.add(key);
			const text = readText(file);
			if (text === null) {
				this.issues.push({ severity: 'warning', message: `Config '${file}' was not found` });
				return;
			}
			let json;
			try {
				json = JSON.parse(envFill(text, fill));
			} catch (error) {
				this.issues.push({ severity: 'error', message: `Config '${file}' is not valid JSON: ${error.message}` });
				return;
			}
			const base = path.dirname(path.resolve(file));
			const own = (json.dirs || []).map((item) => resolveConfigRef(base, item, fill));
			const included = [];
			for (const include of json.includes || []) {
				const target = resolveConfigRef(base, include, fill);
				const before = dirs.length;
				visit(target, seen);
				included.push(...dirs.splice(before));
			}
			// the directories of the includes go first, like in the compiler
			dirs.push(...included, ...own);
		};
		visit(configPath, new Set());
		this.configured = true;
		for (const dir of dirs) {
			if (dir.indexOf('${') >= 0) {
				this.issues.push({ severity: 'warning', message: `Unresolved variable in '${dir}'. Set it in jqml4.env.` });
			} else if (!this.addModuleDir(dir)) {
				this.issues.push({ severity: 'warning', message: `No qmldir with a module name in '${dir}'` });
			}
		}
	}

	readDirectory(dir) {
		const key = toPosix(path.resolve(dir));
		if (this.directories.has(key)) return this.directories.get(key);
		const source = readText(path.join(key, 'qmldir'));
		const parsed = source === null ? null : parseQmldirText(key, source);
		this.directories.set(key, parsed);
		return parsed;
	}

	// { types, scripts, dirs } of a module: the types of its directories merged, the last directory wins.
	module(name) {
		if (this.modules.has(name)) return this.modules.get(name);
		let result = null;
		let dirs = this.moduleDirs.get(name);
		if (dirs === undefined) {
			for (const base of this.importPaths) {
				const dir = path.join(base, ...name.split('.'));
				if (this.readDirectory(dir) !== null) {
					dirs = [dir];
					break;
				}
			}
		}
		if (dirs !== undefined) {
			const types = new Map();
			const scripts = new Map();
			for (const dir of [...dirs].reverse()) {
				const parsed = this.readDirectory(dir);
				if (parsed === null) continue;
				for (const [typeName, entries] of parsed.types) if (!types.has(typeName)) types.set(typeName, entries);
				for (const [scriptName, file] of parsed.scripts) if (!scripts.has(scriptName)) scripts.set(scriptName, file);
			}
			result = { types, scripts, dirs };
		}
		this.modules.set(name, result);
		return result;
	}

	hasModuleSources() {
		return this.configured || this.moduleDirs.size > 0 || this.importPaths.length > 0;
	}

	allModuleNames() {
		return [...this.moduleDirs.keys()];
	}

	// The qmldir file(s) of a module, for Go to Definition on `import Name`.
	moduleFiles(name) {
		const info = this.module(name);
		return info === null ? [] : info.dirs.map((dir) => toPosix(path.join(dir, 'qmldir'))).filter((file) => fs.existsSync(file));
	}

	// File paths where the type `typeName` used in a file with `scan` can be, the best first, and whether any import
	// of the file could not be resolved.
	candidates(typeName, filePath, scan) {
		const dir = path.dirname(filePath);
		const out = [];
		const dot = typeName.indexOf('.');
		const alias = dot >= 0 ? typeName.slice(0, dot) : '';
		const name = dot >= 0 ? typeName.slice(dot + 1) : typeName;
		const imports = scan === null ? [] : scan.imports;
		for (let i = imports.length - 1; i >= 0; i--) {
			const imp = imports[i];
			if (imp.alias !== alias || imp.kind === 'js') continue;
			if (imp.kind === 'dir') {
				out.push(toPosix(path.resolve(dir, imp.path, `${name}.qml`)));
			} else {
				const info = this.module(imp.name);
				if (info === null) continue;
				const entry = pickVersion((info.types.get(name) || []).filter((item) => !item.singleton), imp.version);
				if (entry !== undefined) out.push(entry.path);
			}
		}
		if (dot < 0) out.push(toPosix(path.join(dir, `${typeName}.qml`)));
		return out;
	}

	resolveFile(typeName, filePath, scan) {
		for (const candidate of this.candidates(typeName, filePath, scan)) {
			if (fs.existsSync(candidate)) return candidate;
		}
		return null;
	}

	// The path of a singleton visible from the file: imported modules and directories first, then every known module.
	findSingleton(name, filePath, scan) {
		const dir = path.dirname(filePath);
		const imports = scan === null ? [] : scan.imports;
		const lookIn = [];
		for (const imp of imports) {
			if (imp.kind === 'module') lookIn.push(this.module(imp.name));
			else if (imp.kind === 'dir') lookIn.push(this.readDirectory(path.resolve(dir, imp.path)));
		}
		lookIn.push(this.readDirectory(dir));
		for (const name2 of this.moduleDirs.keys()) lookIn.push(this.module(name2));
		for (const source of lookIn) {
			if (source === null) continue;
			const entry = pickVersion((source.types.get(name) || []).filter((item) => item.singleton), null);
			if (entry !== undefined) return entry.path;
			const script = source.scripts === undefined ? undefined : source.scripts.get(name);
			if (script !== undefined) return script;
		}
		return null;
	}

	// Names of the types that can be written in a file, for completion: { name, path, singleton }.
	visibleTypes(filePath, scan) {
		const dir = path.dirname(filePath);
		const out = new Map();
		const add = (name, file, singleton, prefix) => {
			const key = prefix ? `${prefix}.${name}` : name;
			if (!out.has(key)) out.set(key, { name: key, path: file, singleton });
		};
		const imports = scan === null ? [] : scan.imports;
		for (const imp of imports) {
			if (imp.kind === 'module') {
				const info = this.module(imp.name);
				if (info === null) continue;
				for (const [name, entries] of info.types) {
					const entry = pickVersion(entries, imp.version);
					if (entry !== undefined) add(name, entry.path, entry.singleton, imp.alias);
				}
			} else if (imp.kind === 'dir') {
				const target = path.resolve(dir, imp.path);
				try {
					for (const file of fs.readdirSync(target)) if (/^[A-Z]\w*\.qml$/.test(file)) add(file.slice(0, -4), toPosix(path.join(target, file)), false, imp.alias);
				} catch {
					// the folder does not exist
				}
			}
		}
		try {
			for (const file of fs.readdirSync(dir)) if (/^[A-Z]\w*\.qml$/.test(file)) add(file.slice(0, -4), toPosix(path.join(dir, file)), false, '');
		} catch {
			// no directory
		}
		return [...out.values()];
	}
}

// Used by tests and by the service to index a text without a file.
export { scanText };
