// Names of a binding expression without `with`.
//
// The engine runs a script inside `with (globals) { with (...ids, root, context of the outer levels) { with (item) { with (ids) { ... } } } }`
// (Engine.compile), so a name is looked up in the objects one after the other at every use, which is slow and keeps V8 from optimizing
// the function. For an expression this module writes the same lookup out: `x` becomes `("x" in a ? a.x : "x" in b ? b.x : ... : x)`
// over the objects in the order that the `with` chain has, a call `f(...)` keeps the object it was found on as `this`, and the name
// that is nowhere is the bare name at last (the page's own global, or a ReferenceError, as with `with`).
//
// Only what is understood for certain is rewritten: an expression that has an assignment, a function, a declaration, a template with
// `${}`, `typeof name`, `delete name` and the like is left to `with` (resolveExpression gives null).

import { escapeSourceBreaks, tokenize } from './lexer.js';

// Tokens that make the analysis give up.
const GIVE_UP = new Set(['=>', 'function', 'class', 'var', 'let', 'const', 'async', 'await', 'yield', 'super', 'import', 'export', 'with', 'eval', 'arguments', 'delete', 'void', 'typeof']);
const ASSIGNMENT = new Set(['=', '+=', '-=', '*=', '/=', '%=', '**=', '<<=', '>>=', '>>>=', '&=', '|=', '^=', '&&=', '||=', '??=', '++', '--']);
// Names that are not looked up: they are values, or operators of the language.
const LITERALS = new Set(['true', 'false', 'null', 'undefined', 'NaN', 'Infinity', 'this', 'instanceof', 'in', 'new', 'if', 'else', 'return']);

// The objects that a name is looked up in, innermost first, as the names of the arguments of the factory (Engine.compile):
// ids and the item of the level 0, then the root, the context of the level 0, then ids, root and context of each outer level.
export function lookupOrder(depth, contexts) {
	const order = [];
	if (depth === 0) {
		order.push('__self');
		return order;
	}
	order.push('__i0', '__self', '__r0');
	if (contexts[0] === '1') order.push('__c0');
	for (let l = 1; l < depth; l++) {
		order.push(`__i${l}`, `__r${l}`);
		if (contexts[l] === '1') order.push(`__c${l}`);
	}
	return order;
}

const parsed = new Map();

// The tokens of the expression that matter, or null when the expression is not understood.
function analyze(code) {
	const hit = parsed.get(code);
	if (hit !== undefined) return hit;
	let result = null;
	try {
		result = analyzeTokens(escapeSourceBreaks(code));
	} catch {
		result = null;
	}
	parsed.set(code, result);
	return result;
}

function analyzeTokens(source) {
	const tokens = tokenize(source);
	const sites = []; // { s, e, name, kind }: kind 'read', 'call' or 'shorthand'
	const braces = []; // true for the `{` of an object literal, false for a `(`/`[`: the objects whose keys are not names
	for (let i = 0; i < tokens.length; i++) {
		const token = tokens[i];
		if (token.t === 'eof') break;
		if (token.t === 'tpl') {
			if (token.v.includes('${')) return null;
			continue;
		}
		if (token.t === 'p') {
			if (ASSIGNMENT.has(token.v) || GIVE_UP.has(token.v)) return null;
			if (token.v === '{') braces.push(true);
			else if (token.v === '(' || token.v === '[') braces.push(false);
			else if (token.v === '}' || token.v === ')' || token.v === ']') braces.pop();
			continue;
		}
		if (token.t !== 'id') continue;
		if (/^[0-9]/.test(token.v)) continue;
		if (GIVE_UP.has(token.v)) return null;
		if (LITERALS.has(token.v)) continue;
		const prev = i > 0 ? tokens[i - 1] : null;
		const next = tokens[i + 1];
		// a property name after a dot
		if (prev !== null && prev.t === 'p' && (prev.v === '.' || prev.v === '?.')) continue;
		const inObject = braces.length > 0 && braces[braces.length - 1];
		if (inObject && prev !== null && prev.t === 'p' && (prev.v === '{' || prev.v === ',')) {
			// `{ key: value }`, `{ key }`, `{ key() {} }`
			if (next.t === 'p' && next.v === ':') continue;
			if (next.t === 'p' && (next.v === ',' || next.v === '}')) {
				sites.push({ s: token.s, e: token.e, name: token.v, kind: 'shorthand' });
				continue;
			}
			return null;
		}
		if (next.t === 'tpl') return null;
		// `name?.(...)` calls what was found without its object: left to with; `name?.property` is a read
		if (next.t === 'p' && next.v === '?.' && tokens[i + 2].t === 'p' && tokens[i + 2].v === '(') return null;
		if (next.t === 'p' && next.v === '(') sites.push({ s: token.s, e: token.e, name: token.v, kind: 'call' });
		else sites.push({ s: token.s, e: token.e, name: token.v, kind: 'read' });
	}
	if (braces.length !== 0) return null;
	return { source, sites };
}

// The text of the lookup of `name` over `order`: the object it is found on, or the value.
function lookup(name, order, holder) {
	const key = JSON.stringify(name);
	let out = '';
	for (const object of order) out += `${key} in ${object} ? ${holder ? object : `${object}.${name}`} : `;
	out += `${key} in __g ? ${holder ? '__g' : `__g.${name}`} : `;
	// nowhere in the scopes: the page's own name (a ReferenceError if it has none); a call on it is made on the global object
	out += holder ? `(${name}, globalThis)` : name;
	return `(${out})`;
}

// The source of the factory body for `kind: 'expr'` code in a scope of `depth` levels with `contexts` (a string of 0 and 1 per level),
// or null when the expression is left to `with`. The factory takes the same arguments as the `with` one (Engine.compile).
export function resolveExpression(code, params, depth, contexts) {
	const analysis = analyze(code);
	if (analysis === null) return null;
	const order = lookupOrder(depth, contexts);
	let out = '';
	let at = 0;
	for (const site of analysis.sites) {
		out += analysis.source.slice(at, site.s);
		if (site.kind === 'call') out += `(__h = ${lookup(site.name, order, true)}).${site.name}`;
		else if (site.kind === 'shorthand') out += `${site.name}: ${lookup(site.name, order, false)}`;
		else out += lookup(site.name, order, false);
		at = site.e;
	}
	out += analysis.source.slice(at);
	return `return function (${params}) {\nvar __h;\nreturn (${out}\n);\n};`;
}
