// Parser of the QML document structure. Objects, properties, signals and functions become plain
// data; JavaScript fragments are kept as source text (checked for syntax here) and run later by
// the engine.
//
// Object definition: { type, id, propertyName, properties, functions, signalDefs, imports, children, line, col }
// Property definition: { name, isDeclaration, isAlias, isHandler, isBlock, text, line, col }
// Function definition: { name, params (text), body (text), line, col }

import { QmlError, continuesAcrossNewline, escapeSourceBreaks, tokenize } from './lexer.js';

// An arrow or a function expression used as the whole signal handler.
export function callableHandler(body) {
	const text = String(body).trim();
	return /^function\b/.test(text) || /^(?:\([^)]*\)|[A-Za-z_$][\w$]*)\s*=>/.test(text);
}

function isHandlerName(dotted) {
	const last = dotted.slice(dotted.lastIndexOf('.') + 1);
	// `onFoo`, and `on_Foo` for the handler of a property named `_foo`.
	return last.length > 2 && last.startsWith('on') && /^_*[A-Z]/.test(last.slice(2));
}

function isUpper(text) {
	return text.length > 0 && text[0] >= 'A' && text[0] <= 'Z';
}

// Whether the script fragments are checked while a document is parsed (see parseQmlDocument).
let checking = true;

// Syntax check of a script fragment. `kind`: 'expr' (an expression), 'body' (statements).
function checkSyntax(kind, text, line, col, params = '') {
	if (!checking) return;
	try {
		const js = escapeSourceBreaks(text);
		if (kind === 'expr') {
			new Function(`return (${js}\n)`);
		} else {
			new Function(params, js);
		}
	} catch (e) {
		if (e instanceof SyntaxError) {
			throw new QmlError(e.message, line, col);
		}
		throw e;
	}
}

class Parser {
	constructor(source) {
		this.source = source;
		this.tokens = tokenize(source);
		this.pos = 0;
	}

	peek(offset = 0) {
		const index = this.pos + offset;
		return this.tokens[index < this.tokens.length ? index : this.tokens.length - 1];
	}

	next() {
		const token = this.peek();
		if (this.pos < this.tokens.length - 1) this.pos++;
		return token;
	}

	isP(text, offset = 0) {
		const t = this.peek(offset);
		return t.t === 'p' && t.v === text;
	}

	isId(text, offset = 0) {
		const t = this.peek(offset);
		return t.t === 'id' && t.v === text;
	}

	fail(message, token) {
		throw new QmlError(message, token.line, token.col);
	}

	expectP(text) {
		if (!this.isP(text)) {
			this.fail(`Expected '${text}'`, this.peek());
		}
		return this.next();
	}

	expectId(message) {
		if (this.peek().t !== 'id') this.fail(message, this.peek());
		return this.next().v;
	}

	skipSemicolon() {
		if (this.isP(';')) this.next();
	}

	parseDocument() {
		// `import "folder" [as Alias]` makes the .qml files of a folder available as types, and
		// `import "file.js" as Name` a JavaScript library. `import Module 1.0 [as Alias]` is recorded in
		// `modules` (resolved through qmldir files by the project). Pragmas are skipped.
		const imports = [];
		const modules = [];
		while (this.isId('import') || this.isId('pragma')) {
			const isImport = this.isId('import');
			const line = this.next().line;
			if (isImport && this.peek().t === 'str') {
				const path = this.next().v.slice(1, -1);
				let alias = '';
				if (this.isId('as')) {
					this.next();
					alias = this.expectId('Expected an import alias');
				}
				imports.push({ path, alias, line });
			} else if (isImport && this.peek().t === 'id') {
				const name = this.parseDottedName('Expected a module name');
				let alias = '';
				let version = null;
				while (this.peek().t !== 'eof' && !this.peek().nl) {
					if (this.isId('as') && this.peek(1).t === 'id') {
						this.next();
						alias = this.next().v;
					} else if (this.peek().t === 'num' && version === null) {
						// `import Module 2.1`: the types of the version 2.1 and the lower ones of the major version 2.
						const parts = this.next().v.split('.');
						version = { major: Number(parts[0]), minor: Number(parts[1] || 0) };
					} else {
						this.next();
					}
				}
				modules.push({ name, alias, version, line });
			}
			while (this.peek().t !== 'eof' && !this.peek().nl) this.next();
		}
		const typeToken = this.peek();
		const root = this.parseObject(this.parseDottedName('Expected root object type'), typeToken);
		root.imports = imports;
		root.modules = modules;
		if (this.peek().t !== 'eof') {
			this.fail('Unexpected content after the root object', this.peek());
		}
		return root;
	}

	parseDottedName(message = 'Expected a name') {
		let name = this.expectId(message);
		while (this.isP('.') && this.peek(1).t === 'id') {
			this.next();
			name += '.' + this.next().v;
		}
		return name;
	}

	parseObject(type, typeToken) {
		const def = {
			type,
			id: '',
			propertyName: '',
			properties: [],
			functions: [],
			signalDefs: [],
			imports: [],
			enums: [],
			children: [],
			line: typeToken.line,
			col: typeToken.col,
		};
		this.expectP('{');
		while (!this.isP('}')) {
			if (this.peek().t === 'eof') {
				this.fail("Unexpected end of file, expected '}'", this.peek());
			}
			if (this.isP(';')) {
				this.next();
				continue;
			}
			this.parseMember(def);
		}
		this.next();
		return def;
	}

	parseMember(def) {
		const first = this.peek();
		if (first.t !== 'id') {
			this.fail(`Unexpected token '${first.v}'`, first);
		}

		let isDefault = false;
		let isReadonly = false;
		while (
			(this.isId('readonly') || this.isId('default') || this.isId('required')) &&
			this.peek(1).t === 'id' &&
			['property', 'readonly', 'required', 'default'].includes(this.peek(1).v)
		) {
			if (this.isId('default')) isDefault = true;
			if (this.isId('readonly')) isReadonly = true;
			this.next();
		}

		// `enum Name { A, B = 2 }`
		if (this.isId('enum') && this.peek(1).t === 'id' && this.isP('{', 2)) {
			this.next();
			const name = this.next().v;
			const block = this.captureBlock();
			const values = {};
			let counter = 0;
			for (const part of block.text.replace(/\/\/.*$/gm, '').split(',')) {
				const match = /^\s*([A-Za-z_]\w*)\s*(?:=\s*(-?\w+))?\s*$/.exec(part);
				if (match === null) continue;
				counter = match[2] !== undefined && !Number.isNaN(Number(match[2])) ? Number(match[2]) : counter;
				values[match[1]] = counter++;
			}
			def.enums.push({ name, values });
			return;
		}

		if (this.isId('signal') && this.peek(1).t === 'id') {
			this.next();
			const signal = { name: this.next().v, params: [] };
			if (this.isP('(')) {
				this.next();
				while (!this.isP(')')) {
					// `name`, `type name`, or `name: type`.
					if (this.peek().t !== 'id') this.fail('Expected a signal parameter', this.peek());
					let name = this.next().v;
					if (this.isP(':')) {
						this.next();
						if (this.peek().t !== 'id') this.fail('Expected a parameter type', this.peek());
						this.next();
						if (this.isP('<')) {
							let depth = 1;
							this.next();
							while (depth > 0) {
								if (this.peek().t === 'eof') this.fail("Expected '>'", this.peek());
								if (this.isP('<')) depth++;
								else if (this.isP('>')) depth--;
								this.next();
							}
						}
					} else {
						while (this.peek().t === 'id') name = this.next().v;
					}
					signal.params.push(name);
					if (this.isP(',')) this.next();
					else if (!this.isP(')')) this.fail("Expected ',' or ')'", this.peek());
				}
				this.next();
			}
			this.skipSemicolon();
			def.signalDefs.push(signal);
			return;
		}

		if (this.isId('property') && this.peek(1).t === 'id' && (this.peek(2).t === 'id' || this.isP('<', 2))) {
			this.next();
			const typeName = this.next().v;
			if (this.isP('<')) {
				while (!this.isP('>') && this.peek().t !== 'eof') this.next();
				this.next();
			}
			const prop = {
				name: '',
				isDeclaration: true,
				typeName,
				isAlias: typeName === 'alias',
				isReadonly,
				isDefault,
				isHandler: false,
				isBlock: false,
				text: null,
				line: first.line,
				col: first.col,
			};
			prop.name = this.expectId('Expected a property name');
			def.properties.push(prop);
			if (this.isP(':')) {
				this.next();
				if (this.parseObjectValue(prop.name, def, true)) return;
				this.captureValue(prop);
				this.skipSemicolon();
			}
			return;
		}

		if (this.isId('function') && this.peek(1).t === 'id') {
			this.next();
			const fn = { name: this.next().v, params: '', body: '', line: first.line, col: first.col };
			const open = this.expectP('(');
			let depth = 1;
			let close = open;
			while (depth > 0) {
				close = this.next();
				if (close.t === 'eof') this.fail("Expected ')'", close);
				if (close.t === 'p' && close.v === '(') depth++;
				else if (close.t === 'p' && close.v === ')') depth--;
			}
			fn.params = this.source.slice(open.e, close.s);
			const block = this.captureBlock();
			fn.body = block.text;
			checkSyntax('body', fn.body, block.line, block.col, fn.params);
			def.functions.push(fn);
			return;
		}

		if (this.isId('id') && this.isP(':', 1)) {
			this.next();
			this.next();
			def.id = this.expectId('Expected an id');
			this.skipSemicolon();
			return;
		}

		const name = this.parseDottedName();
		if (this.isP('{') && !isUpper(name[0])) {
			// `coordinate { latitude: 1; longitude: 2 }`: a group of properties, the same as `coordinate.latitude: 1`.
			const group = this.parseObject(name, first);
			for (const prop of group.properties) def.properties.push({ ...prop, name: `${name}.${prop.name}` });
			return;
		}
		if (this.isP('{')) {
			def.children.push(this.parseObject(name, first));
			return;
		}
		// `Behavior on x { ... }`, `NumberAnimation on y { ... }`
		if (this.isId('on') && this.peek(1).t === 'id' && this.isP('{', 2)) {
			this.next();
			const target = this.parseDottedName();
			const object = this.parseObject(name, first);
			object.onProperty = target;
			def.children.push(object);
			return;
		}
		if (!this.isP(':')) {
			this.fail(`Expected ':' or '{' after '${name}'`, this.peek());
		}
		this.next();

		// `delegate: Rectangle { ... }`, `states: [ State { ... } ]`: objects as the value of a property.
		if (this.parseObjectValue(name, def)) return;

		const prop = {
			name,
			isDeclaration: false,
			isAlias: false,
			isHandler: isHandlerName(name),
			isBlock: false,
			text: null,
			line: first.line,
			col: first.col,
		};
		this.captureValue(prop);
		this.skipSemicolon();
		def.properties.push(prop);
	}

	// `Type {` or `A.B {` at the current position.
	objectAhead() {
		return this.peek().t === 'id' && isUpper(this.peek().v) && (this.isP('{', 1) || (this.isP('.', 1) && this.dottedObjectAhead()));
	}

	// Parses `Type { ... }` or `[ Type { ... }, ... ]` as the value of the property `name`; the objects
	// become children with `propertyName` set. Returns false (consuming nothing) for other values.
	parseObjectValue(name, def, declared = false) {
		if (this.objectAhead()) {
			const typeToken = this.peek();
			const value = this.parseObject(this.parseDottedName(), typeToken);
			value.propertyName = name;
			def.children.push(value);
			this.skipSemicolon();
			return true;
		}
		if (this.isP('[')) {
			this.next();
			const empty = this.isP(']');
			if ((!empty || declared) && !this.objectAhead()) {
				this.pos--;
				return false;
			}
			while (!this.isP(']')) {
				if (!this.objectAhead()) this.fail('Expected an object in the list', this.peek());
				const typeToken = this.peek();
				const value = this.parseObject(this.parseDottedName(), typeToken);
				value.propertyName = name;
				value.inList = true;
				def.children.push(value);
				if (this.isP(',')) this.next();
			}
			this.next();
			this.skipSemicolon();
			return true;
		}
		return false;
	}

	// `A.B {` : a dotted type name followed by an object body.
	dottedObjectAhead() {
		let k = 0;
		while (this.peek(k).t === 'id' && this.isP('.', k + 1)) k += 2;
		return this.peek(k).t === 'id' && this.isP('{', k + 1);
	}

	// The value of a property: a block (handlers only) or an expression.
	captureValue(prop) {
		if (prop.isHandler && this.isP('{')) {
			const block = this.captureBlock();
			prop.isBlock = true;
			prop.text = block.text;
			checkSyntax('body', prop.text, block.line, block.col);
			return;
		}
		if (this.isP('{')) {
			// `property int x: { ... return v }` : a block that returns the value.
			const block = this.captureBlock();
			prop.isBlock = true;
			prop.isReturnBlock = true;
			prop.text = block.text;
			checkSyntax('body', prop.text, block.line, block.col);
			return;
		}
		const expr = this.captureExpression();
		prop.text = expr.text;
		if (prop.isHandler) {
			prop.isBlock = true;
			// `onClicked: (mouse) => ...` and `onClicked: function(mouse) {}` are the handler itself.
			checkSyntax(callableHandler(prop.text) ? 'expr' : 'body', prop.text, expr.line, expr.col);
		} else {
			checkSyntax('expr', prop.text, expr.line, expr.col);
		}
	}

	captureExpression() {
		const first = this.peek();
		if (first.t === 'eof' || (first.t === 'p' && (first.v === ';' || first.v === '}'))) {
			this.fail('Expected an expression', first);
		}
		let depth = 0;
		let last = null;
		for (;;) {
			const tk = this.peek();
			if (tk.t === 'eof') break;
			if (depth === 0) {
				if (tk.t === 'p' && (tk.v === ';' || tk.v === '}' || tk.v === ')' || tk.v === ']')) break;
				if (last !== null && tk.nl && !continuesAcrossNewline(last, tk)) break;
			}
			if (tk.t === 'p') {
				if (tk.v === '(' || tk.v === '[' || tk.v === '{') depth++;
				else if (tk.v === ')' || tk.v === ']' || tk.v === '}') depth--;
			}
			last = tk;
			this.pos++;
		}
		return { text: this.source.slice(first.s, last.e), line: first.line, col: first.col };
	}

	// `{ ... }`: returns the text between the braces.
	captureBlock() {
		const open = this.expectP('{');
		let depth = 1;
		for (;;) {
			const tk = this.next();
			if (tk.t === 'eof') this.fail("Unexpected end of file, expected '}'", tk);
			if (tk.t === 'p') {
				if (tk.v === '{') depth++;
				else if (tk.v === '}' && --depth === 0) {
					return { text: this.source.slice(open.e, tk.s), line: open.line, col: open.col };
				}
			}
		}
	}
}

// Parses a QML document and returns its root object. Throws QmlError with a line and column. `check: false` leaves the script
// fragments unchecked: each check is a `new Function`, which a compiled application does not need to repeat, as the compiler
// has already parsed every document with the checks on.
export function parseQmlDocument(source, check = true) {
	const previous = checking;
	checking = check;
	try {
		return new Parser(source).parseDocument();
	} finally {
		checking = previous;
	}
}
