// Run: node --test tests/unit
// A QML enum is a set of integers, as in Qt: a value without "= n" follows the one before it, the first one is 0.
// DocumentRepresentationController.ChangeSource (Own, DocumentView, External) declares no values at all.
const test = require('node:test')
const assert = require('node:assert/strict')
const fs = require('fs')
const os = require('os')
const path = require('path')
const { compile } = require('../../compiler/index')

function bundle(files, entry){
    let root = fs.mkdtempSync(path.join(os.tmpdir(), 'jqml-enum-'))
    try {
        for(let [name, text] of Object.entries(files)){
            let fileName = path.join(root, name)
            fs.mkdirSync(path.dirname(fileName), { recursive: true })
            fs.writeFileSync(fileName, text)
        }
        let output = path.join(root, 'out')
        compile({ mode: 'js', entry: path.join(root, entry), output, name: 'index' })
        return fs.readFileSync(path.join(output, 'index.js'), 'utf8')
    } finally {
        fs.rmSync(root, { recursive: true, force: true })
    }
}

// The values the bundle gives the enum `name`
function enumValues(code, name){
    let match = new RegExp('\\.' + name + '=(\\{[^}]*\\})').exec(code)
    assert.ok(match, 'the bundle defines the enum ' + name)

    return JSON.parse(match[1])
}

function holder(enumText){
    return {
        'app/Main.qml': 'import QtQuick 2.15\nItem {\n    Holder {}\n}\n',
        'app/Holder.qml': 'import QtQuick 2.15\nItem {\n' + enumText + '\n}\n',
    }
}

test('values without "= n" count from 0', () => {
    let code = bundle(holder('    enum ChangeSource {\n        Own,\n        DocumentView,\n        External\n    }'), 'app/Main.qml')
    assert.deepEqual(enumValues(code, 'ChangeSource'), { Own: 0, DocumentView: 1, External: 2 })
})

test('a value without "= n" follows the value before it', () => {
    let code = bundle(holder('    enum Level { Low = 5, Middle, High = 10, Top }'), 'app/Main.qml')
    assert.deepEqual(enumValues(code, 'Level'), { Low: 5, Middle: 6, High: 10, Top: 11 })
})

test('negative values are read', () => {
    let code = bundle(holder('    enum Sign { Minus = -1, Zero, Plus }'), 'app/Main.qml')
    assert.deepEqual(enumValues(code, 'Sign'), { Minus: -1, Zero: 0, Plus: 1 })
})
