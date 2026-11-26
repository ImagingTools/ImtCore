// Prepares language/: the JQML 4 language service that the Composer asks for the types and the properties of a schema. It is not
// written again here: the folder of the language extension (../jqml-vscode) builds its lib/ (a copy of the JQML v4 lexer, parser and
// config reader, and engine-index.json, the description of the built-in types), and this script copies that lib/ and the three files of
// the service next to each other, so the paths the service reads (../lib/...) stay what they are there. Nothing of that extension is
// changed, and this one runs without it installed.

import fs from 'node:fs';
import path from 'node:path';
import { spawnSync } from 'node:child_process';
import { fileURLToPath } from 'node:url';

const extensionRoot = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const languageExtension = path.resolve(extensionRoot, '..', 'jqml-vscode');
const languageDir = path.join(extensionRoot, 'language');

if (!fs.existsSync(path.join(languageExtension, 'scripts', 'build.mjs'))) {
	throw new Error(`The language extension was not found at ${languageExtension}`);
}

const build = spawnSync(process.execPath, [path.join(languageExtension, 'scripts', 'build.mjs')], { stdio: 'inherit' });
if (build.status !== 0) process.exit(build.status || 1);

fs.rmSync(languageDir, { recursive: true, force: true });
fs.mkdirSync(path.join(languageDir, 'src'), { recursive: true });
fs.mkdirSync(path.join(languageDir, 'lib'), { recursive: true });
for (const name of ['service.mjs', 'project.mjs', 'scanner.mjs']) {
	fs.copyFileSync(path.join(languageExtension, 'src', name), path.join(languageDir, 'src', name));
}
for (const name of fs.readdirSync(path.join(languageExtension, 'lib'))) {
	fs.copyFileSync(path.join(languageExtension, 'lib', name), path.join(languageDir, 'lib', name));
}
console.log(`Prepared ${languageDir}`);
