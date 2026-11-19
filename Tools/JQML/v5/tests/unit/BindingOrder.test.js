// Run: node --test tests/unit
// The order in which the bindings of a document are enabled, which orders the notifications later: a binding
// subscribes to what it reads when it is enabled, and a change notifies in the order of subscription
// (QQmlNotifier::emitNotify). QQmlObjectCreator::setupBindings pushes the bindings onto allCreatedBindings in the
// order QmlUnitGenerator writes them - the named values from the last written to the first (QmlIR::Object::
// appendBinding prepends them), the children, the attached and group properties, the bindings to aliases - and
// finalize enables them from the last pushed (Qt 6.8). A binding of a bindable property (Item x, y, width, height)
// is a QQmlPropertyBinding, set after the others in the order made (allQPropertyBindings). The QML is compiled
// and run on the runtime.
const test = require('node:test')
const assert = require('node:assert/strict')
const fs = require('fs')
const os = require('os')
const path = require('path')
const vm = require('vm')
const { compile } = require('../../compiler/index')

function application(main){
    let root = fs.mkdtempSync(path.join(os.tmpdir(), 'jqml-order-'))
    try {
        let entry = path.join(root, 'app', 'Main.qml')
        fs.mkdirSync(path.dirname(entry), { recursive: true })
        fs.writeFileSync(entry, main)
        let output = path.join(root, 'out')
        let result = compile({ mode: 'js', entry, output, name: 'index' })
        assert.deepEqual(result.warnings, [])
        let code = fs.readFileSync(path.join(output, 'index.js'), 'utf8')
        // the application without the runtime, and without the page it would be mounted in
        return code.slice(code.lastIndexOf('\n;(function(){') + 1).replace('content.$mount(document.body)', '')
    } finally {
        fs.rmSync(root, { recursive: true, force: true })
    }
}

// the root object of the document, built as the page builds it
function run(main){
    let loads = []
    let context = { console, setTimeout, clearTimeout, queueMicrotask }
    context.window = context
    vm.createContext(context)
    vm.runInContext(fs.readFileSync(path.resolve(__dirname, '../../dist/core.js'), 'utf8'), context)
    context.addEventListener = (type, fn) => { if(type === 'load') loads.push(fn) }
    context.location = { pathname: '/', origin: '' }
    context.innerWidth = 100
    context.innerHeight = 100
    vm.runInContext(application(main), context)
    loads[0]()
    return { root: context.__root, JQ: context.JQ }
}

function changes(main){
    let { root, JQ } = run(main)
    root.log = []
    JQ.batch(() => { root.v = 5 })
    return Array.from(root.log)
}

test('the own bindings of an object are enabled in the order written', () => {
    assert.deepEqual(changes(`import QtQuick 2.15
Item {
    id: root
    property int v: 1
    property var log: []
    property int a: root.v + 1
    property int b: root.v + 2
    onAChanged: log.push("a")
    onBChanged: log.push("b")
}
`), ['a', 'b'])
})

test('the bindings of the children are enabled before the own ones, the last child first', () => {
    assert.deepEqual(changes(`import QtQuick 2.15
Item {
    id: root
    property int v: 1
    property var log: []
    property int a: root.v + 1
    onAChanged: log.push("own")
    Item { property int c: root.v; onCChanged: root.log.push("first child") }
    Item { property int c: root.v; onCChanged: root.log.push("second child") }
}
`), ['second child', 'first child', 'own'])
})

test('a binding to an alias is enabled before the children', () => {
    assert.deepEqual(changes(`import QtQuick 2.15
Item {
    id: root
    property int v: 1
    property var log: []
    property alias ip: inner.p
    ip: root.v
    Item { id: inner; property int p; onPChanged: root.log.push("alias") }
    Item { property int q: root.v; onQChanged: root.log.push("child") }
}
`), ['alias', 'child'])
})

test('a binding of a bindable property (x) is set after the other bindings', () => {
    assert.deepEqual(changes(`import QtQuick 2.15
Item {
    id: root
    property int v: 1
    property var log: []
    Item {
        x: root.v
        property int p: root.v
        onXChanged: root.log.push("x")
        onPChanged: root.log.push("p")
    }
}
`), ['p', 'x'])
})
