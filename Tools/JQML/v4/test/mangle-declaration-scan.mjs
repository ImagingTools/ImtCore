// mangleLocals and a `var` in a block of a QML script written without ';' (the shape of FileSystemBrowserDialog.browse()): the scan that skips the
// initializer of the declaration must stop at the '}' of the block the `var` sits in. It went on into the statements that follow, took a ',' of
// the call `root.fillEntriesInput(root.a, root.b)` there for the next name of the declaration, and so renamed `root` (an id) to `$a3`, which
// nothing declares: the function threw a ReferenceError and the dialog of the files stayed empty in the web build.
// `node test/mangle-declaration-scan.mjs`
import assert from 'node:assert/strict';
import { mangleLocals } from '../compiler/minify.js';

let failures = 0;
function check(name, run) {
	try {
		run();
		console.log(`[+] ${name}`);
	} catch (error) {
		failures++;
		console.log(`[-] ${name}: ${error.message}`);
	}
}

const BROWSE = `
if (root.isLoading)
	return
root.__appending = append === true
if (!root.__appending){
	var nextPath = path !== undefined && path !== null ? path : ""
	if (nextPath !== root.currentPath)
		root.filterText = ""
}
root.isLoading = true
root.__lastAttemptedPath = path !== undefined && path !== null ? path : ""
root.fillEntriesInput(root.__lastAttemptedPath, root.__appending)
requestSender.send()
`;

check('an id is not renamed after a var in a block', () => {
	const { body, params } = mangleLocals(BROWSE, 'path, append');
	assert.equal(params, '$a0, $a1');
	assert.ok(!/\$a3/.test(body), 'root was renamed');
	assert.ok(body.includes('root.fillEntriesInput('), 'the call kept its object');
	assert.ok(body.includes('var $a2 = $a0 !== undefined'), 'the var and the parameter were renamed');
});

check('the mangled function runs', () => {
	const { body, params } = mangleLocals(BROWSE, 'path, append');
	const calls = [];
	const root = { isLoading: false, currentPath: '', filterText: 'x', fillEntriesInput: (...args) => calls.push(args) };
	const requestSender = { send: () => calls.push('send') };
	new Function('root', 'requestSender', `return function (${params}) {\n${body}\n}`)(root, requestSender)('C:/', false);
	assert.deepEqual(calls, [['C:/', false], 'send']);
	assert.equal(root.isLoading, true);
	assert.equal(root.filterText, '');
});

check('a declaration list is still renamed', () => {
	const { body } = mangleLocals('var a = 1, b = 2\nroot.x = a + b', '');
	assert.equal(body, 'var $a0 = 1, $a1 = 2\nroot.x = $a0 + $a1');
});

check('a var in a block, then a call with a comma, at the top level', () => {
	const { body } = mangleLocals('if (c){\nvar n = 1\nroot.a = n\n}\nroot.f(root.b, root.c)', '');
	assert.ok(body.includes('root.f(root.b, root.c)'));
});

console.log(failures === 0 ? 'All good' : `${failures} failed`);
process.exit(failures === 0 ? 0 : 1);
