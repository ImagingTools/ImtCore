// The parsed tree of a QML document as a compact value (arrays by position, flags in one number), the form in which a compiled
// application carries its documents instead of their text (compiler/compiler.js). `packDocument` runs in the compiler,
// `unpackDocument` in the browser; unpackDocument(JSON.parse(JSON.stringify(packDocument(def)))) is the tree that
// parseQmlDocument gave. The tree has the shape of src/parser.js; a field that the format does not know is an error of the
// compiler, not a field that goes missing in the browser (test/packed.mjs checks every document it can find).
//
//   object   [type, id, propertyName, line, col, properties, functions, signalDefs, imports, enums, children, modules?, extras?]
//   property [name, flags, text, line, col, typeName?]
//   function [name, params, body, line, col]
//   signal   [name, params]
//   import   [path, alias, line]
//   module   [name, alias, version (null or [major, minor]), line]
//   enum     [name, values]
// Trailing empty lists of an object are left out; `modules` (0 when there is none) is there only for the root of the document or when
// the object has `extras`, the fields `onProperty` (`Behavior on x { }`) and `inList` (an object in a list) that only some objects have.

const DECLARATION = 1;
const ALIAS = 2;
const HANDLER = 4;
const BLOCK = 8;
const READONLY = 16;
const DEFAULT = 32;
const RETURN_BLOCK = 64;

function fail(what, key) {
	throw new Error(`The packed format of documents does not know '${key}' of ${what}: add it to src/packed.js`);
}

function expectKeys(value, allowed, what) {
	for (const key of Object.keys(value)) if (!allowed.includes(key)) fail(what, key);
}

function flag(value, key, what) {
	if (typeof value !== 'boolean') throw new Error(`'${key}' of ${what} is not a boolean`);
	return value;
}

const OBJECT_KEYS = ['type', 'id', 'propertyName', 'properties', 'functions', 'signalDefs', 'imports', 'enums', 'children', 'line', 'col', 'modules', 'onProperty', 'inList'];
const PROPERTY_KEYS = ['name', 'isDeclaration', 'isAlias', 'isHandler', 'isBlock', 'text', 'line', 'col', 'typeName', 'isReadonly', 'isDefault', 'isReturnBlock'];

function packProperty(prop, script) {
	expectKeys(prop, PROPERTY_KEYS, 'a property');
	if (prop.text !== null && typeof prop.text !== 'string') throw new Error(`the text of the property '${prop.name}' is not a string`);
	let flags = 0;
	if (flag(prop.isDeclaration, 'isDeclaration', 'a property')) flags |= DECLARATION;
	if (flag(prop.isAlias, 'isAlias', 'a property')) flags |= ALIAS;
	if (flag(prop.isHandler, 'isHandler', 'a property')) flags |= HANDLER;
	if (flag(prop.isBlock, 'isBlock', 'a property')) flags |= BLOCK;
	if (prop.isReturnBlock !== undefined) {
		if (prop.isReturnBlock !== true) throw new Error('isReturnBlock of a property is not true');
		flags |= RETURN_BLOCK;
	}
	const text = prop.text === null ? null : script(prop.text);
	if (prop.isDeclaration) {
		if (flag(prop.isReadonly, 'isReadonly', 'a property')) flags |= READONLY;
		if (flag(prop.isDefault, 'isDefault', 'a property')) flags |= DEFAULT;
		return [prop.name, flags, text, prop.line, prop.col, prop.typeName];
	}
	if (prop.typeName !== undefined || prop.isReadonly !== undefined || prop.isDefault !== undefined) throw new Error('a property that is not a declaration has the fields of one');
	return [prop.name, flags, text, prop.line, prop.col];
}

function unpackProperty(p) {
	const flags = p[1];
	const prop = {
		name: p[0],
		isDeclaration: (flags & DECLARATION) !== 0,
		isAlias: (flags & ALIAS) !== 0,
		isHandler: (flags & HANDLER) !== 0,
		isBlock: (flags & BLOCK) !== 0,
		text: p[2],
		line: p[3],
		col: p[4],
	};
	if (prop.isDeclaration) {
		prop.typeName = p[5];
		prop.isReadonly = (flags & READONLY) !== 0;
		prop.isDefault = (flags & DEFAULT) !== 0;
	}
	if ((flags & RETURN_BLOCK) !== 0) prop.isReturnBlock = true;
	return prop;
}

function packFunction(fn, script) {
	expectKeys(fn, ['name', 'params', 'body', 'line', 'col'], 'a function');
	return [fn.name, fn.params, script(fn.body), fn.line, fn.col];
}

function unpackFunction(f) {
	return { name: f[0], params: f[1], body: f[2], line: f[3], col: f[4] };
}

function packSignal(signal) {
	expectKeys(signal, ['name', 'params'], 'a signal');
	return [signal.name, signal.params];
}

function packImport(entry) {
	expectKeys(entry, ['path', 'alias', 'line'], 'an import');
	return [entry.path, entry.alias, entry.line];
}

function packModule(entry) {
	expectKeys(entry, ['name', 'alias', 'version', 'line'], 'a module import');
	let version = null;
	if (entry.version !== null) {
		expectKeys(entry.version, ['major', 'minor'], 'a module version');
		version = [entry.version.major, entry.version.minor];
	}
	return [entry.name, entry.alias, version, entry.line];
}

function packEnum(entry) {
	expectKeys(entry, ['name', 'values'], 'an enum');
	return [entry.name, entry.values];
}

// `script` turns the text of a script fragment into the text that is carried (the compiler makes it smaller); it must keep the code.
export function packDocument(def, script = (text) => text) {
	expectKeys(def, OBJECT_KEYS, 'an object');
	const packed = [
		def.type,
		def.id,
		def.propertyName,
		def.line,
		def.col,
		def.properties.map((prop) => packProperty(prop, script)),
		def.functions.map((fn) => packFunction(fn, script)),
		def.signalDefs.map(packSignal),
		def.imports.map(packImport),
		def.enums.map(packEnum),
		def.children.map((child) => {
			if (child.modules !== undefined) throw new Error('only the root of a document has modules');
			return packDocument(child, script);
		}),
	];
	const extras = {};
	if (def.onProperty !== undefined) {
		if (typeof def.onProperty !== 'string') throw new Error('onProperty of an object is not a string');
		extras.onProperty = def.onProperty;
	}
	if (def.inList !== undefined) {
		if (def.inList !== true) throw new Error('inList of an object is not true');
		extras.inList = true;
	}
	if (def.modules !== undefined) packed.push(def.modules.map(packModule));
	else while (packed.length > 5 && Array.isArray(packed[packed.length - 1]) && packed[packed.length - 1].length === 0) packed.pop();
	if (Object.keys(extras).length > 0) {
		while (packed.length < 11) packed.push([]);
		if (packed.length === 11) packed.push(0);
		packed.push(extras);
	}
	return packed;
}

export function unpackDocument(p) {
	const def = {
		type: p[0],
		id: p[1],
		propertyName: p[2],
		properties: (p[5] || []).map(unpackProperty),
		functions: (p[6] || []).map(unpackFunction),
		signalDefs: (p[7] || []).map((s) => ({ name: s[0], params: s[1] })),
		imports: (p[8] || []).map((i) => ({ path: i[0], alias: i[1], line: i[2] })),
		enums: (p[9] || []).map((e) => ({ name: e[0], values: e[1] })),
		children: (p[10] || []).map(unpackDocument),
		line: p[3],
		col: p[4],
	};
	if (p.length > 11 && p[11] !== 0) def.modules = p[11].map((m) => ({ name: m[0], alias: m[1], version: m[2] === null ? null : { major: m[2][0], minor: m[2][1] }, line: m[3] }));
	if (p.length > 12) Object.assign(def, p[12]);
	return def;
}
