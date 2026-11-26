// The schema of the composer: a document (`.jqschema`, JSON) that describes a tree of components, not a QML file.
//
//   {
//     "version": 1,
//     "imports": ["QtQuick 2.12", "com.imtcore.imtqml 1.0"],   // the modules the generated QML imports
//     "extends": "../Base.jqschema",                          // optional: the schema this one inherits
//     "root": { "uid", "type", "id", "properties": { name: "QML expression" }, "children": [ node ] },
//     "exports": [ { "name": "title", "uid": "<uid of a node of this schema>", "property": "text", "type": "string" } ]
//   }
//
// node = { uid, type, source, id, properties, children }. `type` is a type of the engine or of an imported module;
// `source` (instead of `type`) is a schema that is used as a component.
//
// Inheritance. A schema that extends another one is the base schema with something added: its root keeps the base
// tree as it is and may only set the *exported* properties of the base (and its bases), and add objects to the end of
// its children. The same holds for a node that is an instance of a schema (`source`): only the exported properties
// can be set (and the placement ones: x, y, size, anchors, ... see isPlacement), the inside of the schema is closed.
// An export is a name for a property of one node of the schema; it becomes `property alias` of the generated QML.
//
// The functions are pure: they take a schema (a plain object) and return a changed copy. What is read from other
// files comes through `resolve(ref, fromKey)` -> { key, schema } | null (key: the identity of the file).

import crypto from 'node:crypto';

const PLACEMENT = new Set(['x', 'y', 'z', 'width', 'height', 'visible', 'opacity', 'rotation', 'scale', 'clip', 'enabled']);

// The properties that place an object in its parent: they are free for the objects that are instances of a schema.
export function isPlacement(name) {
	return PLACEMENT.has(name) || name.startsWith('anchors.') || name.startsWith('Layout.');
}

export const isIdent = (value) => /^[A-Za-z_][A-Za-z0-9_]*$/.test(String(value || ''));

const newUid = () => crypto.randomBytes(6).toString('hex');

// ----- the document -----

export function emptySchema() {
	return { version: 1, imports: ['QtQuick 2.12'], extends: '', root: { uid: newUid(), type: 'Item', source: '', id: 'root', properties: { width: '640', height: '480' }, children: [] }, exports: [] };
}

function normalizeNode(node, asRoot) {
	if (node === null || typeof node !== 'object') return null;
	const source = typeof node.source === 'string' ? node.source : '';
	const type = source ? '' : String(node.type || '');
	if (!source && !type && !asRoot) return null;
	const properties = {};
	if (node.properties && typeof node.properties === 'object') {
		for (const name of Object.keys(node.properties)) {
			const value = node.properties[name];
			if (typeof value === 'string' || typeof value === 'number' || typeof value === 'boolean') properties[name] = String(value);
		}
	}
	const children = Array.isArray(node.children) ? node.children.map((child) => normalizeNode(child, false)).filter(Boolean) : [];
	return { uid: node.uid ? String(node.uid) : newUid(), type, source, id: typeof node.id === 'string' ? node.id : '', properties, children };
}

export function normalizeSchema(value) {
	const input = value !== null && typeof value === 'object' ? value : {};
	const extendsRef = typeof input.extends === 'string' ? input.extends : '';
	let root = normalizeNode(input.root, true);
	if (root === null) root = emptySchema().root;
	if (extendsRef) {
		root.type = '';
		root.source = '';
	} else if (!root.type && !root.source) {
		root.type = 'Item';
	}
	const exports = Array.isArray(input.exports)
		? input.exports.filter((item) => item && typeof item.name === 'string' && typeof item.uid === 'string' && typeof item.property === 'string').map((item) => ({ name: item.name, uid: item.uid, property: item.property, type: typeof item.type === 'string' ? item.type : '' }))
		: [];
	const imports = Array.isArray(input.imports) ? input.imports.filter((item) => typeof item === 'string' && item.trim() !== '') : ['QtQuick 2.12'];
	return { version: 1, imports, extends: extendsRef, root, exports };
}

export function parseSchema(text) {
	if (String(text || '').trim() === '') return { schema: emptySchema(), error: '' };
	try {
		return { schema: normalizeSchema(JSON.parse(text)), error: '' };
	} catch (error) {
		return { schema: emptySchema(), error: error.message };
	}
}

export const stringifySchema = (schema) => JSON.stringify(normalizeSchema(schema), null, 2) + '\n';

const clone = (schema) => JSON.parse(JSON.stringify(schema));

// ----- reading the tree -----

export function findNode(schema, uid) {
	let found = null;
	const visit = (node, parent) => {
		if (found !== null) return;
		if (node.uid === uid) found = { node, parent };
		else for (const child of node.children) visit(child, node);
	};
	visit(schema.root, null);
	return found;
}

export function walkNodes(schema, visit) {
	const go = (node) => {
		visit(node);
		node.children.forEach(go);
	};
	go(schema.root);
}

// The name of the QML type that a file of a schema gets: the file name without the extension, upper-cased.
export function typeNameOfKey(key) {
	const base = String(key).split(/[/\\]/).pop().replace(/\.jqschema$/i, '').replace(/[^A-Za-z0-9_]/g, '');
	if (base === '' || !/^[A-Za-z_]/.test(base)) return 'Schema';
	return base[0].toUpperCase() + base.slice(1);
}

// The schemas that `entry` extends, the nearest first. Throws on a cycle or a missing file.
export function basesOf(entry, resolve) {
	const bases = [];
	const seen = new Set([entry.key]);
	let current = entry;
	while (current.schema.extends) {
		const next = resolve(current.schema.extends, current.key);
		if (next === null) throw new Error(`The schema ${current.schema.extends} that ${typeNameOfKey(current.key)} extends is not found`);
		if (seen.has(next.key)) throw new Error(`The schemas extend each other in a cycle at ${typeNameOfKey(next.key)}`);
		seen.add(next.key);
		bases.push(next);
		current = next;
	}
	return bases;
}

// All exports of a schema: the ones of its bases first, then its own. Each has `owner` (the key of the schema).
export function exportsOf(entry, resolve) {
	const list = [];
	for (const base of [...basesOf(entry, resolve)].reverse()) for (const item of base.schema.exports) list.push({ ...item, owner: base.key, inherited: true });
	for (const item of entry.schema.exports) list.push({ ...item, owner: entry.key, inherited: false });
	return list;
}

// The type of the engine at the bottom of the chain of extends.
export function rootTypeOf(entry, resolve) {
	const chain = [entry, ...basesOf(entry, resolve)];
	const last = chain[chain.length - 1].schema.root;
	if (last.source) {
		const inner = resolve(last.source, chain[chain.length - 1].key);
		return inner === null ? '' : rootTypeOf(inner, resolve);
	}
	return last.type;
}

// What the properties of a node may be: null (any), or the set of names that the schema it uses exports.
function allowedFor(schema, ctx, node) {
	const isDerivedRoot = node === schema.root && schema.extends !== '';
	if (!node.source && !isDerivedRoot) return null;
	const entry = resolveUsed(schema, ctx, node);
	return new Set(exportsOf(entry, ctx.resolve).map((item) => item.name));
}

function resolveUsed(schema, ctx, node) {
	const ref = node === schema.root && schema.extends !== '' ? schema.extends : node.source;
	const found = ctx.resolve(ref, ctx.key);
	if (found === null) throw new Error(`The schema ${ref} is not found`);
	return found;
}

// ----- the edits -----

function nodeOrThrow(schema, uid) {
	const found = findNode(schema, uid);
	if (found === null) throw new Error('No such object');
	return found;
}

export function setProperty(schema, ctx, uid, name, value) {
	const next = clone(schema);
	const { node } = nodeOrThrow(next, uid);
	const allowed = allowedFor(next, ctx, node);
	if (allowed !== null && !allowed.has(name) && !(node.source && isPlacement(name)) && !(node === next.root && isPlacement(name))) {
		throw new Error(`${name} is not exported by the schema: only the exported properties can be set`);
	}
	const text = String(value).trim();
	if (text === '') delete node.properties[name];
	else node.properties[name] = text;
	return next;
}

export function setId(schema, uid, id) {
	const next = clone(schema);
	const { node } = nodeOrThrow(next, uid);
	if (id !== '' && !isIdent(id)) throw new Error(`${id} is not a valid id`);
	let taken = false;
	walkNodes(next, (item) => {
		if (item !== node && item.id === id && id !== '') taken = true;
	});
	if (taken) throw new Error(`The id ${id} is used by another object`);
	node.id = id;
	return next;
}

const stemOf = (name) => {
	const short = String(name).split('.').pop();
	return short.charAt(0).toLowerCase() + short.slice(1);
};

function uniqueId(schema, stem) {
	const taken = new Set();
	walkNodes(schema, (node) => node.id !== '' && taken.add(node.id));
	let index = 1;
	while (taken.has(stem + index)) index++;
	return stem + index;
}

// Adds an object: `spec` is { type } or { source }. Returns { schema, uid }.
export function addNode(schema, ctx, parentUid, spec, props = {}, index = undefined) {
	const next = clone(schema);
	const { node: parent } = nodeOrThrow(next, parentUid);
	if (parent.source) throw new Error('An instance of a schema is closed: objects cannot be added into it');
	if (!spec.type && !spec.source) throw new Error('The type of the object is not set');
	if (spec.source && spec.source === ctx.key) throw new Error('A schema cannot use itself');
	const stem = spec.source ? stemOf(typeNameOfKey(spec.source)) : stemOf(spec.type);
	const node = { uid: newUid(), type: spec.source ? '' : spec.type, source: spec.source || '', id: uniqueId(next, stem), properties: {}, children: [] };
	for (const [name, value] of Object.entries(props)) if (String(value).trim() !== '') node.properties[name] = String(value).trim();
	const at = index === undefined || index > parent.children.length ? parent.children.length : Math.max(0, index);
	parent.children.splice(at, 0, node);
	return { schema: next, uid: node.uid };
}

const contains = (node, uid) => node.uid === uid || node.children.some((child) => contains(child, uid));

export function deleteNode(schema, uid) {
	const next = clone(schema);
	if (uid === next.root.uid) throw new Error('The root object cannot be deleted');
	const { node, parent } = nodeOrThrow(next, uid);
	parent.children.splice(parent.children.indexOf(node), 1);
	// the exports of the objects that are gone go too
	const gone = new Set();
	const collect = (item) => {
		gone.add(item.uid);
		item.children.forEach(collect);
	};
	collect(node);
	next.exports = next.exports.filter((item) => !gone.has(item.uid));
	return next;
}

export function moveNode(schema, uid, parentUid, index = undefined) {
	const next = clone(schema);
	if (uid === next.root.uid) throw new Error('The root object cannot be moved');
	const { node, parent } = nodeOrThrow(next, uid);
	const { node: target } = nodeOrThrow(next, parentUid);
	if (contains(node, parentUid)) throw new Error('An object cannot be moved into itself');
	if (target.source) throw new Error('An instance of a schema is closed: objects cannot be moved into it');
	let at = index;
	const from = parent.children.indexOf(node);
	parent.children.splice(from, 1);
	if (at !== undefined && parent === target && from < at) at--;
	const place = at === undefined || at > target.children.length ? target.children.length : Math.max(0, at);
	target.children.splice(place, 0, node);
	return next;
}

export function setImports(schema, imports) {
	const next = clone(schema);
	next.imports = imports.map((item) => String(item).trim()).filter(Boolean);
	return next;
}

// Sets the schema that is extended ('' for none). The properties of the root that the new base does not export
// are removed: the list of them is returned in `dropped`.
export function setExtends(schema, ctx, ref) {
	const next = clone(schema);
	const dropped = [];
	if (ref === '') {
		next.extends = '';
		if (!next.root.type) next.root.type = 'Item';
		return { schema: next, dropped };
	}
	const entry = ctx.resolve(ref, ctx.key);
	if (entry === null) throw new Error(`The schema ${ref} is not found`);
	if (entry.key === ctx.key) throw new Error('A schema cannot extend itself');
	next.extends = ref;
	basesOf({ key: ctx.key, schema: next }, ctx.resolve);
	next.root.type = '';
	next.root.source = '';
	const allowed = new Set(exportsOf(entry, ctx.resolve).map((item) => item.name));
	for (const name of Object.keys(next.root.properties)) {
		if (!allowed.has(name) && !isPlacement(name)) {
			dropped.push(name);
			delete next.root.properties[name];
		}
	}
	return { schema: next, dropped };
}

const exportNameOf = (property) => property.replace(/[^A-Za-z0-9_]/g, '_').replace(/^(\d)/, '_$1');

// Exports a property of an object of this schema under `name` (the property's own name by default).
export function addExport(schema, ctx, uid, property, name = undefined, typeName = '') {
	const next = clone(schema);
	const { node } = nodeOrThrow(next, uid);
	if (node === next.root && next.extends) throw new Error('The root of an inheriting schema has no properties of its own to export');
	const taken = new Set(exportsOf({ key: ctx.key, schema: next }, ctx.resolve).map((item) => item.name));
	let exportName = name === undefined || name === '' ? exportNameOf(property) : name;
	if (!isIdent(exportName)) throw new Error(`${exportName} is not a valid name`);
	if (name === undefined || name === '') {
		const stem = node.id !== '' && taken.has(exportName) ? `${node.id}_${exportName}` : exportName;
		exportName = stem;
		let index = 2;
		while (taken.has(exportName)) exportName = `${stem}${index++}`;
	} else if (taken.has(exportName)) {
		throw new Error(`The name ${exportName} is already exported`);
	}
	if (next.exports.some((item) => item.uid === uid && item.property === property)) throw new Error(`${property} is already exported`);
	next.exports.push({ name: exportName, uid, property, type: typeName });
	return next;
}

export function removeExport(schema, name) {
	const next = clone(schema);
	next.exports = next.exports.filter((item) => item.name !== name);
	return next;
}

export function renameExport(schema, ctx, name, to) {
	const next = clone(schema);
	const item = next.exports.find((entry) => entry.name === name);
	if (item === undefined) throw new Error('No such export');
	if (!isIdent(to)) throw new Error(`${to} is not a valid name`);
	const taken = exportsOf({ key: ctx.key, schema: next }, ctx.resolve).filter((entry) => entry !== item && entry.name === to);
	if (taken.length > 0) throw new Error(`The name ${to} is already exported`);
	item.name = to;
	return next;
}

// ----- the view for the editor -----
//
// view = { uid, type, id, origin: { key, uid }, props: [{ name, value }], children, editable, instanceOf }
// The objects of the bases and the insides of instances are in the view too (editable: false); an instance and the
// objects of the schema itself are editable. The values that an inheriting schema or an instance sets on exports are
// applied to the objects they belong to, so the canvas shows what is going to be generated.

const ownOf = (properties) => Object.entries(properties).map(([name, value]) => ({ name, value }));

// The exports of a schema that an instance or an inheriting schema can set.
function acceptsOf(entry, resolve) {
	try {
		return exportsOf(entry, resolve).map((item) => ({ name: item.name, type: item.type, property: item.property }));
	} catch (error) {
		return [];
	}
}

function viewOfNode(node, entry, resolve, editable, prefix) {
	if (node.source) {
		const used = resolve(node.source, entry.key);
		if (used === null) return { uid: prefix + node.uid, type: typeNameOfKey(node.source), id: node.id, origin: { key: entry.key, uid: node.uid }, props: [], children: [], editable, missing: node.source, instanceOf: typeNameOfKey(node.source), source: node.source };
		const view = viewOfInstance(used, Object.entries(node.properties), resolve, prefix + node.uid + '/');
		return { ...view, uid: prefix + node.uid, id: node.id, origin: { key: entry.key, uid: node.uid }, editable, instanceOf: typeNameOfKey(node.source), source: node.source, derived: undefined, own: ownOf(node.properties), accepts: acceptsOf(used, resolve) };
	}
	return {
		uid: prefix + node.uid,
		type: node.type,
		id: node.id,
		origin: { key: entry.key, uid: node.uid },
		props: Object.entries(node.properties).map(([name, value]) => ({ name, value })),
		own: ownOf(node.properties),
		children: node.children.map((child) => viewOfNode(child, entry, resolve, editable, prefix)),
		editable,
	};
}

// The root of a schema with its bases, without the values that an instance sets.
function viewOfSchema(entry, resolve, editable, prefix) {
	const schema = entry.schema;
	if (schema.extends === '') return viewOfNode(schema.root, entry, resolve, editable, prefix);
	const base = resolve(schema.extends, entry.key);
	if (base === null) return { uid: prefix + schema.root.uid, type: typeNameOfKey(schema.extends), id: schema.root.id, origin: { key: entry.key, uid: schema.root.uid }, props: [], children: [], editable, missing: schema.extends };
	const view = viewOfInstance(base, Object.entries(schema.root.properties), resolve, prefix + 'base/');
	view.alsoOrigins = [...(view.alsoOrigins || []), view.origin];
	view.uid = prefix + schema.root.uid;
	view.id = schema.root.id;
	view.origin = { key: entry.key, uid: schema.root.uid };
	view.editable = editable;
	view.derived = typeNameOfKey(schema.extends);
	view.source = schema.extends;
	view.own = ownOf(schema.root.properties);
	view.accepts = acceptsOf(base, resolve);
	for (const child of schema.root.children) view.children.push(viewOfNode(child, entry, resolve, editable, prefix));
	return view;
}

function viewOfInstance(entry, values, resolve, prefix) {
	const view = viewOfSchema(entry, resolve, false, prefix);
	applyValues(view, entry, values, resolve);
	return view;
}

function findOrigin(view, key, uid) {
	if ((view.origin.key === key && view.origin.uid === uid) || (view.alsoOrigins || []).some((item) => item.key === key && item.uid === uid)) return view;
	for (const child of view.children) {
		const found = findOrigin(child, key, uid);
		if (found !== null) return found;
	}
	return null;
}

function setViewProp(view, name, value) {
	const found = view.props.find((prop) => prop.name === name);
	if (found !== undefined) found.value = value;
	else view.props.push({ name, value });
}

// Sets the values of the exports (and of the placement properties) on the objects they belong to.
function applyValues(view, entry, values, resolve) {
	let exportList = [];
	try {
		exportList = exportsOf(entry, resolve);
	} catch (error) {
		exportList = [];
	}
	for (const [name, value] of values) {
		const exported = exportList.find((item) => item.name === name);
		if (exported === undefined) {
			setViewProp(view, name, value);
			continue;
		}
		const target = findOrigin(view, exported.owner, exported.uid);
		if (target === null) continue;
		setViewProp(target, exported.property, value);
	}
}

// The view of a schema (the root of the tree that the editor shows).
export function viewOf(key, schema, resolve) {
	return viewOfSchema({ key, schema }, resolve, true, '');
}

// A plain description of the exports of a schema for the editor.
export function describeExports(key, schema, resolve) {
	return exportsOf({ key, schema }, resolve).map((item) => ({ name: item.name, uid: item.uid, property: item.property, type: item.type, inherited: item.inherited }));
}
