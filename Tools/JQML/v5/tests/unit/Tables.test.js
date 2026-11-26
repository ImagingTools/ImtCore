// Run: node --test tests/unit
// TableView with the pointer (a tap makes a cell current), SelectionRectangle (a drag selects, a tap ends it, a handle
// is made with its attached control set before its bindings run, a disabled control lets go) and the QtTest functions
// that wait (an async test function awaits them). Compiled documents, on the sources and on dist/core.js.
const test = require('node:test')
const assert = require('node:assert/strict')
const fs = require('fs')
const os = require('os')
const path = require('path')
const { compileQml, load, CORE } = require('../bench/lib')
const { sourcesBundle } = require('../sourcesBundle')

const sourcesCore = path.join(fs.mkdtempSync(path.join(os.tmpdir(), 'jqml-tables-')), 'sources.js')
fs.writeFileSync(sourcesCore, sourcesBundle())

const TABLE = compileQml({ 'Main.qml': `import QtQuick
import QtQuick.Controls
import Qt.labs.qmlmodels
Item {
    id: root
    width: 400
    height: 300
    property alias view: tableView
    property alias control: selectionRectangle
    property var handles: []
    property bool bound: false
    TableModel {
        id: tableModel
        TableModelColumn { display: "a" }
        TableModelColumn { display: "b" }
        TableModelColumn { display: "c" }
        TableModelColumn { display: "d" }
        rows: {
            let rows = []
            for (let i = 0; i < 20; i++) rows.push({ a: i, b: i, c: i, d: i })
            return rows
        }
    }
    TableView {
        id: tableView
        width: 240
        height: 150
        interactive: false
        model: tableModel
        selectionModel: ItemSelectionModel {}
        delegate: Rectangle { implicitWidth: 60; implicitHeight: 30 }
    }
    SelectionRectangle {
        id: selectionRectangle
        target: tableView
        topLeftHandle: Rectangle {
            id: handle
            width: 10
            height: 10
            visible: SelectionRectangle.control.active
            Component.onCompleted: { root.handles.push(handle); root.bound = SelectionRectangle.control === selectionRectangle }
        }
    }
    SelectionRectangle {
        id: unsupported
        property Item plain: Item {}
        objectName: "unsupported"
    }
}` }, { allowWarnings: true })

const WAITS = compileQml({ 'Main.qml': `import QtQuick
import QtTest
Item {
    id: root
    property var log: []
    property bool fired: false
    Timer { id: timer; interval: 20; onTriggered: root.fired = true }
    TestCase {
        name: "Waits"
        function settle(ms) {
            wait(ms)
            root.log.push("settled " + root.fired)
        }
        function test_a_wait() {
            timer.start()
            root.log.push("before " + root.fired)
            wait(60)
            root.log.push("after " + root.fired)
        }
        function test_b_helper() {
            root.fired = false
            timer.start()
            settle(60)
            root.log.push("helper done")
        }
        function test_c_nested() {
            let values = [1, 2].map(v => v * 2)
            root.log.push("nested " + values.join(","))
        }
        function test_d_tryCompare() {
            root.fired = false
            timer.start()
            tryCompare(root, "fired", true, 1000)
            root.log.push("compared " + root.fired)
        }
    }
}` })

const DOUBLE = compileQml({ 'Main.qml': `import QtQuick
import QtQml.Models
import Qt.labs.qmlmodels
Item {
    id: root
    width: 400
    height: 300
    property alias view: tableView
    property var log: []
    TableModel {
        id: tableModel
        TableModelColumn { display: "a"; edit: "a" }
        TableModelColumn { display: "b"; edit: "b" }
        rows: [ { a: "a0", b: "b0" }, { a: "a1", b: "b1" }, { a: "a2", b: "b2" } ]
    }
    property alias model: tableModel
    TableView {
        id: tableView
        width: 300
        height: 200
        interactive: false
        model: tableModel
        selectionModel: ItemSelectionModel {}
        resizableColumns: true
        editTriggers: TableView.DoubleTapped
        delegate: Rectangle {
            id: cell
            required property int row
            required property int column
            required property var display
            implicitWidth: 60
            implicitHeight: 30
            function write(value) { cell.display = value }
            function writeModel(value) { model.display = value }
            TableView.editDelegate: Item {
                Component.onCompleted: root.log.push("editor " + cell.row + "," + cell.column)
            }
        }
    }
}` })

const tick = (ms) => new Promise((resolve) => setTimeout(resolve, ms))

for(let [name, core] of [['Runtime', sourcesCore], ['dist/core.js', CORE]]){
    // the view of the document in the pointer scene, the mouse in its coordinates
    function open(){
        let warnings = []
        let errors = []
        let consoleOf = { log(){}, info(){}, debug(){}, warn: (...a) => warnings.push(a.join(' ')), error: (...a) => errors.push(a.join(' ')) }
        let loaded = load(TABLE, { core, console: consoleOf })
        let { JQ, root } = loaded
        JQ.mouse.scene.roots = [root]
        let mouse = {
            press: (x, y, modifiers = 0) => JQ.mouse.press(x, y, 1, 1, modifiers, false),
            move: (x, y, modifiers = 0) => JQ.mouse.move(x, y, 1, modifiers),
            release: (x, y, modifiers = 0) => JQ.mouse.release(x, y, 1, 0, modifiers),
        }
        let cells = () => root.view.selectionModel.selectedIndexes.map(i => i.row + ',' + i.column).sort().join(' ')
        let current = () => { let i = root.view.selectionModel.currentIndex; return i.row + ',' + i.column }
        let close = () => { JQ.mouse.scene.roots = []; loaded.dispose() }
        return Object.assign(loaded, { mouse, cells, current, warnings, errors, close })
    }

    // QQuickTableViewPrivate::handleTap: a left click without modifiers makes the cell current (the press carries the
    // buttons and the modifiers apart)
    test(name + ': a click on a TableView makes the cell under it current', () => {
        let t = open()
        t.mouse.press(130, 40)
        t.mouse.release(130, 40)
        assert.equal(t.current(), '1,2')
        t.close()
    })

    // QQuickSelectionRectangle: on a view that does not flick a drag selects; the handle is made at the first selection
    // with SelectionRectangle.control set before its bindings run; a tap clears the selection and ends it
    test(name + ': SelectionRectangle selects by a drag and a tap ends the selection', () => {
        let t = open()
        let control = t.root.control
        assert.equal(control.active, false)
        assert.equal(t.root.handles.length, 0)
        t.mouse.press(30, 15)
        t.mouse.move(45, 30)
        assert.equal(control.active, true)
        assert.equal(control.dragging, true)
        t.mouse.move(150, 75)
        t.mouse.release(150, 75)
        assert.equal(t.cells(), '0,0 0,1 0,2 1,0 1,1 1,2 2,0 2,1 2,2')
        assert.equal(control.dragging, false)
        assert.equal(t.root.handles.length, 1)
        assert.equal(t.root.bound, true)
        let handle = t.root.handles[0]
        assert.equal(handle.parent, t.root.view.contentItem)
        assert.equal(handle.z, 100)
        assert.equal(handle.visible, true)
        assert.deepEqual([handle.x, handle.y], [-5, -5])
        t.mouse.press(200, 120)
        t.mouse.release(200, 120)
        assert.equal(t.cells(), '')
        assert.equal(control.active, false)
        assert.equal(handle.visible, false)
        assert.deepEqual(t.errors, [])
        t.close()
    })

    // the enabledChanged connection of QQuickSelectionRectangle: the selection is no longer active, a drag does not select
    test(name + ': a disabled SelectionRectangle ends its selection and does not select', () => {
        let t = open()
        let control = t.root.control
        t.mouse.press(30, 15)
        t.mouse.move(45, 30)
        t.mouse.move(100, 50)
        t.mouse.release(100, 50)
        assert.equal(control.active, true)
        t.JQ.batch(() => { control.enabled = false })
        assert.equal(control.active, false)
        t.mouse.press(200, 120)
        t.mouse.release(200, 120)
        t.mouse.press(30, 15)
        t.mouse.move(45, 30)
        t.mouse.move(100, 100)
        t.mouse.release(100, 100)
        assert.equal(control.active, false)
        assert.equal(t.cells(), '')
        t.close()
    })

    // QQuickSelectionRectangle::setTarget: an item that is not selectable is warned about
    test(name + ': SelectionRectangle warns about a target it cannot select in', () => {
        let t = open()
        let other = t.root.children.find(c => c.objectName === 'unsupported')
        t.JQ.batch(() => { other.target = other.plain })
        assert.ok(t.warnings.some(w => /the assigned target is not supported by the control/.test(w)))
        t.close()
    })

    function openDouble(){
        let loaded = load(DOUBLE, { core })
        let { JQ, root } = loaded
        JQ.mouse.scene.roots = [root]
        let click = (x, y, double = false) => {
            JQ.mouse.press(x, y, 1, 1, 0, double)
            JQ.mouse.release(x, y, 1, 0, 0)
        }
        let hover = (x, y) => JQ.mouse.move(x, y, 0, 0)
        let close = () => { JQ.mouse.scene.roots = []; loaded.dispose() }
        return Object.assign(loaded, { click, hover, close })
    }

    // QQuickTableViewPrivate tapHandler doubleTapped: on the grid line the hover handler is over, back to the implicit
    // size (setColumnWidth(column, -1)); a drag on it resizes first
    test(name + ': a double tap on a hovered grid line resets the column to its implicit width', () => {
        let t = openDouble()
        let view = t.root.view
        t.hover(60, 15)
        t.JQ.mouse.press(60, 15, 1, 1, 0, false)
        t.JQ.mouse.move(75, 15, 1, 0)
        t.JQ.mouse.move(90, 15, 1, 0)
        t.JQ.mouse.release(90, 15, 1, 0, 0)
        // the drag starts where it passes the threshold (m_columnStartX): 60 + 90 - 75
        assert.equal(view.explicitColumnWidth(0), 75)
        t.JQ.batch(() => view.forceLayout())
        t.hover(75, 15)
        t.click(75, 15)
        t.click(75, 15, true)
        assert.equal(view.explicitColumnWidth(0), -1)
        assert.deepEqual(JSON.parse(JSON.stringify(t.root.log)), [])
        t.close()
    })

    // editTriggers DoubleTapped: the second tap within the double click interval opens the editor of the cell
    test(name + ': a double tap on a cell edits it with the DoubleTapped trigger, a single tap does not', () => {
        let t = openDouble()
        t.click(100, 45)
        assert.deepEqual(JSON.parse(JSON.stringify(t.root.log)), [])
        t.click(100, 45, true)
        assert.deepEqual(JSON.parse(JSON.stringify(t.root.log)), ['editor 1,1'])
        t.close()
    })

    // TableView.delegateModelAccess: ReadOnly makes the roles of the model object read-only, ReadWrite writes a required
    // property of a role back to the model; a new access takes effect with the next layout
    test(name + ': delegateModelAccess ReadOnly and ReadWrite', () => {
        let t = openDouble()
        let view = t.root.view
        let model = t.root.model
        let cell = (row) => view.itemAtCell({ x: 0, y: row })
        t.JQ.batch(() => cell(0).write('required'))
        assert.equal(model.rows[0].a, 'a0')
        t.JQ.batch(() => cell(0).writeModel('through model'))
        assert.equal(model.rows[0].a, 'through model')
        t.JQ.batch(() => { view.delegateModelAccess = 2 })
        t.JQ.batch(() => cell(1).write('not yet'))
        assert.equal(model.rows[1].a, 'a1')
        t.JQ.batch(() => view.forceLayout())
        t.JQ.batch(() => cell(2).write('read write'))
        assert.equal(model.rows[2].a, 'read write')
        t.JQ.batch(() => { view.delegateModelAccess = 1 })
        t.JQ.batch(() => view.forceLayout())
        assert.throws(() => t.JQ.batch(() => cell(1).writeModel('read only')), /read-only/)
        assert.equal(model.rows[1].a, 'a1')
        t.close()
    })

    // QtTest: wait(), tryCompare() and a method that waits block the test function while the timers run
    test(name + ': a TestCase function waits in wait(), tryCompare() and a method of its own that waits', async () => {
        let { JQ, root, dispose } = load(WAITS, { core })
        let testCase = root.children.find(c => c instanceof JQ.types.TestCase)
        for(let i = 0; i < 400 && !testCase.completed; i++) await tick(5)
        assert.equal(testCase.completed, true)
        assert.deepEqual(JSON.parse(JSON.stringify(root.log)), ['before false', 'after true', 'settled true', 'helper done', 'nested 2,4', 'compared true'])
        dispose()
    })
}

test('compiler: the test functions that wait are async and await their waiting calls, nested functions stay as they are', () => {
    let code = WAITS
    assert.match(code, /"test_a_wait",\(async function\(/)
    assert.match(code, /"settle",\(async function\(/)
    assert.match(code, /"test_b_helper",\(async function\(/)
    assert.match(code, /"test_c_nested",\(function\(/)
    assert.match(code, /await \$self\.wait\(60\)/)
    assert.match(code, /await \$self\.settle\(60\)/)
})
