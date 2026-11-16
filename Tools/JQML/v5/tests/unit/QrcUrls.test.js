// Run: node --test tests/unit
// A file of the bundle has the url Qt serves it at: qrc:/qml/ + the path its .qrc declares (the prefix of its
// qresource and its alias). Qt.createComponent resolves a url exactly (Qt 6.8), so a page path such as
// "qrc:/qml/ProLife/Page.qml" finds the file only under that key.
const test = require('node:test')
const assert = require('node:assert/strict')
const fs = require('fs')
const os = require('os')
const path = require('path')
const { compile } = require('../../compiler/index')

function write(root, files){
    for(let [name, text] of Object.entries(files)){
        let fileName = path.join(root, name)
        fs.mkdirSync(path.dirname(fileName), { recursive: true })
        fs.writeFileSync(fileName, text)
    }
}

// The urls of the bundle: the keys the generated code registers its files under
function bundle(files, entry, config){
    let root = fs.mkdtempSync(path.join(os.tmpdir(), 'jqml-qrc-'))
    try {
        write(root, files)
        let output = path.join(root, 'out')
        compile({ mode: 'js', entry: path.join(root, entry), output, name: 'index', config: config ? path.join(root, config) : undefined })
        return fs.readFileSync(path.join(output, 'index.js'), 'utf8')
    } finally {
        fs.rmSync(root, { recursive: true, force: true })
    }
}

const item = 'import QtQuick 2.15\nItem {}\n'
const main = 'import QtQuick 2.15\nItem {\n    Component.onCompleted: Qt.createComponent("qrc:/qml/ProLife/Page.qml")\n}\n'

test('a file of the entry dir gets the url its .qrc alias declares', () => {
    let code = bundle({
        'prolifeqml/Main.qml': main,
        'prolifeqml/Page.qml': item,
        'prolifeqml/prolifeqml.qrc': '<RCC>\n    <qresource prefix="/qml">\n'
            + '        <file alias="ProLife/Main.qml">Main.qml</file>\n'
            + '        <file alias="ProLife/Page.qml">Page.qml</file>\n'
            + '    </qresource>\n</RCC>\n',
    }, 'prolifeqml/Main.qml')
    assert.ok(code.includes('"ProLife/Page.qml"'), 'Page.qml is in the bundle as ProLife/Page.qml')
    assert.ok(code.includes('"ProLife/Main.qml"'), 'Main.qml is in the bundle as ProLife/Main.qml')
    assert.ok(!code.includes('"Page.qml"'), 'no key relative to the entry dir')
})

test('without a .qrc the url stays relative to the entry dir', () => {
    let code = bundle({
        'app/Main.qml': item,
        'app/Page.qml': item,
    }, 'app/Main.qml')
    assert.ok(code.includes('"Page.qml"'))
})

test('a file without an alias gets its path under the prefix', () => {
    let code = bundle({
        'app/Main.qml': item,
        'app/Views/Page.qml': item,
        'app/Views/qmldir': 'Page 1.0 Page.qml\n',
        'app/app.qrc': '<RCC><qresource prefix="/qml/App"><file>Views/Page.qml</file></qresource></RCC>',
        'app/Main2.qml': 'import QtQuick 2.15\nimport "Views"\nItem { Page {} }\n',
    }, 'app/Main2.qml')
    assert.ok(code.includes('"App/Views/Page.qml"'))
})

test('a self-closing qresource and resources outside /qml/ do not give urls', () => {
    let code = bundle({
        'app/Main.qml': item,
        'app/Page.qml': item,
        'app/app.qrc': '<RCC>\n    <qresource prefix="/Icons"/>\n    <qresource prefix="/Icons">\n'
            + '        <file alias="Icons/Page.qml">Page.qml</file>\n    </qresource>\n</RCC>\n',
    }, 'app/Main.qml')
    assert.ok(code.includes('"Page.qml"'), 'the file keeps its url relative to the entry dir')
    assert.ok(!code.includes('"Icons/Page.qml"'))
})

test('a module file keeps its module url when its .qrc declares the same path', () => {
    let code = bundle({
        'app/Main.qml': 'import QtQuick 2.15\nimport mymodule 1.0\nItem { Thing {} }\n',
        'mymodule/qmldir': 'module mymodule\nThing 1.0 Thing.qml\n',
        'mymodule/Thing.qml': item,
        'mymodule/mymodule.qrc': '<RCC><qresource prefix="/qml"><file alias="mymodule/Thing.qml">Thing.qml</file></qresource></RCC>',
        'app/app.json': '{ "dirs": ["../mymodule"] }',
    }, 'app/Main.qml', 'app/app.json')
    assert.ok(code.includes('"mymodule/Thing.qml"'))
})

test('a module file the .qrc puts at the root of qml/ gets that url, as Qt serves it', () => {
    let code = bundle({
        'app/Main.qml': 'import QtQuick 2.15\nimport controls 1.0\nItem { Window {} }\n',
        'controls/qmldir': 'module controls\nWindow 1.0 Base/Window.qml\n',
        'controls/Base/Window.qml': item,
        'controls/controls.qrc': '<RCC><qresource prefix="/qml"><file alias="Window.qml">Base/Window.qml</file></qresource></RCC>',
        'app/app.json': '{ "dirs": ["../controls"] }',
    }, 'app/Main.qml', 'app/app.json')
    assert.ok(code.includes('"Window.qml"'))
    assert.ok(!code.includes('"controls/Base/Window.qml"'))
})
