// The files of a QML project. Loading is asynchronous (the browser has to fetch the files); the engine
// afterwards looks everything up synchronously: components, JavaScript imports and font files are
// discovered up front by following imports, the types a file uses, createObject("Type") literals and
// font file names written in the source.

import { fonts } from './fonts.js';
import { QmlError } from './lexer.js';
import { parseQmlDocument } from './parser.js';
import { BUILTIN_TYPES } from './items.js';
import { basename, dirname, joinPath, normalizePath, resolvePath, SCRIPT_IMPORT_RE } from './util.js';

// Files served over HTTP (relative to the page).
export class HttpFS {
	// The reason of the last failed read (URL and HTTP status), shown in error messages.
	failure = '';

	async readBytes(path) {
		try {
			const response = await fetch(path, { cache: 'no-cache' });
			if (!response.ok) this.failure = `${response.url} -> HTTP ${response.status}`;
			return response.ok ? await response.arrayBuffer() : null;
		} catch (error) {
			this.failure = `${path} -> ${error && error.message}`;
			return null;
		}
	}

	async readText(path) {
		try {
			const response = await fetch(path, { cache: 'no-cache' });
			if (!response.ok) this.failure = `${response.url} -> HTTP ${response.status}`;
			return response.ok ? await response.text() : null;
		} catch (error) {
			this.failure = `${path} -> ${error && error.message}`;
			return null;
		}
	}
}

// Files held in memory: a Map from path to a File/Blob, string or ArrayBuffer.
export class MapFS {
	constructor(files) {
		this.files = new Map();
		for (const [path, content] of files) this.files.set(normalizePath(path), content);
	}

	async readBytes(path) {
		const content = this.files.get(normalizePath(path));
		if (content === undefined) return null;
		if (typeof content === 'string') return new TextEncoder().encode(content).buffer;
		if (content instanceof ArrayBuffer) return content;
		return content.arrayBuffer();
	}

	async readText(path) {
		const content = this.files.get(normalizePath(path));
		if (content === undefined) return null;
		if (typeof content === 'string') return content;
		if (content instanceof ArrayBuffer) return new TextDecoder().decode(content);
		return content.text();
	}
}

// A document of an embedded file system: the source is parsed when the engine first asks for the tree, so
// the many documents of a compiled application that a run never touches cost no memory beyond their text.
class LazyDocument {
	constructor(path, source) {
		this.path = path;
		this.source = source;
		this.parsed = false;
		this.tree = null;
		this.failure = null;
	}

	parse() {
		if (this.parsed) return;
		this.parsed = true;
		try {
			this.tree = parseQmlDocument(this.source);
		} catch (error) {
			if (!(error instanceof QmlError)) throw error;
			this.failure = error;
		}
	}

	get def() {
		this.parse();
		return this.tree;
	}

	get error() {
		this.parse();
		return this.failure;
	}
}

const DYNAMIC_TYPE_RE = /createObject\s*\(\s*(["'])([A-Za-z_][\w.]*)\1/g;
const FONT_FILE_RE = /(["'])([^"'\n]+\.(?:ttf|otf|woff2?))\1/gi;
const QML_FILE_RE = /(["'])([\w./-]+\.qml)\1/g;

// `Type 1.0 path/File.qml` lines of a qmldir file; singletons, internal types and other directives are ignored.
const QMLDIR_TYPE_RE = /^(singleton\s+)?([A-Z]\w*)\s+\d+(?:\.\d+)?\s+(\S+\.(?:qml|js))\s*$/;

export class Project {
	// options.importPaths: folders searched for `<Module>/qmldir` to resolve `import Module 1.0`.
	// options.modules: module name -> directory (or array of directories) whose qmldir files declare the module;
	// checked before importPaths. Several directories may declare the same module (the compiler config lists
	// `imtcontrols` and `imtcontrols/Popup/Internal` separately), their types are merged, the first one wins.
	constructor(fs, options = {}) {
		this.fs = fs;
		this.importPaths = options.importPaths || [];
		this.moduleDirs = new Map();
		for (const [name, dirs] of Object.entries(options.modules || {})) {
			this.moduleDirs.set(name, Array.isArray(dirs) ? dirs : [dirs]);
		}
		this.modules = new Map(); // module name -> Map(type name -> path), null if not found
		this.moduleLoads = new Map(); // module name -> Promise
		this.pending = []; // promises of singleton documents still being discovered
		this.singletons = new Map(); // singleton name -> path (of all loaded modules)
		this.jsModules = new Map(); // global name -> path of a JavaScript file listed in a qmldir
		this.mainPath = '';
		this.docs = new Map(); // path -> { path, source, def, error }
		this.scripts = new Map(); // path -> source
		this.fontFamilies = new Map(); // path -> family name ('' if it could not be loaded)
		this.missing = new Set();
		this.directoryLoads = new Set();
	}

	// Loads the main document and everything it refers to. Throws a QmlError if the main file is
	// missing or has a syntax error.
	async load(mainPath) {
		this.mainPath = normalizePath(mainPath);
		const doc = await this.addDoc(this.mainPath, true);
		if (doc.error) throw doc.error;
		await this.discover(doc);
		while (this.pending.length > 0) await Promise.all(this.pending.splice(0));
		return doc;
	}

	// Every .qml file of an embedded file system (a compiled application). A page or a component that the server or a
	// string built at run time names (`qrc:/qml/Dir/Page.qml`) cannot be seen by following imports, yet the engine
	// looks documents up synchronously. The documents are registered unparsed (LazyDocument); what following their
	// imports would have discovered is read here from the file system instead: every module and directory qmldir,
	// every JavaScript file and the fonts named in the sources.
	async loadEmbedded() {
		const files = this.fs.files;
		if (!(files instanceof Map)) return;
		const paths = [...files.keys()];
		const lazy = [];
		for (const path of paths) {
			if (!path.endsWith('.qml') || this.docs.has(path)) continue;
			const content = files.get(path);
			if (typeof content === 'string') {
				const doc = new LazyDocument(path, content);
				this.docs.set(path, doc);
				lazy.push(doc);
			} else {
				await this.addSingleton(path);
			}
		}

		const moduleDirs = new Set([...this.moduleDirs.values()].flat());
		for (const name of this.moduleDirs.keys()) await this.addModule(name);
		for (const path of paths) {
			if (basename(path) === 'qmldir' && !moduleDirs.has(dirname(path))) await this.addDirectorySingletons(dirname(path));
			else if (path.endsWith('.js')) await this.addScript(path);
		}
		for (const doc of lazy) {
			for (const match of doc.source.matchAll(FONT_FILE_RE)) await this.addFont(resolvePath(match[2], dirname(doc.path)));
		}
		while (this.pending.length > 0) await Promise.all(this.pending.splice(0));
	}

	async addDoc(path, required) {
		const existing = this.docs.get(path);
		if (existing) return existing;
		const source = await this.fs.readText(path);
		if (source === null) {
			if (required) throw new QmlError(`Cannot read '${path}'${this.fs.failure ? ` (${this.fs.failure})` : ''}`);
			return null;
		}
		const doc = { path, source, def: null, error: null };
		this.docs.set(path, doc);
		try {
			doc.def = parseQmlDocument(source);
		} catch (error) {
			if (!(error instanceof QmlError)) throw error;
			doc.error = error;
		}
		return doc;
	}

	addModule(name) {
		let loading = this.moduleLoads.get(name);
		if (loading === undefined) {
			loading = this.readModule(name);
			this.moduleLoads.set(name, loading);
		}
		return loading;
	}

	async readModule(name) {
		this.modules.set(name, null);
		const explicit = this.moduleDirs.get(name);
		if (explicit !== undefined) {
			const types = new Map();
			for (const dir of explicit) {
				const source = await this.fs.readText(joinPath(dir, 'qmldir'));
				if (source !== null) this.readQmldir(dir, source, types);
			}
			this.modules.set(name, types);
			return;
		}
		for (const base of this.importPaths) {
			const dir = joinPath(base, name.replace(/\./g, '/'));
			const source = await this.fs.readText(joinPath(dir, 'qmldir'));
			if (source === null) continue;
			const types = new Map();
			this.readQmldir(dir, source, types);
			this.modules.set(name, types);
			return;
		}
	}

	// Registers the types, singletons and JavaScript files of one qmldir in `types`.
	readQmldir(dir, source, types) {
		const singletons = [];
		for (const line of source.split(/\r?\n/)) {
			const match = QMLDIR_TYPE_RE.exec(line.trim());
			if (match === null) continue;
			const path = joinPath(dir, match[3]);
			if (path.endsWith('.js')) {
				if (!this.jsModules.has(match[2])) {
					this.jsModules.set(match[2], path);
					this.pending.push(this.addScript(path));
				}
				continue;
			}
			if (match[1]) {
				if (!this.singletons.has(match[2])) {
					this.singletons.set(match[2], path);
					singletons.push(path);
				}
			} else if (!types.has(match[2])) {
				types.set(match[2], path);
			}
		}
		// Singleton documents are discovered later by load() (they may import this very module).
		for (const path of singletons) this.pending.push(this.addSingleton(path));
	}

	async addSingleton(path) {
		if (this.docs.has(path)) return;
		const doc = await this.addDoc(path, false);
		if (doc !== null) await this.discover(doc);
	}

	// `import "Internal"`: the `singleton` and JavaScript lines of the qmldir of an imported directory (an aliased
	// import only contributes its JavaScript files).
	async addDirectorySingletons(dir, jsOnly = false) {
		const key = jsOnly ? `${dir}#js` : dir;
		if (this.directoryLoads.has(key)) return;
		this.directoryLoads.add(key);
		const source = await this.fs.readText(joinPath(dir, 'qmldir'));
		if (source === null) return;
		for (const line of source.split(/\r?\n/)) {
			const match = QMLDIR_TYPE_RE.exec(line.trim());
			if (match === null) continue;
			const path = joinPath(dir, match[3]);
			if (match[3].endsWith('.js')) {
				if (!this.jsModules.has(match[2])) {
					this.jsModules.set(match[2], path);
					this.pending.push(this.addScript(path));
				}
				continue;
			}
			if (jsOnly || !match[1] || this.singletons.has(match[2])) continue;
			this.singletons.set(match[2], path);
			this.pending.push(this.addSingleton(path));
		}
	}

	async discover(doc) {
		const dir = dirname(doc.path);
		const tasks = [];

		if (doc.def) {
			await Promise.all(doc.def.modules.map((m) => this.addModule(m.name)));
			for (const imp of doc.def.imports) {
				if (imp.path.endsWith('.js')) tasks.push(this.addScript(resolvePath(imp.path, dir)));
				else tasks.push(this.addDirectorySingletons(resolvePath(imp.path, dir), Boolean(imp.alias)));
			}
			const pairs = [];
			const walk = (def) => {
				if (!BUILTIN_TYPES.has(def.type) && def.type !== 'Repeater') pairs.push([def.type, doc.path]);
				for (const child of def.children) walk(child);
			};
			walk(doc.def);
			for (const match of doc.source.matchAll(DYNAMIC_TYPE_RE)) {
				pairs.push([match[2], doc.path]);
				if (doc.path !== this.mainPath) pairs.push([match[2], this.mainPath]);
			}
			const seen = new Set();
			for (const [type, from] of pairs) {
				const key = `${from}\n${type}`;
				if (seen.has(key)) continue;
				seen.add(key);
				tasks.push(this.findComponent(type, from));
			}
		}

		for (const match of doc.source.matchAll(FONT_FILE_RE)) {
			tasks.push(this.addFont(resolvePath(match[2], dir)));
		}
		// Loader.source and setSource: QML files named by a string literal.
		for (const match of doc.source.matchAll(QML_FILE_RE)) tasks.push(this.addSingleton(resolvePath(match[2], dir)));
		await Promise.all(tasks);
	}

	async findComponent(type, fromFile) {
		if (BUILTIN_TYPES.has(type) || type === 'Repeater') return;
		for (const candidate of this.candidates(type, fromFile)) {
			if (this.docs.has(candidate)) return;
			if (this.missing.has(candidate)) continue;
			const doc = await this.addDoc(candidate, false);
			if (doc === null) {
				this.missing.add(candidate);
				continue;
			}
			await this.discover(doc);
			return;
		}
	}

	async addScript(path) {
		if (this.scripts.has(path)) return;
		this.scripts.set(path, '');
		const source = await this.fs.readText(path);
		if (source === null) {
			this.scripts.delete(path);
			return;
		}
		this.scripts.set(path, source);
		// `.import "Other.js" as Alias` lines of the script itself.
		const tasks = [];
		for (const match of source.matchAll(SCRIPT_IMPORT_RE)) tasks.push(this.addScript(resolvePath(match[1], dirname(path))));
		await Promise.all(tasks);
	}

	async addFont(path) {
		if (this.fontFamilies.has(path)) return;
		this.fontFamilies.set(path, '');
		const buffer = await this.fs.readBytes(path);
		if (buffer === null) {
			this.fontFamilies.delete(path);
			return;
		}
		const name = basename(path).replace(/\.[^.]*$/, '');
		this.fontFamilies.set(path, await fonts.registerFont(buffer, name));
	}

	// Paths where a component type used in `fromFile` can be: `Name` in the directory of the file and
	// in imported directories without alias, `Alias.Name` in the directory imported as Alias.
	candidates(type, fromFile) {
		const from = this.docs.get(fromFile);
		const dir = dirname(fromFile);
		const imports = from && from.def ? from.def.imports : [];
		const modules = from && from.def ? from.def.modules : [];
		const out = [];
		const moduleType = (module, name) => {
			const types = this.modules.get(module);
			const path = types ? types.get(name) : undefined;
			if (path !== undefined) out.push(path);
		};
		const dot = type.indexOf('.');
		if (dot >= 0) {
			const alias = type.slice(0, dot);
			const name = type.slice(dot + 1);
			for (const imp of imports) {
				if (imp.alias === alias && !imp.path.endsWith('.js')) {
					out.push(joinPath(resolvePath(imp.path, dir), `${name}.qml`));
				}
			}
			for (const m of modules) {
				if (m.alias === alias) moduleType(m.name, name);
			}
			return out;
		}
		out.push(joinPath(dir, `${type}.qml`));
		for (const imp of imports) {
			if (!imp.alias && !imp.path.endsWith('.js')) out.push(joinPath(resolvePath(imp.path, dir), `${type}.qml`));
		}
		for (const m of modules) {
			if (!m.alias) moduleType(m.name, type);
		}
		return out;
	}

	// The loaded document of a component type, or null.
	resolveType(type, fromFile) {
		for (const candidate of this.candidates(type, fromFile)) {
			const doc = this.docs.get(candidate);
			if (doc) return doc;
		}
		return null;
	}
}
