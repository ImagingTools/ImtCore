// Prepares lib/: a copy of the JQML v4 lexer, parser and config reader, and engine-index.json, the
// description of the built-in types (members, signals, base types) read from the running engine classes.

import fs from 'node:fs';
import path from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';

const extensionRoot = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const v4Root = path.resolve(extensionRoot, '..', '..');
const libDir = path.join(extensionRoot, 'lib');

fs.mkdirSync(libDir, { recursive: true });
fs.writeFileSync(path.join(libDir, 'package.json'), '{ "type": "module" }\n');
fs.copyFileSync(path.join(v4Root, 'src', 'lexer.js'), path.join(libDir, 'lexer.js'));
fs.copyFileSync(path.join(v4Root, 'src', 'parser.js'), path.join(libDir, 'parser.js'));
fs.copyFileSync(path.join(v4Root, 'compiler', 'config.js'), path.join(libDir, 'config.js'));

const itemsPath = path.join(v4Root, 'src', 'items.js');
const items = await import(pathToFileURL(itemsPath).href);
const itemsSource = fs.readFileSync(itemsPath, 'utf8');
const engineSource = fs.readFileSync(path.join(v4Root, 'src', 'engine.js'), 'utf8');

function setFromSource(source, name) {
	const match = new RegExp(`const ${name} = new Set\\(\\[([\\s\\S]*?)\\]\\)`).exec(source);
	if (match === null) throw new Error(`${name} was not found`);
	return [...match[1].matchAll(/'([^']+)'/g)].map((item) => item[1]);
}

const classLines = new Map();
itemsSource.split('\n').forEach((line, index) => {
	const match = /^(?:export )?class (\w+)/.exec(line);
	if (match !== null) classLines.set(match[1], index + 1);
});

// QML name -> class. Types the engine creates in its own way are added by name.
const registered = new Map(items.BUILTIN_TYPES);
for (const name of ['Loader', 'Repeater', 'ListModel', 'TreeItemModel', 'ListView', 'GridView', 'Stack', 'Animation']) {
	if (items[name] !== undefined && !registered.has(name)) registered.set(name, items[name]);
}
const stackTypes = setFromSource(engineSource, 'STACK_TYPES');
const animationTypes = [...items.ANIMATION_TYPES];
const opaque = [
	...setFromSource(engineSource, 'IGNORED_TYPES'),
	...setFromSource(engineSource, 'SILENT_TYPES'),
	'Component', 'ListElement', 'FocusScope', 'Window', 'Connections', 'Binding',
];

const classKey = new Map(); // class -> key of the index
for (const [name, cls] of registered) if (!classKey.has(cls)) classKey.set(cls, name);
const keyOf = (cls) => classKey.get(cls) || cls.name;

function instantiate(cls) {
	try {
		return new cls();
	} catch {
		return null;
	}
}

function valueType(instance, name) {
	try {
		const value = instance[name];
		if (typeof value === 'number') return Number.isInteger(value) ? 'int' : 'real';
		if (typeof value === 'string') return 'string';
		if (typeof value === 'boolean') return 'bool';
		return 'var';
	} catch {
		return 'var';
	}
}

function groupMembers(instance, name) {
	try {
		const value = instance[name];
		if (value === null || typeof value !== 'object' || Array.isArray(value)) return null;
		const names = new Set();
		for (let proto = value; proto !== null && proto !== Object.prototype; proto = Object.getPrototypeOf(proto)) {
			for (const own of Object.getOwnPropertyNames(proto)) {
				if (!own.startsWith('$') && own !== 'constructor' && !own.endsWith('Changed')) names.add(own);
			}
		}
		return names.size > 0 ? [...names].sort() : null;
	} catch {
		return null;
	}
}

const types = {};

function describe(cls) {
	const key = keyOf(cls);
	if (types[key] !== undefined) return key;
	const parent = Object.getPrototypeOf(cls);
	const parentKey = parent !== null && parent.prototype !== undefined && parent !== Function.prototype ? describe(parent) : null;
	const entry = { base: parentKey, line: classLines.get(cls.name) || 0, className: cls.name, exposed: registered.has(key), props: {}, methods: {}, signals: {}, groups: {} };
	types[key] = entry;

	const instance = instantiate(cls);
	const proto = cls.prototype;
	const names = Object.getOwnPropertyNames(proto).filter((name) => name !== 'constructor' && !name.startsWith('$'));
	const props = new Set();
	for (const name of names) {
		const descriptor = Object.getOwnPropertyDescriptor(proto, name);
		if (descriptor.get === undefined && descriptor.set === undefined) continue;
		if (!name.endsWith('Changed')) props.add(name);
	}
	for (const name of names) {
		const descriptor = Object.getOwnPropertyDescriptor(proto, name);
		if (typeof descriptor.value === 'function') {
			entry.methods[name] = descriptor.value.length;
		} else if (name.endsWith('Changed') && descriptor.get !== undefined) {
			entry.signals[name] = [];
		}
	}
	for (const name of props) {
		entry.props[name] = instance === null ? 'var' : valueType(instance, name);
		const group = instance === null ? null : groupMembers(instance, name);
		if (group !== null) {
			entry.props[name] = 'group';
			entry.groups[name] = group;
		}
	}
	// Properties the constructor defines on the instance itself.
	if (instance !== null) {
		for (const name of Object.getOwnPropertyNames(instance)) {
			if (name.startsWith('$') || name in entry.props || name in entry.signals || name in entry.methods) continue;
			const descriptor = Object.getOwnPropertyDescriptor(instance, name);
			if (typeof descriptor.value === 'function') continue;
			entry.props[name] = valueType(instance, name);
		}
	}
	// Signals declared by the constructor ($declareSignal) and not by the base type.
	if (instance !== null && instance.$sigs instanceof Map) {
		const inherited = parent !== null && parent.prototype !== undefined ? instantiate(parent) : null;
		for (const [name, signal] of instance.$sigs) {
			if (inherited !== null && inherited.$sigs instanceof Map && inherited.$sigs.has(name)) continue;
			entry.signals[name] = (signal.params || []).map(String);
			delete entry.props[name];
		}
	}
	return key;
}

for (const cls of registered.values()) describe(cls);

const aliases = {};
for (const name of stackTypes) aliases[name] = types.Stack === undefined ? null : 'Stack';
for (const name of animationTypes) aliases[name] = types.Animation === undefined ? null : 'Animation';
for (const name of Object.keys(aliases)) if (aliases[name] === null) delete aliases[name];

const index = {
	generated: new Date().toISOString().slice(0, 10),
	sourceFile: 'src/items.js',
	types,
	aliases,
	opaque: [...new Set(opaque)].filter((name) => types[name] === undefined && aliases[name] === undefined),
};
fs.writeFileSync(path.join(libDir, 'engine-index.json'), JSON.stringify(index));
console.log(`engine index: ${Object.keys(types).length} types, ${Object.keys(aliases).length} aliases, ${index.opaque.length} opaque`);
