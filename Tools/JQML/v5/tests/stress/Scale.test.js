// Scale: 100000 items, nesting 1000 deep, 10000 children of one parent; views of 100000 rows scrolled fast while their
// model changes; 10000 pointer moves, grabs and cancels over nested Flickable and MouseArea, the focused item destroyed.
const test = require('node:test')
const assert = require('node:assert/strict')
const { cores, run, random } = require('./stress')

const nested = (depth, inner) => Array.from({ length: depth }, () => 'Item { width: 10; height: 10; ').join('') + inner + ' }'.repeat(depth)

for(let [name, core] of cores()){
    test(name + ': 100000 items made and destroyed', { timeout: 120000 }, () => {
        let { JQ, root, errors } = run(`import QtQuick 2.15
Item {
    id: root
    property int n: 0
    Repeater { model: root.n; Item { width: 1; height: 1 } }
}`, core)
        let start = Date.now()
        JQ.batch(() => { root.n = 100000 })
        JQ.engine.flushFrame()
        assert.equal(root.children.length, 100001)
        JQ.batch(() => { root.n = 0 })
        JQ.engine.flushFrame()
        assert.equal(root.children.length, 1)
        assert.ok(Date.now() - start < 60000)
        assert.deepEqual(errors, [])
    })

    test(name + ': items nested 1000 deep: bindings through the parents, visibility, destruction', { timeout: 60000 }, () => {
        let { JQ, root, errors } = run(`import QtQuick 2.15
Item {
    id: root
    property bool shown: true
    property alias outer: outer
    Item { id: outer; visible: root.shown; ${nested(1000, 'Item { objectName: "leaf"; width: parent.width + 1 }')} }
}`, core)
        let leaf = root.outer
        while(leaf.children.length) leaf = leaf.children[0]
        assert.equal(leaf.objectName, 'leaf')
        assert.equal(leaf.width, 11)
        JQ.batch(() => { root.shown = false })
        assert.equal(leaf.visible, false)
        JQ.batch(() => { root.shown = true })
        assert.equal(leaf.visible, true)
        JQ.batch(() => root.outer.destroy())
        assert.deepEqual(errors, [])
    })

    test(name + ': 10000 children of one parent made, reordered by z and destroyed', { timeout: 60000 }, () => {
        let { JQ, root, errors } = run(`import QtQuick 2.15
Item {
    id: root
    property int n: 0
    property alias box: box
    Item { id: box; Repeater { model: root.n; Item { z: index % 5; width: 1; height: 1 } } }
}`, core)
        let start = Date.now()
        JQ.batch(() => { root.n = 10000 })
        JQ.engine.flushFrame()
        assert.equal(root.box.children.length, 10001)
        JQ.batch(() => { root.n = 0 })
        JQ.engine.flushFrame()
        assert.equal(root.box.children.length, 1)
        assert.ok(Date.now() - start < 30000, 'took ' + (Date.now() - start) + ' ms')
        assert.deepEqual(errors, [])
    })

    test(name + ': ListView of 100000 rows scrolled fast while the model changes; nested lists', { timeout: 120000 }, () => {
        let { JQ, root, errors } = run(`import QtQuick 2.15
Item {
    id: root
    width: 400; height: 600
    property alias view: list
    property alias model: listModel
    ListModel { id: listModel }
    ListView {
        id: list
        anchors.fill: parent
        model: listModel
        delegate: Column {
            width: 400
            Text { text: model.title + " " + index }
            ListView { width: 400; height: 40; orientation: ListView.Horizontal; model: 5; delegate: Rectangle { width: 30; height: 40 } }
        }
    }
}`, core)
        let rows = []
        for(let i = 0; i < 100000; i++) rows.push({ title: 't' + i })
        JQ.batch(() => { for(let row of rows) root.model.append(row) })
        JQ.engine.flushFrame()
        let rnd = random(3)
        let view = root.view
        for(let i = 0; i < 300; i++){
            let r = rnd()
            JQ.batch(() => { view.contentY = Math.floor(rnd() * Math.max(0, view.contentHeight - view.height)) })
            if(r < 0.1) JQ.batch(() => root.model.insert(Math.floor(rnd() * root.model.count), { title: 'new' }))
            else if(r < 0.2) JQ.batch(() => root.model.remove(Math.floor(rnd() * (root.model.count - 10)), 5))
            else if(r < 0.25) JQ.batch(() => root.model.move(Math.floor(rnd() * 1000), Math.floor(rnd() * 1000), 3))
            JQ.engine.flushFrame()
            let first = view.indexAt(10, view.contentY + 1)
            assert.ok(first >= 0, 'no row at the top after step ' + i)
        }
        JQ.batch(() => root.model.clear())
        JQ.engine.flushFrame()
        assert.equal(view.count, 0)
        assert.deepEqual(errors, [])
    })

    test(name + ': 10000 pointer moves, presses, grabs and cancels over nested Flickables and MouseAreas', { timeout: 120000 }, () => {
        let { JQ, root, errors } = run(`import QtQuick 2.15
Item {
    id: root
    width: 400; height: 400
    property int clicks: 0
    property int entered: 0
    property int canceled: 0
    Flickable {
        anchors.fill: parent
        contentHeight: 2000
        Flickable {
            width: 400; height: 300
            contentWidth: 2000
            flickableDirection: Flickable.HorizontalFlick
            Repeater {
                model: 20
                MouseArea {
                    x: index * 100; width: 90; height: 90
                    hoverEnabled: true
                    onClicked: root.clicks++
                    onEntered: root.entered++
                    onCanceled: root.canceled++
                }
            }
        }
    }
    TextInput { id: field; y: 350; width: 100; height: 20; focus: true }
    property alias field: field
}`, core)
        let delivery = JQ.mouse
        delivery.scene.roots.length = 0
        delivery.scene.roots.push(root)
        let rnd = random(4)
        let down = false
        for(let i = 0; i < 10000; i++){
            let x = Math.floor(rnd() * 400), y = Math.floor(rnd() * 400)
            let r = rnd()
            if(!down && r < 0.05){ delivery.press(x, y, 1, 1, 0, false, null); down = true }
            else if(down && r < 0.1){ delivery.release(x, y, 1, 0, 0); down = false }
            else if(r < 0.101) delivery.leaveWindow()
            else delivery.move(x, y, down ? 1 : 0, 0)
            if(i % 100 === 0) JQ.engine.flushFrame()
        }
        if(down) delivery.release(0, 0, 1, 0, 0)
        assert.ok(root.entered > 0)
        // the focused item destroyed
        JQ.batch(() => root.field.forceActiveFocus())
        JQ.batch(() => root.field.destroy())
        JQ.engine.flushFrame()
        delivery.move(10, 360, 0, 0)
        delivery.scene.roots.length = 0
        assert.deepEqual(errors, [])
    })
}
