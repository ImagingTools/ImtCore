// Generation of QML from schemas. Every schema becomes a QML type (a file named after the schema), so a schema that
// extends another one is `Base { ... }`, a schema that is used as a component is an object of its type, and an export
// is a `property alias` of the root. The generated files stay in one folder: the engine finds the types of the folder of
// a file without an import.

import { basesOf, exportsOf, isIdent, isPlacement, normalizeSchema, typeNameOfKey } from './schema.mjs';

const DEFAULT_IMPORT = 'QtQuick 2.12';

function isHandlerName(name) {
	const last = name.split('.').pop();
	return last.length > 2 && last.startsWith('on') && /^_*[A-Z]/.test(last.slice(2));
}

// The schemas the entry needs: its bases and the schemas used as components, each once. Map key -> { key, schema }.
export function collectSchemas(entry, resolve, problems) {
	const found = new Map();
	const visit = (item) => {
		if (found.has(item.key)) return;
		found.set(item.key, item);
		let bases = [];
		try {
			bases = basesOf(item, resolve);
		} catch (error) {
			problems.push(error.message);
		}
		for (const base of bases) visit(base);
		const go = (node) => {
			if (node.source) {
				const used = resolve(node.source, item.key);
				if (used === null) problems.push(`${typeNameOfKey(item.key)}: the schema ${node.source} is not found`);
				else visit(used);
			}
			node.children.forEach(go);
		};
		go(item.schema.root);
	};
	visit(entry);
	return found;
}

// The problems of a schema: what it sets on a closed schema, the exports that point nowhere, the ids.
export function validateSchema(entry, resolve) {
	const problems = [];
	const name = typeNameOfKey(entry.key);
	const schema = entry.schema;
	const uids = new Map();
	const ids = new Set();
	const go = (node) => {
		uids.set(node.uid, node);
		if (node.id !== '') {
			if (!isIdent(node.id)) problems.push(`${name}: ${node.id} is not a valid id`);
			if (ids.has(node.id)) problems.push(`${name}: the id ${node.id} is used twice`);
			ids.add(node.id);
		}
		if (!node.source && node.type === '' && !(node === schema.root && schema.extends)) problems.push(`${name}: an object has no type`);
		if (node.source) {
			const used = resolve(node.source, entry.key);
			if (used !== null) {
				let allowed = new Set();
				try {
					allowed = new Set(exportsOf(used, resolve).map((item) => item.name));
				} catch (error) {
					problems.push(error.message);
				}
				for (const property of Object.keys(node.properties)) if (!allowed.has(property) && !isPlacement(property)) problems.push(`${name}: ${node.id || node.source}: ${property} is not exported by ${typeNameOfKey(node.source)}`);
			}
		}
		node.children.forEach(go);
	};
	go(schema.root);
	if (schema.extends) {
		const base = resolve(schema.extends, entry.key);
		if (base !== null) {
			let allowed = new Set();
			try {
				allowed = new Set(exportsOf(base, resolve).map((item) => item.name));
			} catch (error) {
				problems.push(error.message);
			}
			for (const property of Object.keys(schema.root.properties)) if (!allowed.has(property) && !isPlacement(property)) problems.push(`${name}: ${property} is not exported by ${typeNameOfKey(schema.extends)}`);
		}
	}
	const seen = new Set();
	try {
		for (const item of exportsOf(entry, resolve)) {
			if (seen.has(item.name)) problems.push(`${name}: the export ${item.name} is defined twice`);
			seen.add(item.name);
			if (item.owner === entry.key && !uids.has(item.uid)) problems.push(`${name}: the export ${item.name} points to an object that is gone`);
		}
	} catch (error) {
		problems.push(error.message);
	}
	return problems;
}

function writeValue(lines, pad, name, value) {
	const parts = String(value).trim().split(/\r?\n/);
	lines.push(`${pad}${name}: ${parts[0]}`);
	for (let i = 1; i < parts.length; i++) lines.push(pad + parts[i]);
}

function writeHandler(lines, pad, name, value) {
	const text = String(value).trim();
	if (text.startsWith('{') || /^function\b/.test(text) || /^(?:\([^)]*\)|[A-Za-z_$][\w$]*)\s*=>/.test(text)) {
		writeValue(lines, pad, name, text);
		return;
	}
	lines.push(`${pad}${name}: {`);
	for (const line of text.split(/\r?\n/)) lines.push(`${pad}    ${line}`);
	lines.push(`${pad}}`);
}

function assignIds(schema) {
	const taken = new Set();
	const visit = (node) => {
		if (node.id !== '') taken.add(node.id);
		node.children.forEach(visit);
	};
	visit(schema.root);
	const ids = new Map();
	const give = (node, isRoot) => {
		let id = node.id;
		if (id === '') {
			const stem = isRoot ? 'root' : (node.type || typeNameOfKey(node.source)).split('.').pop().replace(/^./, (c) => c.toLowerCase());
			let index = isRoot ? 0 : 1;
			id = isRoot ? 'root' : stem + index;
			while (taken.has(id)) id = stem + ++index;
			taken.add(id);
		}
		ids.set(node.uid, id);
		node.children.forEach((child) => give(child, false));
	};
	give(schema.root, true);
	return ids;
}

function writeNode(node, ctx, lines, depth) {
	const pad = '    '.repeat(depth);
	const inner = '    '.repeat(depth + 1);
	let typeName = node.type;
	if (node.source) {
		const used = ctx.resolve(node.source, ctx.key);
		typeName = typeNameOfKey(used === null ? node.source : used.key);
	} else if (node === ctx.schema.root && ctx.schema.extends) {
		const base = ctx.resolve(ctx.schema.extends, ctx.key);
		typeName = typeNameOfKey(base === null ? ctx.schema.extends : base.key);
	}
	lines.push(`${pad}${typeName} {`);
	lines.push(`${inner}id: ${ctx.ids.get(node.uid)}`);
	if (node === ctx.schema.root) {
		for (const item of ctx.schema.exports) {
			const target = ctx.ids.get(item.uid);
			if (target !== undefined) lines.push(`${inner}property alias ${item.name}: ${target}.${item.property}`);
		}
	}
	const entries = Object.entries(node.properties).filter(([, value]) => String(value).trim() !== '');
	for (const [name, value] of entries.filter(([name]) => !isHandlerName(name))) writeValue(lines, inner, name, value);
	for (const [name, value] of entries.filter(([name]) => isHandlerName(name))) writeHandler(lines, inner, name, value);
	if (node.children.length > 0 && (entries.length > 0 || node === ctx.schema.root)) lines.push('');
	node.children.forEach((child, index) => {
		writeNode(child, ctx, lines, depth + 1);
		if (index < node.children.length - 1) lines.push('');
	});
	lines.push(`${pad}}`);
}

// The QML text of one schema.
export function writeSchema(entry, resolve) {
	const schema = normalizeSchema(entry.schema);
	const ctx = { key: entry.key, schema, resolve, ids: assignIds(schema) };
	const lines = [`// Generated from ${entry.key.split(/[/\\]/).pop()} by the JQML 4 composer. Do not edit: change the schema.`];
	const imports = schema.imports.length > 0 ? schema.imports : [DEFAULT_IMPORT];
	for (const item of imports) lines.push(`import ${item}`);
	lines.push('');
	writeNode(schema.root, ctx, lines, 0);
	lines.push('');
	return lines.join('\n');
}

// All the files of the application that starts from `entry`: { files: Map name -> text, entryName }.
// Throws one error with every problem that was found.
export function generate(entry, resolve) {
	const problems = [];
	const schemas = collectSchemas(entry, resolve, problems);
	const names = new Map();
	for (const item of schemas.values()) {
		const name = typeNameOfKey(item.key);
		if (names.has(name) && names.get(name) !== item.key) problems.push(`Two schemas give the type ${name}: ${names.get(name)} and ${item.key}`);
		names.set(name, item.key);
		problems.push(...validateSchema(item, resolve));
	}
	if (problems.length > 0) throw new Error(problems.join('\n'));
	const files = new Map();
	for (const item of schemas.values()) files.set(`${typeNameOfKey(item.key)}.qml`, writeSchema(item, resolve));
	return { files, entryName: `${typeNameOfKey(entry.key)}.qml` };
}
