// The compiler: the parser fuzzed with mutations of real QML and generated documents (it ends, with a result or an error
// of its own, never a crash of another kind or a hang), and the output of every real application byte for byte the same
// when compiled twice.
const test = require('node:test')
const assert = require('node:assert/strict')
const fs = require('fs')
const os = require('os')
const path = require('path')
const { random } = require('./stress')
const { available } = require('../bench/apps')

const parser = require('../../compiler/parser')
const { compile, listQmlFiles } = require('../../compiler/index')

function realQml(limit){
    let files = []
    for(let app of available()){
        for(let file of listQmlFiles(app.config, app.entry)) if(!files.includes(file)) files.push(file)
    }
    return files.slice(0, limit)
}

// an error of the parser: a SyntaxError or an Error with a message, not a TypeError / RangeError of its own code
function parseOnce(source){
    let start = Date.now()
    try {
        parser.parse(source)
    } catch(error) {
        assert.ok(!(error instanceof TypeError) && !(error instanceof RangeError) && !(error instanceof ReferenceError), 'parser crashed: ' + (error && error.stack))
        assert.ok(error && error.message, 'an error without a message')
    }
    assert.ok(Date.now() - start < 2000, 'the parser took ' + (Date.now() - start) + ' ms')
}

const TOKENS = ['{', '}', '(', ')', '[', ']', ':', ';', ',', '.', '"', "'", '`', '/', '/*', '*/', '//', '\n', ' ', 'property', 'int', 'var',
    'alias', 'signal', 'function', 'on', 'import', 'QtQuick', '2.15', 'Item', 'id', 'x', '1', '0x1F', '1e3', '/re/g', 'a / b / c', '${', '\\u0041',
    'enum', 'component', 'required', 'readonly', 'default', 'pragma', 'Singleton', '=>', '?', '?.', '??', '...', 'let', 'const', '=']

test('the parser on 3000 mutations of real QML and 2000 generated documents', { timeout: 300000 }, () => {
    let rnd = random(5)
    let pick = (list) => list[Math.floor(rnd() * list.length)]
    let sources = realQml(300).map(file => fs.readFileSync(file, 'utf8'))
    assert.ok(sources.length > 0)
    for(let i = 0; i < 3000; i++){
        let text = pick(sources)
        let at = Math.floor(rnd() * text.length)
        let kind = rnd()
        let mutated = kind < 0.3 ? text.slice(0, at) + text.slice(at + 1 + Math.floor(rnd() * 20))
            : kind < 0.6 ? text.slice(0, at) + pick(TOKENS) + text.slice(at)
            : kind < 0.8 ? text.slice(0, at)
            : text.slice(0, at) + text.slice(Math.floor(rnd() * text.length))
        parseOnce(mutated)
    }
    for(let i = 0; i < 2000; i++){
        let n = 1 + Math.floor(rnd() * 60)
        parseOnce('import QtQuick 2.15\n' + Array.from({ length: n }, () => pick(TOKENS)).join(rnd() < 0.5 ? ' ' : ''))
    }
})

test('the parser on the edge cases of the grammar', () => {
    let cases = [
        'Item { property int a: b / c / d }',
        'Item { property var r: /a\\/b[/]c/gi.test(x) ? 1 : 2 }',
        'Item { property string t: `a ${ `b ${c}` } d` }',
        'Item { property int a: 1\n property int b: 2 }',
        'Item { function f(){ return\n 1 } }',
        'Item { property string s: "\\u{1F600} é 中" }',
        'Item { enum Color { Red, Green = 5, Blue } }',
        'Item { component Inline: Rectangle { required property int n } Inline { n: 1 } }',
        'Item { readonly property int r: 1; default property list<Item> kids; required property var m }',
        'Item { property alias a: inner.child.value; Item { id: inner; property Item child: Item { property int value } } }',
        'pragma Singleton\nimport QtQuick 2.15\nQtObject { }',
        'import QtQuick 2.15 as Q\nimport "dir" as D\nQ.Item { }',
        'Item { property int a: x?.y ?? 0; property var f: (a, ...b) => a + b.length }',
        'Item { // comment with import "x" and /* */\n /* block import "y" */ }',
    ]
    for(let text of cases) parseOnce(text.startsWith('import') || text.startsWith('pragma') ? text : 'import QtQuick 2.15\n' + text)
})

test('every real application compiles to the same bytes twice', { timeout: 600000 }, () => {
    for(let app of available()){
        let outputs = []
        for(let i = 0; i < 2; i++){
            let out = fs.mkdtempSync(path.join(os.tmpdir(), 'jqml-twice-'))
            let log = console.log
            console.log = () => {}
            try {
                compile({ mode: 'js', config: app.config, entry: app.entry, output: out, name: 'index', root: app.root })
            } finally {
                console.log = log
            }
            outputs.push(fs.readFileSync(path.join(out, 'index.js')))
            fs.rmSync(out, { recursive: true, force: true })
        }
        assert.ok(outputs[0].equals(outputs[1]), app.name + ' differs between two compiles')
    }
})

test('every real application minified with the faster mark_enclosed is the same, byte for byte', { timeout: 900000 }, () => {
    for(let app of available()){
        let outputs = []
        for(let original of [true, false]){
            if(original) process.env.JQML_UGLIFY_ORIGINAL = '1'
            let out = fs.mkdtempSync(path.join(os.tmpdir(), 'jqml-minify-'))
            let log = console.log
            console.log = () => {}
            try {
                compile({ mode: 'js', config: app.config, entry: app.entry, output: out, name: 'index', root: app.root, minify: true })
            } finally {
                console.log = log
                delete process.env.JQML_UGLIFY_ORIGINAL
            }
            outputs.push(fs.readFileSync(path.join(out, 'index.js')))
            fs.rmSync(out, { recursive: true, force: true })
        }
        assert.ok(outputs[0].equals(outputs[1]), app.name + ': the minified output differs')
    }
})
