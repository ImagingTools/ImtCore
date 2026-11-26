// mangleLocals (compiler/minify.js): renames the plain parameters and the top-level var/let/const of one script fragment, and
// only those, exactly when nothing in the fragment makes that ambiguous (a nested function, destructuring, a name declared
// twice, a for-loop's own let/const...), in which case it bails and returns the fragment unchanged. The same safety property,
// checked against every real .qml file this repository (and others) can find, is test/mangle-corpus.mjs.
// `node test/mangle-locals.mjs`
import { mangleLocals } from '../compiler/minify.js';

let failures = 0;
function check(name, body, params, expectBody, expectParams) {
	const { body: outBody, params: outParams } = mangleLocals(body, params);
	const ok = outBody === expectBody && outParams === expectParams;
	if (!ok) failures++;
	console.log(`${ok ? '[+]' : '[-]'} ${name}${ok ? '' : `\n    got body: ${JSON.stringify(outBody)}\n    got params: ${JSON.stringify(outParams)}`}`);
}

// plain var, renamed consistently, including a non-nested if-block (var is function-scoped)
check('plain var', 'var a = 1; if (root.ready) { a = a + 1; } return a;', '', 'var $a0 = 1; if (root.ready) { $a0 = $a0 + 1; } return $a0;', '');

// parameter renamed, outer id left alone
check('param and outer id', 'return x + root.y;', 'x', 'return $a0 + root.y;', '$a0');

// member access after a dot is never renamed, even though the word matches
check('member access preserved', 'var a = 1; return root.a + a;', '', 'var $a0 = 1; return root.a + $a0;', '');

// object literal key preserved, its value still renamed
check('object literal key', 'var a = 1; return { a: a, b: 2 };', '', 'var $a0 = 1; return { a: $a0, b: 2 };', '');

// ternary middle branch is a value, not a key: still renamed
check('ternary is a value', 'var a = 1; return cond ? a : 0;', '', 'var $a0 = 1; return cond ? $a0 : 0;', '');

// nested object literal key preserved at its own level
check('nested literal key', 'var a = 1; return { outer: { a: a } };', '', 'var $a0 = 1; return { outer: { a: $a0 } };', '');

// array literal items are values, always renamed
check('array literal value', 'var a = 1; return [a, 2];', '', 'var $a0 = 1; return [$a0, 2];', '');

// bails: nested function
{
	const body = 'var a = 1; return function() { return a; };';
	const r = mangleLocals(body, '');
	console.log(r.body === body && r.params === '' ? '[+] bails on nested function' : '[-] bails on nested function');
	if (r.body !== body) failures++;
}

// bails: arrow function
{
	const body = 'var a = 1; items.forEach(x => a += x);';
	const r = mangleLocals(body, '');
	console.log(r.body === body ? '[+] bails on arrow function' : '[-] bails on arrow function');
	if (r.body !== body) failures++;
}

// bails: destructuring declaration
{
	const body = 'var { a, b } = root; return a + b;';
	const r = mangleLocals(body, '');
	console.log(r.body === body ? '[+] bails on destructuring' : '[-] bails on destructuring');
	if (r.body !== body) failures++;
}

// bails: destructuring parameter
{
	const r = mangleLocals('return a.x;', '{ a }');
	console.log(r.params === '{ a }' && r.body === 'return a.x;' ? '[+] bails on destructured parameter' : '[-] bails on destructured parameter');
	if (r.params !== '{ a }') failures++;
}

// bails: name declared twice
{
	const body = 'var a = 1; var a = 2; return a;';
	const r = mangleLocals(body, '');
	console.log(r.body === body ? '[+] bails on a name declared twice' : '[-] bails on a name declared twice');
	if (r.body !== body) failures++;
}

// bails: var shadows a parameter
{
	const r = mangleLocals('var a = 2; return a;', 'a');
	const unchanged = r.body === 'var a = 2; return a;' && r.params === 'a';
	console.log(unchanged ? '[+] bails when a var shadows a parameter' : '[-] bails when a var shadows a parameter');
	if (!unchanged) failures++;
}

// bails: let/const below the top level (block-scoped, could be shadowed by a sibling block)
{
	const body = 'if (root.ready) { let a = 1; return a; } return 0;';
	const r = mangleLocals(body, '');
	console.log(r.body === body ? '[+] bails on a nested let' : '[-] bails on a nested let');
	if (r.body !== body) failures++;
}

// a top-level let is fine
check('top-level let', 'let a = 1; return a;', '', 'let $a0 = 1; return $a0;', '');

// bails: a for-loop's own let goes out of scope again inside the same fragment, so a later bare 'i' could mean something else
// entirely (an outer id, say) that must not be touched; var does not have this problem (it is scoped to the whole fragment)
{
	const body = 'for (let i = 0; i < n; i++) { total += i; } return i.width;';
	const r = mangleLocals(body, '');
	console.log(r.body === body ? "[+] bails on a for-loop's own let" : "[-] bails on a for-loop's own let");
	if (r.body !== body) failures++;
}
check('a for-loop\'s own var is still fragment-scoped, safe to rename', 'for (var i = 0; i < n; i++) { total += i; } return i;', '', 'for (var $a0 = 0; $a0 < n; $a0++) { total += $a0; } return $a0;', '');

// a chosen name never collides with a word already used in the fragment, even one that is not itself being renamed
// (here $a0 is an outer id, not a local: localNames never adds it to `names`, so it is a word mangleLocals must avoid)
check('generated name avoids a name already used as text', 'var a = 1; return a + $a0.x;', '', 'var $a1 = 1; return $a1 + $a0.x;', '');

console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
