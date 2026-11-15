// Bundles the ES modules of src/ into one classic script (no build tools, no dependencies).
//
// The modules of src/ only use `import { a, b as c } from './x.js'` and `export class|function|const|let`
// and their import graph has no cycles, so every module becomes a function that returns its exports and
// the modules are emitted in dependency order. Every line of a module keeps its line number inside the
// module, which makes the source map a list of line offsets.

import { readFileSync } from 'node:fs';
import path from 'node:path';
import vm from 'node:vm';

const IMPORT_RE = /^import\s*\{([^}]*)\}\s*from\s*(['"])(\.{1,2}\/[^'"]+)\2\s*;?/gm;
const EXPORT_RE = /^export\s+(class|async\s+function|function|const|let|var)\s+([A-Za-z_$][\w$]*)/gm;
const UNSUPPORTED_RE = /^(?:export\s*(?:\{|default\b|\*)|import\s+(?!\{)|import\s*\()/m;

const BASE64 = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/';

function vlq(value) {
	let v = value < 0 ? (-value << 1) | 1 : value << 1;
	let out = '';
	do {
		let digit = v & 31;
		v >>>= 5;
		if (v > 0) digit |= 32;
		out += BASE64[digit];
	} while (v > 0);
	return out;
}

function toPosix(file) {
	return file.split(path.sep).join('/');
}

function countLines(text) {
	let count = 1;
	for (let i = 0; i < text.length; i++) if (text.charCodeAt(i) === 10) count++;
	return count;
}

// Reads a module and rewrites its imports and exports. The line count of the source is preserved.
function readModule(srcDir, file) {
	const source = readFileSync(path.join(srcDir, file), 'utf8');
	if (UNSUPPORTED_RE.test(source)) {
		throw new Error(`${file}: only 'import { ... } from' and 'export class|function|const|let' are supported by the bundler`);
	}
	const dependencies = [];
	let code = source.replace(IMPORT_RE, (whole, names, _quote, specifier) => {
		const target = path.posix.normalize(path.posix.join(path.posix.dirname(file), specifier));
		dependencies.push(target);
		const list = names
			.split(',')
			.map((name) => name.trim())
			.filter(Boolean)
			.map((name) => name.replace(/\s+as\s+/, ': '));
		const padding = '\n'.repeat(countLines(whole) - 1);
		return `const { ${list.join(', ')} } = __modules[${JSON.stringify(target)}];${padding}`;
	});
	const exports = [];
	code = code.replace(EXPORT_RE, (whole, kind, name) => {
		exports.push(name);
		return `${kind} ${name}`;
	});
	return { file, source, code, dependencies, exports };
}

// Modules reachable from `entry`, dependencies first.
function collectModules(srcDir, entry) {
	const modules = new Map();
	const order = [];
	const visiting = new Set();
	const visit = (file) => {
		if (modules.has(file)) return;
		if (visiting.has(file)) throw new Error(`${file}: circular import, the bundler needs an acyclic module graph`);
		visiting.add(file);
		const module = readModule(srcDir, file);
		for (const dependency of module.dependencies) visit(dependency);
		visiting.delete(file);
		modules.set(file, module);
		order.push(module);
	};
	visit(entry);
	return order;
}

// options: { srcDir, entry, banner, tail, file, sourceRoot }
// Returns { code, map, modules }: the script, its source map (version 3) and the bundled module names.
// `tail` runs inside the bundle after the modules and reaches them through `__modules`.
export function bundleRuntime(options) {
	const { srcDir, entry = 'runtime.js', banner = '', tail = '', file = 'bundle.js' } = options;
	const modules = collectModules(srcDir, entry);

	const generated = [];
	const sources = [];
	const sourcesContent = [];
	const mappings = []; // per generated line: null or [sourceIndex, sourceLine]

	const emit = (text) => {
		for (const line of text.replace(/\n$/, '').split('\n')) {
			generated.push(line);
			mappings.push(null);
		}
	};

	if (banner) emit(banner);
	emit('(function () {');
	emit("'use strict';");
	emit('const __modules = Object.create(null);');
	for (const module of modules) {
		const sourceIndex = sources.length;
		sources.push(options.sourceRoot ? toPosix(path.join(options.sourceRoot, module.file)) : module.file);
		sourcesContent.push(module.source);
		emit(`__modules[${JSON.stringify(module.file)}] = (function () {`);
		const lines = module.code.replace(/\n$/, '').split('\n');
		for (let i = 0; i < lines.length; i++) {
			generated.push(lines[i]);
			mappings.push([sourceIndex, i]);
		}
		emit(`return { ${module.exports.join(', ')} };`);
		emit('})();');
	}
	if (tail) emit(tail);
	emit('})();');
	emit(`//# sourceMappingURL=${path.basename(file)}.map`);

	let previousSource = 0;
	let previousLine = 0;
	const encoded = mappings.map((mapping) => {
		if (mapping === null) return '';
		const [sourceIndex, line] = mapping;
		const segment = vlq(0) + vlq(sourceIndex - previousSource) + vlq(line - previousLine) + vlq(0);
		previousSource = sourceIndex;
		previousLine = line;
		return segment;
	});

	const code = `${generated.join('\n')}\n`;
	// A syntax error here is a bug of the bundler or of a module, better found now than in the browser.
	new vm.Script(code, { filename: file });

	const map = {
		version: 3,
		file: path.basename(file),
		sources,
		sourcesContent,
		names: [],
		mappings: encoded.join(';'),
	};
	return { code, map, modules: modules.map((module) => module.file) };
}
