// mangleLocals (compiler/minify.js), over every real .qml file it can find (the repository, and QMLWEB_CORPUS like test/packed.mjs):
// renaming a parameter or a top-level var/let/const must never change which free name an unrenamed token reads or writes, never
// drop or add a token, and must always leave valid JavaScript behind. Checked without running the file (no Window, no network,
// no backend): the two token streams, original and mangled, are compared position by position instead.
// `node test/mangle-corpus.mjs`
import assert from 'node:assert/strict';
import { existsSync, readdirSync, readFileSync, statSync } from 'node:fs';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';
import { escapeSourceBreaks } from '../src/lexer.js';
import { mangleLocals, scriptTokens } from '../compiler/minify.js';
import { callableHandler, parseQmlDocument } from '../src/parser.js';

const here = dirname(fileURLToPath(import.meta.url));
const roots = [join(here, '..', '..', '..', '..'), ...(process.env.QMLWEB_CORPUS ? process.env.QMLWEB_CORPUS.split(';') : [])].filter((dir) => existsSync(dir));

function* walk(dir) {
	for (const name of readdirSync(dir)) {
		if (name === 'node_modules' || name === '.git' || name === '3rdParty' || name === 'Bin' || name.startsWith('Build')) continue;
		const path = join(dir, name);
		let info;
		try {
			info = statSync(path);
		} catch {
			continue;
		}
		if (info.isDirectory()) yield* walk(path);
		else if (name.endsWith('.qml')) yield path;
	}
}

// Still valid JavaScript after mangling, the same check src/parser.js runs on the original text.
function stillCompiles(kind, text, params) {
	const js = escapeSourceBreaks(text);
	if (kind === 'expr') new Function(`return (${js}\n)`);
	else new Function(params, js);
}

// The two token streams must have the same length (mangleLocals never adds or removes a token) and, at every position, either
// the same text, or the renaming map's old -> new text consistently (the same old name always becomes the same new name, and
// no two different old names collapse into one new name - that would be a name collision, not a rename).
function checkRename(file, where, before, after) {
	const a = scriptTokens(before);
	const b = scriptTokens(after);
	assert.equal(b.length, a.length, `${file} ${where}: mangleLocals changed the token count`);
	const forward = new Map();
	const backward = new Map();
	for (let i = 0; i < a.length; i++) {
		if (a[i] === b[i]) continue;
		const mappedForward = forward.get(a[i]);
		const mappedBackward = backward.get(b[i]);
		assert.ok(mappedForward === undefined || mappedForward === b[i], `${file} ${where}: '${a[i]}' was renamed to two different names`);
		assert.ok(mappedBackward === undefined || mappedBackward === a[i], `${file} ${where}: two different names were renamed to '${b[i]}'`);
		forward.set(a[i], b[i]);
		backward.set(b[i], a[i]);
	}
}

function checkFragment(file, where, kind, text, params = '') {
	const { body, params: newParams } = mangleLocals(text, params);
	checkRename(file, where, text, body);
	if (params !== newParams) checkRename(file, `${where} (params)`, params, newParams);
	stillCompiles(kind, body, newParams);
}

// The kind (checkSyntax's own, see src/parser.js captureValue): a non-handler declaration with a `{ ... return v }` block, or a
// handler, is 'body' unless the handler is itself a function/arrow expression; anything else is a plain 'expr' binding.
function propertyKind(prop) {
	if (prop.isBlock && !prop.isHandler) return 'body';
	if (prop.isHandler) return callableHandler(prop.text) ? 'expr' : 'body';
	return 'expr';
}

function walkDocument(file, def, failures) {
	for (const prop of def.properties) {
		if (prop.text === null) continue;
		try {
			checkFragment(file, `property '${prop.name}'`, propertyKind(prop), prop.text);
		} catch (error) {
			failures.push(`${file} property '${prop.name}': ${String(error.message).split('\n')[0]}`);
		}
	}
	for (const fn of def.functions) {
		try {
			checkFragment(file, `function '${fn.name}'`, 'body', fn.body, fn.params);
		} catch (error) {
			failures.push(`${file} function '${fn.name}': ${String(error.message).split('\n')[0]}`);
		}
	}
	for (const child of def.children) walkDocument(file, child, failures);
}

// test/compiler/broken/Main.qml is deliberately invalid QML (the compiler's own 'syntax error' test): parseQmlDocument with
// checking off still gives back a tree, with text that is not valid JavaScript to start with, nothing mangleLocals could help.
// The cases of JQML v5 (Tools/JQML/v5/tests/diff/cases) are written for the syntax of that engine (`as`, an object literal as a binding),
// which the parser of v4 does not take: they are not documents of this engine.
const EXCLUDE = /[\\/]test[\\/]compiler[\\/]broken[\\/]|[\\/]JQML[\\/]v5[\\/]tests[\\/]diff[\\/]cases[\\/]/;

let files = 0;
let skipped = 0;
const failures = [];
for (const root of roots) {
	for (const file of walk(root)) {
		if (EXCLUDE.test(file)) continue;
		let def;
		try {
			def = parseQmlDocument(readFileSync(file, 'utf8'), false);
		} catch {
			skipped++;
			continue;
		}
		files++;
		walkDocument(file, def, failures);
	}
}
console.log(`${files} documents checked (${skipped} that do not parse)`);
for (const failure of failures.slice(0, 20)) console.log(`[-] ${failure}`);
console.log(failures.length === 0 && files > 0 ? 'All good' : `${failures.length} failed`);
process.exit(failures.length === 0 && files > 0 ? 0 : 1);
