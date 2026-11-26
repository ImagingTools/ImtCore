// The names of an expression that are looked up before the run (src/resolve.js) give what the `with` chain of Engine.compile gives: the order
// of the objects, `this` of a call, a name that is nowhere, objects with keys, `new`; what is not understood is left to `with`.
// Every expression is run both ways over the same scopes. `node test/resolve.mjs`
import { lookupOrder, resolveExpression } from '../src/resolve.js';

let failures = 0;
function check(name, ok, detail = '') {
	if (!ok) failures++;
	console.log(`${ok ? '[+]' : '[-]'} ${name}${ok ? '' : `: ${detail}`}`);
}

// Scopes of a depth of 3 with a context at the levels 0 and 2: the arguments of the factory (Engine.compile).
const depth = 3;
const contexts = '101';
const names = ['__g'];
for (let l = 0; l < depth; l++) {
	if (contexts[l] === '1') names.push(`__c${l}`);
	names.push(`__r${l}`, `__i${l}`);
}
names.push('__self');

class Thing {
	constructor(label) {
		this.label = label;
		this.value = 1;
	}

	whoami() {
		return this.label;
	}
}

function scopes() {
	const make = (label, extra = {}) => Object.assign(new Thing(label), extra);
	const ids0 = Object.create({ Style: { radius: 3 } }); // the ids layer carries singletons
	ids0.header = make('header', { width: 10 });
	ids0.shared = 'id0';
	const ids1 = Object.create(null);
	ids1.outer = make('outer');
	ids1.shared = 'id1';
	const ids2 = Object.create(null);
	const self = make('self', { width: 5, shared: 'self', onlySelf: 's' });
	const root0 = make('root0', { shared: 'root0', onlyRoot: 'r0', width: 99 });
	const root1 = make('root1', { shared: 'root1', onlyRoot1: 'r1' });
	const root2 = make('root2');
	const ctx0 = Object.create(null);
	ctx0.index = 7;
	ctx0.modelData = { name: 'row' };
	ctx0.shared = 'ctx0';
	const ctx2 = Object.create(null);
	ctx2.role = 'deep';
	const globals = { Qt: { tr: (text) => `tr:${text}` }, Math, shared: 'global', onlyGlobal: 'g', makeThing: (label) => new Thing(label), Thing };
	const args = { __g: globals, __c0: ctx0, __r0: root0, __i0: ids0, __r1: root1, __i1: ids1, __c2: ctx2, __r2: root2, __i2: ids2, __self: self };
	return { self, args: names.map((name) => args[name]) };
}

function viaWith(code) {
	let open = 'with (__g) {';
	let closing = 2 * depth + 2;
	for (let l = depth - 1; l >= 1; l--) {
		if (contexts[l] === '1') {
			open += `with (__c${l}) {`;
			closing++;
		}
		open += `with (__r${l}) { with (__i${l}) {`;
	}
	if (contexts[0] === '1') {
		open += 'with (__c0) {';
		closing++;
	}
	open += 'with (__r0) { with (__self) { with (__i0) {';
	return new Function(...names, `${open} return function () {\nreturn (${code}\n);\n}; ${'}'.repeat(closing)}`);
}

function describe(fn, scope) {
	try {
		const value = fn.apply(scope.self, []);
		return typeof value === 'object' && value !== null ? JSON.stringify(value) : String(value);
	} catch (error) {
		return `threw ${error.constructor.name}`;
	}
}

const same = [
	['the order: an id of level 0 first', 'shared'],
	['the item before the root', 'width'],
	['the root of level 0', 'onlyRoot + "!"'],
	['the context of level 0', 'index + 1'],
	['a role of the context of level 0', 'modelData.name'],
	['an id of level 1', 'outer.label'],
	['the root of level 1', 'onlyRoot1'],
	['a role of the context of level 2', 'role'],
	['a global', 'onlyGlobal'],
	['a singleton of the ids layer', 'Style.radius * 2'],
	['Math through the globals', 'Math.max(width, 3) + Math.min(1, 2)'],
	['a call keeps its object: a method of the item', 'whoami()'],
	['a call of a method of an id', 'header.whoami() + "/" + outer.whoami()'],
	['a call of a global function', 'makeThing("made").label'],
	['a call of Qt', 'Qt.tr("x")'],
	['new of a global class', 'new Thing("fresh").label'],
	['new of a member', 'new (makeThing("c").constructor)("viaNew").label'],
	['a ternary and logic', 'width > 3 ? onlySelf : onlyGlobal'],
	['an object literal with keys and shorthand', '({ a: width, onlySelf, "b": index })'],
	['an array and spread', '[width, ...[index, onlyRoot]]'],
	['property names are not looked up', 'header.width + outer.value + this.value'],
	['a name that is nowhere', 'nowhereAtAll'],
	['a call of a name that is nowhere', 'nowhereAtAll(1)'],
	['a string with names in it', '"width " + \'shared\' + width'],
	['a regular expression', '/shared/.test(onlyRoot + "shared")'],
	['a template without ${}', '`width`'],
	['in and instanceof', '("width" in header) && header instanceof Thing'],
	['optional chaining on a property', 'header?.width + (outer?.nope ?? 4)'],
	['a comma and parentheses', '(width, shared)'],
	['the unary operators', '!width + -width + +index'],
	['undefined, NaN, null, true', '[undefined, NaN, null, true, index].length'],
];
for (const [name, code] of same) {
	const body = resolveExpression(code, '', depth, contexts);
	if (body === null) {
		check(name, false, 'was left to with');
		continue;
	}
	const a = scopes();
	const b = scopes();
	const expected = describe(viaWith(code)(...a.args), a);
	const actual = describe(new Function(...names, body)(...b.args), b);
	check(name, expected === actual, `with: ${expected}, resolved: ${actual}`);
}

const left = [
	['an assignment', 'width = 3'],
	['an increment', 'index++'],
	['an arrow function', 'items.map(x => x + 1)'],
	['a function', '(function () { return width })()'],
	['typeof of a name', 'typeof nowhere'],
	['a template with ${}', '`a ${width}`'],
	['a declaration', 'let a = 1'],
	['delete', 'delete shared'],
];
for (const [name, code] of left) check(`left to with: ${name}`, resolveExpression(code, '', depth, contexts) === null);

check('the order for a scope of no depth is the item', JSON.stringify(lookupOrder(0, '')) === '["__self"]');
check('the order of three levels', JSON.stringify(lookupOrder(3, '101')) === '["__i0","__self","__r0","__c0","__i1","__r1","__i2","__r2","__c2"]', JSON.stringify(lookupOrder(3, '101')));
console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
