// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
'use strict';

/**
 * Minimal self-contained test suite for the JQML v2 compiler.
 * Run with:  node test/run.js
 */

const { tokenize, TT } = require('../src/lexer');
const { parse }        = require('../src/parser');
const { generate }     = require('../src/codegen');

let passed = 0;
let failed = 0;

function assert(condition, message) {
    if (condition) {
        process.stdout.write(`  ✓  ${message}\n`);
        passed++;
    } else {
        process.stderr.write(`  ✗  ${message}\n`);
        failed++;
    }
}

// ── Lexer tests ───────────────────────────────────────────────────────────────

process.stdout.write('Lexer\n');
{
    const toks = tokenize('import QtQuick 2.12');
    assert(toks[0].t === TT.IMPORT,      'import keyword recognised');
    assert(toks[1].t === TT.IDENTIFIER,  'module identifier recognised');
    assert(toks[2].t === TT.NUMBER,      'version number recognised');
}
{
    const toks = tokenize('// comment\nproperty string name');
    assert(toks[0].t === TT.PROPERTY,    'line comment skipped, property recognised');
}
{
    const toks = tokenize('/* block */ signal clicked()');
    assert(toks[0].t === TT.SIGNAL,      'block comment skipped, signal recognised');
}
{
    const toks = tokenize('"hello world"');
    assert(toks[0].t === TT.STRING && toks[0].v === 'hello world', 'string literal');
}
{
    const toks = tokenize('42 3.14');
    assert(toks[0].t === TT.NUMBER && toks[0].v === '42',   'integer literal');
    assert(toks[1].t === TT.NUMBER && toks[1].v === '3.14', 'float literal');
}

// ── Parser tests ──────────────────────────────────────────────────────────────

process.stdout.write('\nParser\n');
{
    const src = `
import QtQuick 2.12
Rectangle {
    id: root
    property string title: "Hello"
    property int count: 0
    signal clicked()
    onClicked: { count++ }
    function reset() { count = 0 }
    Text {
        id: label
        text: root.title
    }
}`;
    const ast = parse(src);

    assert(ast.imports.length >= 1,                           'import parsed');
    assert(ast.component !== null,                            'component parsed');
    assert(ast.component.type === 'Rectangle',                'component type');
    assert(ast.component.id   === 'root',                     'component id');
    assert(ast.component.properties.length === 2,             'two properties');
    assert(ast.component.properties[0].name === 'title',      'property name: title');
    assert(ast.component.properties[0].typeName === 'string', 'property type: string');
    assert(ast.component.signals.length === 1,                'one signal');
    assert(ast.component.signals[0].name === 'clicked',       'signal name: clicked');
    assert(ast.component.handlers.length >= 1,                'handler parsed');
    assert(ast.component.functions.length === 1,              'function parsed');
    assert(ast.component.children.length === 1,               'one child');
    assert(ast.component.children[0].type === 'Text',         'child type: Text');
    assert(ast.component.children[0].id   === 'label',        'child id: label');
}

// ── Code-gen tests ────────────────────────────────────────────────────────────

process.stdout.write('\nCode generation\n');
{
    const src = `
import QtQuick 2.12
Item {
    id: root
    property bool visible: true
    signal done()
    onDone: { console.log("done") }
    function greet(name) { return "Hi " + name }
}`;
    const ast = parse(src);
    const js  = generate(ast, 'Item');

    assert(js.includes('JQML.define('),      'JQML.define call emitted');
    assert(js.includes('"Item"'),             'component name in output');
    assert(js.includes('"visible"'),          'property name in output');
    assert(js.includes('"done"'),             'signal name in output');
    assert(js.includes('"onDone"'),           'handler key in output');
    assert(js.includes('greet: function(name)'), 'function in output');
}
{
    const ast = parse('');
    const js  = generate(ast, 'Empty');
    assert(js.includes('// JQML: No component'), 'empty file handled gracefully');
}

// ── Alias property ────────────────────────────────────────────────────────────

process.stdout.write('\nAlias property\n');
{
    const src = `
Item {
    property alias myView: internalView_
    ListView { id: internalView_ }
}`;
    const ast = parse(src);
    assert(ast.component.properties[0].isAlias === true, 'alias flag set');
    assert(ast.component.properties[0].name === 'myView', 'alias name correct');
}

// ── Dotted event handler ──────────────────────────────────────────────────────

process.stdout.write('\nDotted handler\n');
{
    const src = `
Item {
    Component.onCompleted: { doSetup() }
}`;
    const ast = parse(src);
    const h   = ast.component.handlers.find(x => x.event === 'Component.onCompleted');
    assert(h !== undefined, 'Component.onCompleted parsed');
}

// ── Summary ───────────────────────────────────────────────────────────────────

process.stdout.write(`\n${passed + failed} tests: ${passed} passed, ${failed} failed\n`);
process.exit(failed > 0 ? 1 : 0);
