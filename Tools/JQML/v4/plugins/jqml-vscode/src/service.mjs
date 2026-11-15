// The JQML 4 language service: diagnostics, completion, hover, definition, references and outline for the text of
// a QML file. It knows nothing about VS Code; positions are zero-based { line, character } pairs, files are paths.

import fs from 'node:fs';
import path from 'node:path';
import { continuesAcrossNewline, QmlError } from '../lib/lexer.js';
import { parseQmlDocument } from '../lib/parser.js';
import { toPosix } from '../lib/config.js';
import { ProjectIndex } from './project.mjs';
import { frameAt, isUpper, objectFrameOf, parentFrame, scanText, statementBefore, tokenIndexAt } from './scanner.mjs';

const KEYWORDS_MEMBER = ['property', 'readonly property', 'default property', 'required property', 'signal', 'function', 'enum', 'id'];
const KEYWORDS_SCRIPT = ['true', 'false', 'null', 'undefined', 'var', 'let', 'const', 'function', 'if', 'else', 'for', 'while', 'return', 'switch', 'case', 'break', 'continue', 'new', 'typeof', 'try', 'catch', 'finally', 'throw', 'this'];
const GLOBALS = ['Qt', 'console', 'JSON', 'Math', 'Date', 'Array', 'Object', 'String', 'Number', 'Boolean', 'RegExp', 'parseInt', 'parseFloat', 'isNaN', 'Error', 'Promise', 'setTimeout', 'clearTimeout', 'setInterval', 'clearInterval', 'Component', 'model', 'modelData', 'index'];
const QT_MEMBERS = ['binding', 'rgba', 'hsla', 'hsva', 'lighter', 'darker', 'tint', 'point', 'rect', 'size', 'vector2d', 'vector3d', 'quit', 'openUrlExternally', 'createComponent', 'createQmlObject', 'formatDateTime', 'formatDate', 'formatTime', 'md5', 'atob', 'btoa', 'application', 'platform', 'resolvedUrl', 'locale', 'include', 'callLater', 'colorEqual', 'fontFamilies', 'inputMethod', 'uiLanguage'];
const CONSOLE_MEMBERS = ['log', 'warn', 'error', 'info', 'debug', 'assert', 'time', 'timeEnd', 'trace', 'count'];
// Properties of the Item family that exist in Qt but that the engine does not declare as accessors.
const EXTRA_ITEM_PROPS = new Set(['data', 'resources', 'states', 'transitions', 'state', 'children', 'parent', 'layer', 'smooth', 'baselineOffset', 'transform', 'containmentMask', 'visibleChildren', 'childrenRect', 'antialiasing', 'activeFocusOnTab', 'focus', 'enabled', 'objectName', 'implicitWidth', 'implicitHeight', 'transformOrigin', 'anchors', 'Layout', 'Component', 'Keys', 'KeyNavigation', 'delegate', 'model']);

// Levenshtein distance, for "did you mean" on a name that is a typo of a known one.
function distance(a, b) {
	if (Math.abs(a.length - b.length) > 2) return 3;
	let row = Array.from({ length: b.length + 1 }, (_, i) => i);
	for (let i = 1; i <= a.length; i++) {
		const next = [i];
		for (let j = 1; j <= b.length; j++) next[j] = Math.min(row[j] + 1, next[j - 1] + 1, row[j - 1] + (a[i - 1] === b[j - 1] ? 0 : 1));
		row = next;
	}
	return row[b.length];
}

function closestName(name, names) {
	if (name.length < 4) return null;
	let best = null;
	let bestDistance = 3;
	for (const candidate of names) {
		const d = distance(name.toLowerCase(), candidate.toLowerCase());
		if (d < bestDistance) {
			best = candidate;
			bestDistance = d;
		}
	}
	return bestDistance <= (name.length < 8 ? 1 : 2) ? best : null;
}

const lower = (text) => text.charAt(0).toLowerCase() + text.slice(1);
const upper = (text) => text.charAt(0).toUpperCase() + text.slice(1);

// `onClicked` -> `clicked`, `on_Foo` -> `_foo`; null when the name is not a handler name.
export function handlerSignal(name) {
	const match = /^on(_*)([A-Z])(.*)$/.exec(name);
	return match === null ? null : `${match[1]}${lower(match[2])}${match[3]}`;
}

// Is the offset inside a `//` comment of its line or inside a block comment?
function inComment(text, offset) {
	const open = text.lastIndexOf('/*', offset - 1);
	if (open >= 0) {
		const close = text.indexOf('*/', open + 2);
		if (close < 0 || close >= offset) return true;
	}
	const lineStart = text.lastIndexOf('\n', offset - 1) + 1;
	let quote = '';
	for (let i = lineStart; i < offset; i++) {
		const c = text[i];
		if (quote !== '') {
			if (c === '\\') i++;
			else if (c === quote) quote = '';
		} else if (c === '"' || c === "'" || c === '`') quote = c;
		else if (c === '/' && text[i + 1] === '/') return true;
	}
	return false;
}

class LineIndex {
	constructor(text) {
		this.starts = [0];
		for (let i = 0; i < text.length; i++) if (text.charCodeAt(i) === 10) this.starts.push(i + 1);
		this.length = text.length;
	}

	position(offset) {
		let lo = 0;
		let hi = this.starts.length - 1;
		while (lo < hi) {
			const mid = (lo + hi + 1) >> 1;
			if (this.starts[mid] <= offset) lo = mid;
			else hi = mid - 1;
		}
		return { line: lo, character: offset - this.starts[lo] };
	}
}

function tokenRange(tok) {
	return { start: { line: tok.line - 1, character: tok.col - 1 }, end: { line: tok.line - 1, character: tok.col - 1 + (tok.e - tok.s) } };
}

export class LanguageService {
	// options: { engineIndexPath, engineRoot, hideInternal, extraEnv, unknownTypes, unknownMembers }
	constructor(options = {}) {
		this.options = { hideInternal: true, unknownTypes: true, unknownMembers: true, ...options };
		const raw = fs.readFileSync(options.engineIndexPath, 'utf8');
		this.engine = JSON.parse(raw);
		this.project = new ProjectIndex();
		this.docs = new Map(); // path -> { text, scan, mtime, open }
		this.lastGood = new Set();
		this.engineRoot = options.engineRoot || '';
		this.configIssues = [];
	}

	// ----- configuration -----

	loadConfig(configPath, env = {}) {
		this.project.loadConfig(configPath, { ...process.env, ...env });
		this.configIssues = this.project.issues.slice();
	}

	setImportPaths(paths) {
		this.project.importPaths = paths.map((item) => toPosix(path.resolve(item)));
		this.project.invalidate();
	}

	indexQmldir(file) {
		this.project.directories.delete(toPosix(path.resolve(path.dirname(file))));
		this.project.addModuleDir(path.dirname(file));
		this.project.invalidate();
	}

	// ----- documents -----

	setDocument(file, text) {
		const key = toPosix(path.resolve(file));
		const known = this.docs.get(key);
		if (known !== undefined && known.open && known.text === text) return known;
		const doc = { text, scan: scanText(text), mtime: 0, open: true };
		this.docs.set(key, doc);
		return doc;
	}

	closeDocument(file) {
		this.docs.delete(toPosix(path.resolve(file)));
	}

	removeFile(file) {
		this.closeDocument(file);
	}

	doc(file) {
		const key = toPosix(path.resolve(file));
		const known = this.docs.get(key);
		if (known !== undefined && known.open) return known;
		let stat;
		try {
			stat = fs.statSync(key);
		} catch {
			this.docs.delete(key);
			return null;
		}
		if (known !== undefined && known.mtime === stat.mtimeMs) return known;
		const text = fs.readFileSync(key, 'utf8');
		const doc = { text, scan: scanText(text), mtime: stat.mtimeMs, open: false };
		this.docs.set(key, doc);
		return doc;
	}

	scanOf(file, text) {
		if (text !== undefined) return this.setDocument(file, text).scan;
		const doc = this.doc(file);
		return doc === null ? null : doc.scan;
	}

	// ----- types -----

	engineType(name) {
		const index = this.engine;
		if (index.types[name] !== undefined && index.types[name].exposed) return { kind: 'builtin', key: name, name };
		if (index.aliases[name] !== undefined) return { kind: 'builtin', key: index.aliases[name], name };
		if (index.opaque.includes(name)) return { kind: 'opaque', name };
		return null;
	}

	resolveType(typeName, file, scan) {
		const builtin = this.engineType(typeName);
		if (builtin !== null) return builtin;
		const found = this.project.resolveFile(typeName, file, scan);
		return found === null ? null : { kind: 'file', path: found, name: typeName };
	}

	typeRefOfFrame(frame, ctx) {
		return this.resolveType(frame.type, ctx.path, ctx.scan);
	}

	allTypeNames() {
		return [...Object.keys(this.engine.types).filter((name) => this.engine.types[name].exposed), ...Object.keys(this.engine.aliases), ...this.engine.opaque];
	}

	// All members of a type, the most derived first: Map name -> member; `complete` is false when the chain is not
	// fully known (an unresolved or an opaque base).
	members(ref) {
		const map = new Map();
		let complete = true;
		const seen = new Set();
		const add = (member) => {
			if (!map.has(member.name)) map.set(member.name, member);
		};
		const visit = (r) => {
			if (r === null || r === undefined || r.kind === 'opaque') {
				complete = false;
				return;
			}
			if (r.kind === 'builtin') {
				for (let key = r.key; key; key = (this.engine.types[key] || {}).base) {
					const entry = this.engine.types[key];
					if (entry === undefined) break;
					const owner = { builtin: key, line: entry.line };
					for (const [name, typeName] of Object.entries(entry.props)) add({ kind: 'property', name, typeName, owner, group: entry.groups[name] || null });
					for (const [name, params] of Object.entries(entry.signals)) add({ kind: 'signal', name, params, owner });
					for (const [name, count] of Object.entries(entry.methods)) add({ kind: 'method', name, params: Array.from({ length: count }, (_, i) => `arg${i + 1}`), owner });
				}
				return;
			}
			if (seen.has(r.path)) {
				complete = false;
				return;
			}
			seen.add(r.path);
			const doc = this.doc(r.path);
			if (doc === null || doc.scan.root === null) {
				complete = false;
				return;
			}
			const root = doc.scan.root;
			for (const decl of root.decls.values()) {
				const owner = { path: r.path, tok: decl.tok, file: path.basename(r.path) };
				add({ kind: decl.kind, name: decl.name, typeName: decl.typeName, params: decl.params, owner });
				if (decl.kind === 'property') add({ kind: 'signal', name: `${decl.name}Changed`, params: [], owner });
			}
			visit(this.resolveType(root.type, r.path, doc.scan));
		};
		visit(ref);
		return { map, complete };
	}

	// The members visible without a qualifier in a frame: declarations of the object and those of its type.
	frameMembers(frame, ctx) {
		const result = this.members(this.typeRefOfFrame(frame, ctx));
		for (const decl of frame.decls.values()) {
			const owner = { path: ctx.path, tok: decl.tok, file: path.basename(ctx.path) };
			result.map.set(decl.name, { kind: decl.kind, name: decl.name, typeName: decl.typeName, params: decl.params, owner });
			if (decl.kind === 'property') result.map.set(`${decl.name}Changed`, { kind: 'signal', name: `${decl.name}Changed`, params: [], owner });
		}
		return result;
	}

	// ----- context -----

	context(file, text) {
		const doc = text === undefined ? this.doc(file) : this.setDocument(file, text);
		const key = toPosix(path.resolve(file));
		return { path: key, text: doc.text, scan: doc.scan, lines: new LineIndex(doc.text) };
	}

	rangeOfOffsets(ctx, start, end) {
		return { start: ctx.lines.position(start), end: ctx.lines.position(end) };
	}

	locationOf(owner) {
		if (owner === undefined || owner === null) return null;
		if (owner.tok !== undefined) return { filePath: owner.path, range: tokenRange(owner.tok) };
		if (owner.builtin !== undefined && this.engineRoot) {
			const file = path.join(this.engineRoot, this.engine.sourceFile);
			if (fs.existsSync(file) && owner.line > 0) return { filePath: file, range: { start: { line: owner.line - 1, character: 0 }, end: { line: owner.line - 1, character: 0 } } };
		}
		return null;
	}

	// ----- diagnostics -----

	getDiagnostics(file, text) {
		const ctx = this.context(file, text);
		const out = [];
		let tree = null;
		try {
			tree = parseQmlDocument(ctx.text);
		} catch (error) {
			if (!(error instanceof QmlError)) throw error;
			const line = Math.max(0, error.line - 1);
			const character = Math.max(0, error.column - 1);
			const lineText = ctx.text.split(/\r?\n/)[line] || '';
			const word = /^[\w$]+/.exec(lineText.slice(character));
			const length = word !== null ? word[0].length : Math.max(1, Math.min(1, lineText.length - character));
			out.push({ severity: 'error', message: error.message, range: { start: { line, character }, end: { line, character: character + length } } });
			return out;
		}
		if (tree === null) return out;

		const scan = ctx.scan;
		const unresolvedModules = [];
		for (const imp of scan.imports) {
			if (imp.kind === 'module' && this.project.hasModuleSources() && !/^Qt/.test(imp.name) && this.project.module(imp.name) === null) {
				unresolvedModules.push(imp.name);
				out.push({ severity: 'information', message: `Module '${imp.name}' was not found. Check jqml4.configPath and jqml4.importPaths.`, range: { start: tokenRange(imp.tok).start, end: tokenRange(imp.endTok).end } });
			} else if (imp.kind === 'dir' && !fs.existsSync(path.resolve(path.dirname(ctx.path), imp.path))) {
				out.push({ severity: 'warning', message: `'${imp.path}' was not found`, range: tokenRange(imp.tok) });
			} else if (imp.kind === 'js' && !fs.existsSync(path.resolve(path.dirname(ctx.path), imp.path))) {
				out.push({ severity: 'warning', message: `'${imp.path}' was not found`, range: tokenRange(imp.tok) });
			}
		}
		const modulesUnknown = scan.imports.some((imp) => imp.kind === 'module' && !/^Qt/.test(imp.name) && this.project.module(imp.name) === null);

		// Duplicate ids inside one scope (a file, a Component or a delegate).
		const scopeOf = (frameIndex) => {
			for (let f = scan.frames[frameIndex]; f !== undefined; f = f.parent >= 0 ? scan.frames[f.parent] : undefined) {
				if (f.kind === 'object' && (f.type === 'Component' || f.propertyName === 'delegate')) return f.index;
			}
			return -1;
		};
		const seenIds = new Map();
		for (const decl of scan.idDecls) {
			const key = `${scopeOf(decl.frame)}:${decl.name}`;
			if (seenIds.has(key)) out.push({ severity: 'error', message: `Duplicate id '${decl.name}'`, range: tokenRange(decl.tok) });
			else seenIds.set(key, decl);
		}

		for (const frame of scan.frames) {
			if (frame.kind !== 'object') continue;
			const ref = this.resolveType(frame.type, ctx.path, scan);
			if (ref === null) {
				if (this.options.unknownTypes && !modulesUnknown) {
					const first = frame.typeToks[0];
					const last = frame.typeToks[frame.typeToks.length - 1];
					out.push({ severity: 'warning', message: `Unknown type '${frame.type}'`, range: { start: tokenRange(first).start, end: tokenRange(last).end } });
				}
				continue;
			}
			if (!this.options.unknownMembers || frame.onProperty) continue;
			const { map, complete } = this.frameMembers(frame, ctx);
			if (!complete) continue;
			for (const assign of frame.assigns) {
				const first = assign.toks[0].v;
				if (isUpper(first)) continue;
				const handler = assign.toks.length === 1 ? handlerSignal(first) : null;
				if (handler !== null) {
					if (!map.has(handler) && !map.has(first)) {
						const signals = [...map.values()].filter((item) => item.kind === 'signal').map((item) => item.name);
						const near = closestName(handler, signals);
						out.push({ severity: near === null ? 'information' : 'warning', message: `'${frame.type}' has no signal '${handler}'${near === null ? '' : `. Did you mean 'on${upper(near)}'?`}`, range: tokenRange(assign.tok) });
					}
					continue;
				}
				if (!map.has(first) && !EXTRA_ITEM_PROPS.has(first) && !(first === 'id')) {
					const near = closestName(first, [...map.values()].filter((item) => item.kind === 'property').map((item) => item.name));
					out.push({ severity: near === null ? 'information' : 'warning', message: `'${first}' is not a known property of ${frame.type}${near === null ? '' : `. Did you mean '${near}'?`}`, range: tokenRange(assign.tok) });
					continue;
				}
				const member = map.get(first);
				if (assign.toks.length > 1 && member !== undefined && member.group !== undefined && member.group !== null && !member.group.includes(assign.toks[1].v)) {
					out.push({ severity: 'warning', message: `'${assign.toks[1].v}' is not a member of '${first}'`, range: tokenRange(assign.toks[1]) });
				}
			}
		}
		return out;
	}

	// ----- chains and lookups -----

	// The thing a dotted chain `a.b.c` before the cursor stands for: { ref } | { group, names } | { special } | null.
	resolveChain(chain, ctx, frame) {
		const objFrame = objectFrameOf(ctx.scan, frame);
		const scan = ctx.scan;
		const first = chain[0];
		let current = null;
		const asTarget = (member) => {
			if (member === undefined) return null;
			if (member.group) return { group: member.name, names: member.group };
			if (member.kind === 'property' && member.typeName && isUpper(member.typeName)) {
				const ref = this.resolveType(member.typeName, ctx.path, scan);
				return ref === null ? null : { ref };
			}
			return null;
		};
		if (first === 'parent' && objFrame !== null) {
			let up = parentFrame(scan, objFrame);
			up = up === null ? null : objectFrameOf(scan, up);
			if (up !== null && up.kind === 'group') up = objectFrameOf(scan, parentFrame(scan, up));
			current = up !== null ? { ref: this.typeRefOfFrame(up, ctx), frame: up } : null;
		} else if (scan.ids.has(first)) {
			const target = scan.frames[scan.ids.get(first).frame];
			current = { ref: this.typeRefOfFrame(target, ctx), frame: target };
		} else if (first === 'Qt' || first === 'console') {
			current = { special: first };
		} else {
			const lexical = [objFrame, scan.root].filter(Boolean);
			for (const fr of lexical) {
				if (fr.kind === 'group') continue;
				const member = this.frameMembers(fr, ctx).map.get(first);
				if (member !== undefined) {
					current = asTarget(member);
					if (current === null && member.kind !== 'property') current = null;
					break;
				}
			}
			if (current === null) {
				const singleton = this.project.findSingleton(first, ctx.path, scan);
				if (singleton !== null) current = { ref: { kind: 'file', path: singleton, name: first } };
			}
		}
		for (let i = 1; i < chain.length && current !== null; i++) {
			if (current.names) {
				current = null;
				break;
			}
			if (current.ref === undefined || current.ref === null) {
				current = null;
				break;
			}
			current = asTarget(this.members(current.ref).map.get(chain[i]));
		}
		return current;
	}

	chainBefore(scan, index) {
		// tokens[index] is the last identifier of a dotted chain; returns the names, or null.
		const tokens = scan.tokens;
		if (tokens[index] === undefined || tokens[index].t !== 'id') return null;
		const names = [tokens[index].v];
		let k = index;
		while (k >= 2 && tokens[k - 1].t === 'p' && (tokens[k - 1].v === '.' || tokens[k - 1].v === '?.') && tokens[k - 2].t === 'id') {
			names.unshift(tokens[k - 2].v);
			k -= 2;
		}
		if (k >= 1 && tokens[k - 1].t === 'p' && (tokens[k - 1].v === '.' || tokens[k - 1].v === '?.')) return null;
		return names;
	}

	// ----- completion -----

	itemsFromMembers(members, mode, prefix = '') {
		const items = [];
		for (const member of members.values()) {
			if (this.options.hideInternal && member.name.startsWith('_')) continue;
			const owner = member.owner && member.owner.builtin ? member.owner.builtin : member.owner && member.owner.file ? member.owner.file : '';
			if (mode === 'assign') {
				if (member.kind === 'property') items.push({ label: member.name, kind: 'property', detail: `${member.typeName || 'var'} · ${owner}`, insertText: `${member.name}: ` });
				else if (member.kind === 'signal' && !member.name.endsWith('Changed')) items.push({ label: `on${upper(member.name)}`, kind: 'handler', detail: `signal ${member.name}(${(member.params || []).join(', ')}) · ${owner}`, insertText: `on${upper(member.name)}: ` });
				else if (member.kind === 'signal') items.push({ label: `on${upper(member.name)}`, kind: 'handler', detail: `signal ${member.name} · ${owner}`, insertText: `on${upper(member.name)}: ` });
			} else if (member.kind === 'property') {
				items.push({ label: member.name, kind: 'property', detail: `${member.typeName || 'var'} · ${owner}` });
			} else if (member.kind === 'method') {
				items.push({ label: member.name, kind: 'method', detail: `function ${member.name}(${(member.params || []).join(', ')}) · ${owner}` });
			} else if (member.kind === 'signal') {
				items.push({ label: member.name, kind: 'signal', detail: `signal ${member.name}(${(member.params || []).join(', ')}) · ${owner}` });
			} else if (member.kind === 'enum') {
				items.push({ label: member.name, kind: 'enum', detail: `enum · ${owner}` });
			}
		}
		return items;
	}

	typeItems(ctx) {
		const items = this.allTypeNames().map((name) => ({ label: name, kind: 'class', detail: 'built-in' }));
		for (const type of this.project.visibleTypes(ctx.path, ctx.scan)) {
			if (type.path === ctx.path) continue;
			items.push({ label: type.name, kind: 'class', detail: type.singleton ? `singleton · ${path.basename(type.path)}` : path.basename(type.path) });
		}
		return items;
	}

	getCompletions(file, text, offset) {
		const ctx = this.context(file, text);
		const { scan } = ctx;
		const tokens = scan.tokens;
		const idx = tokenIndexAt(scan, offset);
		const here = idx >= 0 ? tokens[idx] : null;

		if (here !== null && (here.t === 'str' || here.t === 'tpl') && here.s < offset && offset < here.e) return [];
		let partialStart = offset;
		if (here !== null && here.t === 'id' && here.s < offset && here.e >= offset) partialStart = here.s;
		if (inComment(ctx.text, offset)) return [];

		let prevIdx = -1;
		for (let k = 0; k < tokens.length && tokens[k].e <= partialStart; k++) prevIdx = k;
		const prev = prevIdx >= 0 ? tokens[prevIdx] : null;
		const frame = frameAt(scan, offset);

		// imports at the top of the file
		const lineStart = ctx.text.lastIndexOf('\n', offset - 1) + 1;
		const rootStart = scan.root === null ? Infinity : scan.root.typeToks[0].s;
		if (frame === null && partialStart < rootStart) {
			const line = ctx.text.slice(lineStart, partialStart).trim();
			if (line === 'import') return this.project.allModuleNames().map((name) => ({ label: name, kind: 'module', detail: 'module' }));
			if (line === '') return [{ label: 'import', kind: 'keyword' }, { label: 'pragma', kind: 'keyword' }, ...this.typeItems(ctx)];
			return [];
		}
		if (frame === null) return this.typeItems(ctx);

		if (prev !== null && prev.t === 'p' && (prev.v === '.' || prev.v === '?.')) {
			const chain = this.chainBefore(scan, prevIdx - 1);
			if (chain === null) return [];
			return this.dotCompletions(chain, ctx, frame);
		}

		if (frame.kind === 'script') return this.scriptCompletions(ctx, frame);

		// An object or a group: a member name or a value?
		let stmt = statementBefore(scan, frame, partialStart);
		if (stmt.length > 0) {
			const last = stmt[stmt.length - 1];
			const gap = ctx.text.slice(last.e, partialStart);
			if (gap.includes('\n') && !continuesAcrossNewline(last, { t: 'id', v: 'x' })) stmt = [];
		}
		if (stmt.some((tk) => tk.t === 'p' && tk.v === ':')) return this.scriptCompletions(ctx, frame);
		if (stmt.length > 0) {
			const words = stmt.map((tk) => tk.v);
			if (words.every((word) => ['property', 'readonly', 'default', 'required'].includes(word)) && words.includes('property')) {
				return [...['int', 'real', 'string', 'bool', 'var', 'color', 'url', 'alias', 'list', 'variant', 'double', 'date'].map((label) => ({ label, kind: 'keyword' })), ...this.typeItems(ctx)];
			}
			return [];
		}
		if (frame.kind === 'group') {
			const owner = parentFrame(scan, frame);
			const info = owner === null ? null : this.frameMembers(owner, ctx).map.get(frame.type);
			return info && info.group ? info.group.map((name) => ({ label: name, kind: 'property', insertText: `${name}: ` })) : [];
		}
		const { map } = this.frameMembers(frame, ctx);
		const items = this.itemsFromMembers(map, 'assign');
		for (const member of map.values()) if (member.group) items.push({ label: member.name, kind: 'property', detail: 'group', insertText: member.name });
		for (const word of KEYWORDS_MEMBER) items.push({ label: word, kind: 'keyword' });
		items.push(...this.typeItems(ctx));
		return items;
	}

	scriptCompletions(ctx, frame) {
		const { scan } = ctx;
		const items = [];
		for (const id of scan.ids.values()) items.push({ label: id.name, kind: 'variable', detail: `id · ${scan.frames[id.frame].type}` });
		items.push({ label: 'parent', kind: 'variable', detail: 'parent object' });
		const objFrame = objectFrameOf(scan, frame);
		const seen = new Set();
		for (const fr of [objFrame, scan.root]) {
			if (fr === null || fr === undefined || fr.kind === 'group' || seen.has(fr.index)) continue;
			seen.add(fr.index);
			items.push(...this.itemsFromMembers(this.frameMembers(fr, ctx).map, 'value'));
		}
		for (const name of GLOBALS) items.push({ label: name, kind: 'variable' });
		for (const word of KEYWORDS_SCRIPT) items.push({ label: word, kind: 'keyword' });
		for (const type of this.project.visibleTypes(ctx.path, ctx.scan)) if (type.singleton) items.push({ label: type.name, kind: 'class', detail: 'singleton' });
		const unique = new Map();
		for (const item of items) if (!unique.has(item.label)) unique.set(item.label, item);
		return [...unique.values()];
	}

	dotCompletions(chain, ctx, frame) {
		const target = this.resolveChain(chain, ctx, frame);
		if (target === null) return [];
		if (target.special === 'Qt') return QT_MEMBERS.map((label) => ({ label, kind: 'method', detail: 'Qt' }));
		if (target.special === 'console') return CONSOLE_MEMBERS.map((label) => ({ label, kind: 'method', detail: 'console' }));
		if (target.names) return target.names.map((label) => ({ label, kind: 'property', detail: target.group }));
		return this.itemsFromMembers(this.members(target.ref).map, 'value');
	}

	// ----- symbol resolution: hover and definition -----

	memberDescription(member) {
		if (member.kind === 'property') return `property ${member.typeName || 'var'} ${member.name}`;
		if (member.kind === 'signal') return `signal ${member.name}(${(member.params || []).join(', ')})`;
		if (member.kind === 'method') return `function ${member.name}(${(member.params || []).join(', ')})`;
		if (member.kind === 'enum') return `enum ${member.name}`;
		return member.name;
	}

	resolveAt(file, text, offset) {
		const ctx = this.context(file, text);
		const { scan } = ctx;
		const tokens = scan.tokens;
		const idx = tokenIndexAt(scan, offset);
		if (idx < 0) return null;
		const tok = tokens[idx];
		const range = tokenRange(tok);

		// import "folder", import Module
		for (const imp of scan.imports) {
			if (tok.s >= imp.tok.s && tok.e <= imp.endTok.e) {
				if (imp.kind === 'module') {
					const files = this.project.moduleFiles(imp.name);
					return { range: { start: tokenRange(imp.tok).start, end: tokenRange(imp.endTok).end }, detail: `module ${imp.name}`, locations: files.map((filePath) => ({ filePath, range: { start: { line: 0, character: 0 }, end: { line: 0, character: 0 } } })) };
				}
				const target = path.resolve(path.dirname(ctx.path), imp.path);
				const filePath = imp.kind === 'js' ? target : path.join(target, 'qmldir');
				return { range: tokenRange(imp.tok), detail: `${imp.kind === 'js' ? 'script' : 'folder'} ${imp.path}`, locations: fs.existsSync(filePath) ? [{ filePath: toPosix(filePath), range: { start: { line: 0, character: 0 }, end: { line: 0, character: 0 } } }] : [] };
			}
		}
		if (tok.t !== 'id') return null;
		const name = tok.v;
		const frame = frameAt(scan, tok.s + 1);

		const typeResult = (ref, typeName) => {
			if (ref === null) return null;
			if (ref.kind === 'file') {
				const doc = this.doc(ref.path);
				const rootTok = doc && doc.scan.root ? doc.scan.root.typeToks[0] : null;
				const where = rootTok === null ? { start: { line: 0, character: 0 }, end: { line: 0, character: 0 } } : tokenRange(rootTok);
				return { range, detail: `type ${typeName}\n// ${path.basename(ref.path)}`, locations: [{ filePath: ref.path, range: where }] };
			}
			if (ref.kind === 'builtin') {
				const entry = this.engine.types[ref.key];
				const loc = this.locationOf({ builtin: ref.key, line: entry.line });
				return { range, detail: `type ${typeName}\n// built-in${entry.base ? `, extends ${entry.base}` : ''}`, locations: loc === null ? [] : [loc] };
			}
			return { range, detail: `type ${typeName}\n// built-in`, locations: [] };
		};

		// the type of an object
		const header = scan.frames.find((fr) => fr.kind === 'object' && fr.typeToks.includes(tok));
		if (header !== undefined) {
			const index = header.typeToks.indexOf(tok);
			const typeName = header.typeToks.slice(0, index + 1).map((item) => item.v).join('.');
			return typeResult(this.resolveType(typeName === header.type ? header.type : typeName, ctx.path, scan) || this.resolveType(header.type, ctx.path, scan), header.type);
		}
		if (frame === null) return null;
		const objFrame = objectFrameOf(scan, frame);

		// a declaration itself, or the type of a property declaration
		for (const fr of scan.frames) {
			for (const decl of fr.decls.values()) {
				if (decl.tok === tok) {
					return { range, detail: this.memberDescription(decl), locations: [{ filePath: ctx.path, range }] };
				}
				if (decl.typeTok === tok && isUpper(tok.v)) return typeResult(this.resolveType(tok.v, ctx.path, scan), tok.v);
			}
		}
		for (const decl of scan.idDecls) {
			if (decl.tok === tok) return { range, detail: `id ${name}: ${scan.frames[decl.frame].type}`, locations: [{ filePath: ctx.path, range }] };
		}

		const describe = (member) => ({ range, detail: `${this.memberDescription(member)}${member.owner && (member.owner.file || member.owner.builtin) ? `\n// ${member.owner.file || member.owner.builtin}` : ''}`, locations: [this.locationOf(member.owner)].filter(Boolean) });
		const lookup = (map, key) => {
			let member = map.get(key);
			if (member === undefined) {
				const signal = handlerSignal(key);
				if (signal !== null) member = map.get(signal);
			}
			return member;
		};

		// a.b: the member of what is before the dot
		const prev = tokens[idx - 1];
		if (prev !== undefined && prev.t === 'p' && (prev.v === '.' || prev.v === '?.')) {
			const chain = this.chainBefore(scan, idx - 2);
			if (chain === null) return null;
			const target = this.resolveChain(chain, ctx, frame);
			if (target === null) return null;
			if (target.names) return target.names.includes(name) ? { range, detail: `property ${name}\n// ${target.group}`, locations: [] } : null;
			if (target.special) return { range, detail: `${target.special}.${name}`, locations: [] };
			const member = lookup(this.members(target.ref).map, name);
			return member === undefined ? null : describe(member);
		}

		// the key of an assignment in an object
		if (objFrame !== null) {
			const assign = objFrame.assigns.find((item) => item.toks[0] === tok);
			if (assign !== undefined) {
				const member = lookup(this.frameMembers(objFrame, ctx).map, name);
				if (member !== undefined) return describe(member);
			}
		}

		if (scan.ids.has(name)) {
			const decl = scan.ids.get(name);
			return { range, detail: `id ${name}: ${scan.frames[decl.frame].type}`, locations: [{ filePath: ctx.path, range: tokenRange(decl.tok) }] };
		}
		if (name === 'parent') return { range, detail: 'parent', locations: [] };
		for (const fr of [objFrame, scan.root]) {
			if (fr === null || fr === undefined || fr.kind === 'group') continue;
			const member = lookup(this.frameMembers(fr, ctx).map, name);
			if (member !== undefined) return describe(member);
		}
		const singleton = this.project.findSingleton(name, ctx.path, scan);
		if (singleton !== null) return typeResult({ kind: 'file', path: singleton, name }, name);
		if (isUpper(name)) return typeResult(this.resolveType(name, ctx.path, scan), name);
		return null;
	}

	getDefinition(file, text, offset) {
		const result = this.resolveAt(file, text, offset);
		return result === null ? [] : result.locations;
	}

	getHover(file, text, offset) {
		const result = this.resolveAt(file, text, offset);
		return result === null ? null : { detail: result.detail, range: result.range };
	}

	// All uses of the identifier under the cursor in the file (by name).
	getReferences(file, text, offset) {
		const ctx = this.context(file, text);
		const idx = tokenIndexAt(ctx.scan, offset);
		if (idx < 0 || ctx.scan.tokens[idx].t !== 'id') return [];
		const name = ctx.scan.tokens[idx].v;
		const out = [];
		ctx.scan.tokens.forEach((tok) => {
			if (tok.t !== 'id' || tok.v !== name) return;
			out.push({ filePath: ctx.path, range: tokenRange(tok) });
		});
		return out;
	}

	// ----- outline -----

	getSymbols(file, text) {
		const ctx = this.context(file, text);
		const { scan } = ctx;
		const build = (frame) => {
			const children = [];
			for (const decl of frame.decls.values()) {
				children.push({ name: decl.name, detail: decl.kind, kind: decl.kind === 'method' ? 'method' : decl.kind === 'signal' ? 'event' : decl.kind === 'enum' ? 'enum' : 'property', range: tokenRange(decl.tok), selection: tokenRange(decl.tok), children: [] });
			}
			for (const child of scan.frames) {
				if (child.kind === 'object' && this.nearestObject(scan, child) === frame.index) children.push(build(child));
			}
			const title = frame.id ? `${frame.type} #${frame.id}` : frame.type;
			const first = frame.typeToks[0];
			const end = frame.end === Infinity ? ctx.text.length : frame.end + 1;
			return { name: title, detail: '', kind: 'class', range: { start: tokenRange(first).start, end: ctx.lines.position(end) }, selection: { start: tokenRange(first).start, end: tokenRange(frame.typeToks[frame.typeToks.length - 1]).end }, children };
		};
		return scan.root === null ? [] : [build(scan.root)];
	}

	nearestObject(scan, frame) {
		const up = frame.parent >= 0 ? objectFrameOf(scan, scan.frames[frame.parent]) : null;
		return up === null ? -1 : up.index;
	}
}
