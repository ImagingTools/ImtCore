// The packed form of a document (src/packed.js) gives back the tree of the parser, and the smaller scripts of the compiler
// (compiler/minify.js) are the same code: every .qml file below the folders given (the repository, and the folders named in
// QMLWEB_CORPUS, separated by `;`) is parsed, packed, written as JSON, read again and compared.
// `node test/packed.mjs`
import assert from 'node:assert/strict';
import { existsSync, readdirSync, readFileSync, statSync } from 'node:fs';
import { dirname, join } from 'node:path';
import { fileURLToPath } from 'node:url';
import { minifyScript, scriptTokens } from '../compiler/minify.js';
import { packDocument, unpackDocument } from '../src/packed.js';
import { parseQmlDocument } from '../src/parser.js';

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

// The tree with every script replaced by its tokens: two scripts that do the same have the same ones.
function tokensOf(def) {
	return JSON.parse(
		JSON.stringify(def, (key, value) => (typeof value === 'string' && (key === 'text' || key === 'body') ? scriptTokens(value) : value)),
	);
}

let files = 0;
let skipped = 0;
let scriptBytes = 0;
let minifiedBytes = 0;
const failures = [];
for (const root of roots) {
	for (const file of walk(root)) {
		let def;
		try {
			def = parseQmlDocument(readFileSync(file, 'utf8'), false);
		} catch {
			skipped++;
			continue;
		}
		files++;
		try {
			assert.deepStrictEqual(unpackDocument(JSON.parse(JSON.stringify(packDocument(def)))), def);
			const small = unpackDocument(JSON.parse(JSON.stringify(packDocument(def, minifyScript))));
			assert.deepStrictEqual(tokensOf(small), tokensOf(def));
			const count = (tree) => {
				let size = 0;
				const visit = (node) => {
					for (const prop of node.properties) size += (prop.text || '').length;
					for (const fn of node.functions) size += fn.body.length;
					node.children.forEach(visit);
				};
				visit(tree);
				return size;
			};
			scriptBytes += count(def);
			minifiedBytes += count(small);
		} catch (error) {
			failures.push(`${file}: ${String(error.message).split('\n')[0]}`);
		}
	}
}
console.log(`${files} documents (${skipped} that do not parse), scripts ${(scriptBytes / 1e6).toFixed(2)} MB -> ${(minifiedBytes / 1e6).toFixed(2)} MB`);
for (const failure of failures.slice(0, 20)) console.log(`[-] ${failure}`);
console.log(failures.length === 0 && files > 0 ? 'All good' : `${failures.length} failed`);
process.exit(failures.length === 0 && files > 0 ? 0 : 1);
