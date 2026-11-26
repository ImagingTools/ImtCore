// Run: node --test tests/unit
// A Window on the page (issue #1178): the page is the window of the platform, so a Window whose document sets no size
// is as large as the page and follows it as the page is resized, as the root of a QQuickView follows its window. A
// width or height the document sets is the size the window asks for (QWindow::resize) and is kept. The QML is
// compiled and run with the bootstrap of the page.
const test = require('node:test')
const assert = require('node:assert/strict')
const fs = require('fs')
const os = require('os')
const path = require('path')
const vm = require('vm')
const { compile } = require('../../compiler/index')

function application(main){
    let root = fs.mkdtempSync(path.join(os.tmpdir(), 'jqml-window-'))
    try {
        let entry = path.join(root, 'app', 'Main.qml')
        fs.mkdirSync(path.dirname(entry), { recursive: true })
        fs.writeFileSync(entry, main)
        let output = path.join(root, 'out')
        let result = compile({ mode: 'js', entry, output, name: 'index' })
        assert.deepEqual(result.warnings, [])
        let code = fs.readFileSync(path.join(output, 'index.js'), 'utf8')
        return code.slice(code.lastIndexOf('\n;(function(){') + 1).replace('content.$mount(document.body)', '')
    } finally {
        fs.rmSync(root, { recursive: true, force: true })
    }
}

// the page: a viewport of 800 x 600 that resize() changes, as the browser does
function run(main){
    let listeners = { load: [], resize: [] }
    let context = { console, setTimeout, clearTimeout, queueMicrotask }
    context.window = context
    vm.createContext(context)
    vm.runInContext(fs.readFileSync(path.resolve(__dirname, '../../dist/core.js'), 'utf8'), context)
    context.addEventListener = (type, fn) => { if(listeners[type]) listeners[type].push(fn) }
    context.location = { pathname: '/', origin: '' }
    context.innerWidth = 800
    context.innerHeight = 600
    vm.runInContext(application(main), context)
    listeners.load[0]()
    let resize = (width, height) => {
        context.innerWidth = width
        context.innerHeight = height
        for(let fn of listeners.resize) fn()
    }
    return { root: context.__root, JQ: context.JQ, resize }
}

const size = (item) => [item.width, item.height]

test('a root Window with no size is as large as the page, and so is its content', () => {
    let { root } = run(`import QtQuick
import QtQuick.Window
Window {
    property alias inner: rect
    Rectangle { id: rect; anchors.fill: parent; color: "red" }
}`)
    assert.deepEqual(size(root), [800, 600])
    assert.deepEqual(size(root.inner), [800, 600])
    assert.deepEqual(size(root.contentItem), [800, 600])
})

test('a root Window follows the page as it is resized', () => {
    let { root, resize } = run(`import QtQuick
import QtQuick.Window
Window {
    property alias inner: rect
    property var seen: []
    onWidthChanged: seen.push(width)
    Rectangle { id: rect; anchors.fill: parent }
}`)
    resize(1024, 700)
    assert.deepEqual(size(root), [1024, 700])
    assert.deepEqual(size(root.inner), [1024, 700])
    assert.deepEqual(Array.from(root.seen), [800, 1024])
})

test('a size the document sets is the size the window asks for, the other one follows the page', () => {
    let { root, resize } = run(`import QtQuick
import QtQuick.Window
Window { width: 320 }`)
    assert.deepEqual(size(root), [320, 600])
    resize(1000, 500)
    assert.deepEqual(size(root), [320, 500])
})

test('a size set later stops the window following the page (QWindow::resize)', () => {
    let { root, resize } = run(`import QtQuick
import QtQuick.Window
Window { function ask(){ width = 400 } }`)
    root.ask()
    resize(1000, 500)
    assert.deepEqual(size(root), [400, 500])
})

test('a Window made with no parent and shown is on the page as large as the page', () => {
    let { root, resize } = run(`import QtQuick
import QtQuick.Window
Item {
    anchors.fill: parent
    property var made: null
    Component { id: comp; Window { visible: false } }
    function make(){ made = comp.createObject(null); made.show() }
}`)
    root.make()
    assert.deepEqual(size(root.made), [800, 600])
    resize(640, 480)
    assert.deepEqual(size(root.made), [640, 480])
})

test('a root Item is not changed: it is sized by its own anchors', () => {
    let { root } = run(`import QtQuick
Item { width: 50; height: 40 }`)
    assert.deepEqual(size(root), [50, 40])
})
