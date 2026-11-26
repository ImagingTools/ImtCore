// Builds lib/ and packs the extension into jqml4.vsix.

import fs from 'node:fs';
import path from 'node:path';
import { spawnSync } from 'node:child_process';
import { fileURLToPath } from 'node:url';

const root = path.resolve(path.dirname(fileURLToPath(import.meta.url)), '..');
const windows = process.platform === 'win32';

const build = spawnSync(process.execPath, [path.join(root, 'scripts', 'build.mjs')], { stdio: 'inherit' });
if (build.status !== 0) process.exit(build.status || 1);

for (const name of fs.readdirSync(root)) if (name.endsWith('.vsix')) fs.unlinkSync(path.join(root, name));

const out = path.join(root, 'jqml4.vsix');
const vsce = spawnSync(windows ? 'npx.cmd' : 'npx', ['--yes', '@vscode/vsce', 'package', '--out', out, '--allow-missing-repository', '--skip-license'], { cwd: root, stdio: 'inherit', shell: windows });
if (vsce.status !== 0) process.exit(vsce.status || 1);
console.log(`Packed ${out}`);
