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
const QMLDIR_TYPE_RE = /^(singleton\s+)?([A-Z]\w*)\s+(\d+)(?:\.(\d+))?\s+(\S+\.(?:qml|js))\s*$/;

// The newest of the declarations `{ major, minor, path }` of a name; the first one among equal versions.
function newest(entries) {
	let best = entries[0];
	for (const entry of entries) {
		if (entry.major > best.major || (entry.major === best.major && entry.minor > best.minor)) best = entry;
	}
	return best;
}

// The declaration that an import of `version` ({ major, minor } or null for the latest) gets: the newest one of its
// major version that is not above it. An import that nothing fits gets the newest, which is more than Qt gives.
function pickVersion(entries, version) {
	if (entries === undefined || entries.length === 0) return undefined;
	if (version !== null && version !== undefined) {
		const fitting = entries.filter((entry) => entry.major === version.major && entry.minor <= version.minor);
		if (fitting.length > 0) return newest(fitting);
	}
	return newest(entries);
}

export class Project {
	// options.importPaths: folders searched for `<Module>/qmldir` to resolve `import Module 1.0`.
	// options.modules: module name -> directory (or array of directories) whose qmldir files declare the module;
	// checked before importPaths. Several directories may declare the same module (the compiler config lists
	// `imtcontrols` and `imtcontrols/Popup/Internal` separately), their types are merged. A type or a singleton that
	// several of them declare comes from the last directory: the config lists the directories of its includes first, so
	// the application overrides a library, like in the JQML v3 compiler (the `Style` of an application replaces the
	// one of the library).
	constructor(fs, options = {}) {
		this.fs = fs;
		this.importPaths = options.importPaths || [];
		this.moduleDirs = new Map();
		for (const [name, dirs] of Object.entries(options.modules || {})) {
			this.moduleDirs.set(name, Array.isArray(dirs) ? dirs : [dirs]);
		}
		this.modules = new Map(); // module name -> Map(type name -> path of the newest version), null if not found
		this.moduleTypeVersions = new Map(); // module name -> Map(type name -> [{ major, minor, path }])
		this.moduleSingletons = new Map(); // module name -> Map(singleton name -> [{ major, minor, path }])
		this.dirSingletons = new Map(); // directory -> Map(singleton name -> path) of its qmldir
		this.moduleLoads = new Map(); // module name -> Promise
		this.pending = []; // promises of singleton documents still being discovered
		this.singletons = new Map(); // singleton name -> path (of all loaded modules)
		this.jsModules = new Map(); // global name -> path of a JavaScript file listed in a qmldir
		this.mainPath = '';
		this.docs = new Map(); // path -> { path, source, def, error }
		this.scripts = new Map(); // path -> source
		this.fontFamilies = new Map(); // path -> family name ('' if it could not be loaded)
		this.missing = new Set();
		this.directoryLoads = new Map(); // directory -> Promise of the read of its qmldir
		this.directoryGlobal = new Set(); // directories whose singletons are global names
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
			const versions = new Map();
			const singletons = new Map();
			// The first declaration is kept, so the last directory goes first.
			for (const dir of [...explicit].reverse()) {
				const source = await this.fs.readText(joinPath(dir, 'qmldir'));
				if (source !== null) this.readQmldir(dir, source, types, versions, singletons);
			}
			this.modules.set(name, types);
			this.moduleTypeVersions.set(name, versions);
			this.moduleSingletons.set(name, singletons);
			return;
		}
		for (const base of this.importPaths) {
			const dir = joinPath(base, name.replace(/\./g, '/'));
			const source = await this.fs.readText(joinPath(dir, 'qmldir'));
			if (source === null) continue;
			const types = new Map();
			const versions = new Map();
			const singletons = new Map();
			this.readQmldir(dir, source, types, versions, singletons);
			this.modules.set(name, types);
			this.moduleTypeVersions.set(name, versions);
			this.moduleSingletons.set(name, singletons);
			return;
		}
	}

	// Registers the types, singletons and JavaScript files of one qmldir: `types` get the newest version of a type,
	// `versions` and `singletonVersions` all of them. What an earlier call declared wins over a later one.
	readQmldir(dir, source, types, versions = new Map(), singletonVersions = new Map()) {
		const singletonPaths = [];
		const local = new Map();
		const localSingletons = new Map();
		for (const line of source.split(/\r?\n/)) {
			const match = QMLDIR_TYPE_RE.exec(line.trim());
			if (match === null) continue;
			const path = joinPath(dir, match[5]);
			if (path.endsWith('.js')) {
				if (!this.jsModules.has(match[2])) {
					this.jsModules.set(match[2], path);
					this.pending.push(this.addScript(path));
				}
				continue;
			}
			const entry = { major: Number(match[3]), minor: Number(match[4] || 0), path };
			const target = match[1] ? singletonVersions : versions;
			if (!target.has(match[2])) target.set(match[2], []);
			target.get(match[2]).push(entry);
			if (match[1]) {
				singletonPaths.push(path);
				if (!localSingletons.has(match[2])) localSingletons.set(match[2], []);
				localSingletons.get(match[2]).push(entry);
			} else {
				if (!local.has(match[2])) local.set(match[2], []);
				local.get(match[2]).push(entry);
			}
		}
		for (const [name, entries] of local) {
			if (!types.has(name)) types.set(name, newest(entries).path);
		}
		// The global name of a singleton is its newest version.
		for (const [name, entries] of localSingletons) {
			if (!this.singletons.has(name)) this.singletons.set(name, newest(entries).path);
		}
		// Singleton documents are discovered later by load() (they may import this very module).
		for (const path of singletonPaths) this.pending.push(this.addSingleton(path));
	}

	async addSingleton(path) {
		if (this.docs.has(path)) return;
		const doc = await this.addDoc(path, false);
		if (doc !== null) await this.discover(doc);
	}

	// `import "Internal"`: the `singleton` and JavaScript lines of the qmldir of an imported directory. The singletons
	// are global names, except those of a directory that is imported with an alias (`Alias.Name`).
	async addDirectorySingletons(dir, aliased = false) {
		let loading = this.directoryLoads.get(dir);
		if (loading === undefined) {
			loading = this.readDirectory(dir);
			this.directoryLoads.set(dir, loading);
		}
		await loading;
		const singletons = this.dirSingletons.get(dir);
		if (aliased || singletons === undefined || this.directoryGlobal.has(dir)) return;
		this.directoryGlobal.add(dir);
		for (const [name, path] of singletons) {
			if (!this.singletons.has(name)) this.singletons.set(name, path);
		}
	}

	async readDirectory(dir) {
		const source = await this.fs.readText(joinPath(dir, 'qmldir'));
		if (source === null) return;
		const singletons = new Map();
		for (const line of source.split(/\r?\n/)) {
			const match = QMLDIR_TYPE_RE.exec(line.trim());
			if (match === null) continue;
			const path = joinPath(dir, match[5]);
			if (match[5].endsWith('.js')) {
				if (!this.jsModules.has(match[2])) {
					this.jsModules.set(match[2], path);
					this.pending.push(this.addScript(path));
				}
				continue;
			}
			if (!match[1] || singletons.has(match[2])) continue;
			singletons.set(match[2], path);
			this.pending.push(this.addSingleton(path));
		}
		this.dirSingletons.set(dir, singletons);
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

	// `qrc:/Fonts/Name.ttf` is the Qt resource of imtstylecontrolsqml; on disk the files live next to
	// the Acf module as `../Fonts/Name.ttf`. The compiler FS has no qrc, so those paths are rewritten.
	fontFileCandidates(path) {
		const normalized = normalizePath(path);
		const out = [normalized];
		const qrcFonts = /^qrc:\/+Fonts\/([^/]+)$/i.exec(normalized);
		if (qrcFonts) {
			const file = qrcFonts[1];
			const underFonts = `Fonts/${file}`;
			out.push(underFonts);
			for (const dirs of this.moduleDirs.values()) {
				for (const dir of dirs) out.push(joinPath(dirname(dir), underFonts));
			}
			for (const dir of this.importPaths) {
				out.push(joinPath(dir, underFonts), joinPath(dirname(dir), underFonts));
			}
		}
		return out;
	}

	async addFont(path) {
		if (this.fontFamilies.has(path)) return;
		this.fontFamilies.set(path, '');
		let buffer = null;
		let resolved = path;
		for (const candidate of this.fontFileCandidates(path)) {
			buffer = await this.fs.readBytes(candidate);
			if (buffer !== null) {
				resolved = candidate;
				break;
			}
		}
		if (buffer === null) {
			this.fontFamilies.delete(path);
			return;
		}
		const name = basename(resolved).replace(/\.[^.]*$/, '');
		const family = await fonts.registerFont(buffer, name);
		this.fontFamilies.set(path, family);
		if (resolved !== path) this.fontFamilies.set(resolved, family);
	}

	// The imports of a document that are modules or directories, in the order of the file.
	fileImports(doc) {
		if (doc.imports !== undefined) return doc.imports;
		const list = [];
		if (doc.def) {
			for (const imp of doc.def.imports) {
				if (!imp.path.endsWith('.js')) list.push({ kind: 'directory', path: imp.path, alias: imp.alias, version: null, line: imp.line });
			}
			for (const m of doc.def.modules) list.push({ kind: 'module', name: m.name, alias: m.alias, version: m.version === undefined ? null : m.version, line: m.line });
			list.sort((a, b) => a.line - b.line);
		}
		doc.imports = list;
		return list;
	}

	// The path of a type of a module for an import of `version`.
	moduleTypePath(module, name, version) {
		const versions = this.moduleTypeVersions.get(module);
		const entry = pickVersion(versions === undefined ? undefined : versions.get(name), version);
		if (entry !== undefined) return entry.path;
		const types = this.modules.get(module);
		return types ? types.get(name) : undefined;
	}

	// The path of a singleton of a module for an import of `version`.
	moduleSingletonPath(module, name, version) {
		const singletons = this.moduleSingletons.get(module);
		const entry = pickVersion(singletons === undefined ? undefined : singletons.get(name), version);
		return entry === undefined ? undefined : entry.path;
	}

	// Paths where a component type used in `fromFile` can be, the best first. Like in Qt the later import wins over an
	// earlier one, and every import wins over the directory of the file: `Name` in the imported directories and modules
	// without alias, then in the directory of the file; `Alias.Name` in what is imported as Alias.
	candidates(type, fromFile) {
		const from = this.docs.get(fromFile);
		const dir = dirname(fromFile);
		const imports = from ? this.fileImports(from) : [];
		const out = [];
		const dot = type.indexOf('.');
		const alias = dot >= 0 ? type.slice(0, dot) : '';
		const name = dot >= 0 ? type.slice(dot + 1) : type;
		for (let i = imports.length - 1; i >= 0; i--) {
			const imp = imports[i];
			if (imp.alias !== alias) continue;
			if (imp.kind === 'directory') {
				out.push(joinPath(resolvePath(imp.path, dir), `${name}.qml`));
			} else {
				const path = this.moduleTypePath(imp.name, name, imp.version);
				if (path !== undefined) out.push(path);
			}
		}
		if (dot < 0) out.push(joinPath(dir, `${type}.qml`));
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
