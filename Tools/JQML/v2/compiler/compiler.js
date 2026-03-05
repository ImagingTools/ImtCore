#!/usr/bin/env node
// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
'use strict';

/**
 * JQML v2 — QML-to-JavaScript compiler entry point.
 *
 * Usage:
 *   node compiler.js <source-dir>
 *
 * Compiles all .qml and .js files found under <source-dir> into a single
 * bundled JavaScript file at <source-dir>/jqml.full.js.
 *
 * The output file is then referenced by a .qrc resource file and compiled
 * into the Qt application with rcc.
 *
 * Exit codes:
 *   0  — success (all files compiled without errors)
 *   1  — one or more files had compilation errors (output still written)
 *   2  — fatal error (bad arguments, I/O failure, etc.)
 */

const path = require('path');
const { processDirectory } = require('./src/bundler');

// ── Argument parsing ──────────────────────────────────────────────────────────

const args = process.argv.slice(2);

if (args.length < 1 || args[0] === '--help' || args[0] === '-h') {
    process.stdout.write([
        'JQML v2 — QML-to-JavaScript compiler',
        '',
        'Usage:',
        '  node compiler.js <source-dir>',
        '',
        'Arguments:',
        '  source-dir   Directory that contains the .qml and .js source files.',
        '               The bundled output is written to <source-dir>/jqml.full.js.',
        '',
        'Environment variables:',
        '  JQML_VERBOSE=1   Enable verbose logging.',
        '',
        'Examples:',
        '  node compiler.js build/web/src',
        '  node compiler.js $BUILDDIR/web/src',
    ].join('\n') + '\n');
    process.exit(args.length < 1 ? 2 : 0);
}

const srcDir     = path.resolve(args[0]);
const outputFile = path.join(srcDir, 'jqml.full.js');
const verbose    = process.env.JQML_VERBOSE === '1';

// ── Compile ───────────────────────────────────────────────────────────────────

process.stdout.write(`[JQML v2] Compiling: ${srcDir}\n`);

let result;
try {
    result = processDirectory(srcDir, outputFile);
} catch (err) {
    process.stderr.write(`[JQML v2] Fatal error: ${err.message}\n`);
    if (verbose) process.stderr.write(err.stack + '\n');
    process.exit(2);
}

if (verbose) {
    process.stdout.write(
        `[JQML v2] Processed ${result.qml} QML file(s) and ${result.js} JS file(s)\n`
    );
}

process.stdout.write(
    `[JQML v2] ${result.errors === 0 ? 'OK' : `${result.errors} error(s)`}` +
    ` — output: ${outputFile}\n`
);

process.exit(result.errors > 0 ? 1 : 0);
