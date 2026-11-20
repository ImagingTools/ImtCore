// Objects created and destroyed many times: once destroyed, nothing of the runtime keeps them (WeakRef), the heap comes
// to a plateau, and the subscriptions of the objects that live on do not grow.
const test = require('node:test')
const assert = require('node:assert/strict')
const { cores, run, collected, heap, linkCount } = require('./stress')

const CYCLES = 1000

// cycles of cycle(i), refs of what each cycle made; the heap of the second half stays within the first's
async function plateau(context, cycle, { cycles = CYCLES, slack = 512 * 1024 } = {}){
    let refs = []
    let half = cycles / 2
    let first = 0
    for(let i = 0; i < cycles; i++){
        let made = cycle(i)
        if(made && i % 50 === 0) refs.push(...[].concat(made).map(obj => new WeakRef(obj)))
        if(i === half) first = heap()
    }
    context.JQ.engine.flushFrame()
    let last = heap()
    assert.ok(last - first < slack, 'heap grew by ' + (last - first) + ' bytes in ' + half + ' cycles')
    assert.ok(await collected(refs), 'destroyed objects are still reachable')
}

const DELEGATES = {
    plain: 'Item { width: 10; height: 10 }',
    binding: 'Item { width: root.v; property int a: root.v + index }',
    handlers: 'Item { property int a: root.v; onAChanged: root.count++; Component.onCompleted: root.count++; Component.onDestruction: root.count++ }',
    anchors: 'Item { anchors.left: parent.left; anchors.right: parent.right; height: 10 }',
    visual: 'Rectangle { width: 10; height: 10; color: "red"; border.width: 1; Text { text: "x" + root.v } MouseArea { anchors.fill: parent; hoverEnabled: true } }',
    connections: 'Item { Connections { target: root; function onVChanged(){ root.count++ } } }',
    states: 'Item { id: d; state: root.v % 2 ? "a" : ""; states: State { name: "a"; PropertyChanges { target: d; width: 5 } } }',
    animation: 'Rectangle { width: 10; height: 10; NumberAnimation on x { from: 0; to: 100; duration: 1000; running: true } }',
    timer: 'Item { Timer { interval: 1000; running: true; repeat: true } }',
}

for(let [name, core] of cores()){
    for(let [kind, delegate] of Object.entries(DELEGATES)){
        test(name + ': Repeater delegates (' + kind + ') created and destroyed ' + CYCLES + ' times', { timeout: 120000 }, async () => {
            let context = run(`import QtQuick 2.15
Item {
    id: root
    property int v: 1
    property int n: 0
    property int count: 0
    property alias rep: repeater
    Repeater { id: repeater; model: root.n; delegate: ${delegate} }
}`, core)
            let { JQ, root } = context
            let links = linkCount(root)
            await plateau(context, (i) => {
                JQ.batch(() => { root.n = 5 })
                JQ.batch(() => { root.v++ })
                let made = [root.rep.itemAt(0), root.rep.itemAt(4)]
                JQ.batch(() => { root.n = 0 })
                return made
            })
            assert.equal(linkCount(root), links)
            assert.deepEqual(context.errors, [])
        })
    }

    test(name + ': a Loader switching its component, also inactive in between, with an explicit size', { timeout: 120000 }, async () => {
        let context = run(`import QtQuick 2.15
Item {
    id: root
    property int v: 1
    property bool on: true
    property bool second: false
    property alias loader: loader
    Component { id: a; Rectangle { width: root.v; height: 10; Text { text: "a" } } }
    Component { id: b; Item { implicitWidth: 20; Rectangle { anchors.fill: parent } } }
    Loader { id: loader; width: 100; height: 50; active: root.on; sourceComponent: root.second ? b : a }
}`, core)
        let { JQ, root } = context
        let loader = root.loader
        let links = linkCount(loader)
        await plateau(context, (i) => {
            let made = loader.item
            if(i % 3 === 0) JQ.batch(() => { root.on = false })
            else JQ.batch(() => { root.second = !root.second })
            JQ.batch(() => { loader.width = 100 + i % 7 })
            JQ.batch(() => { root.on = true })
            return made
        })
        assert.ok(linkCount(loader) <= links + 2)
        assert.deepEqual(context.errors, [])
    })

    test(name + ': a ListView with its model replaced, reset, inserted and removed in', { timeout: 120000 }, async () => {
        let context = run(`import QtQuick 2.15
Item {
    id: root
    width: 300; height: 300
    property alias view: list
    property alias model: listModel
    ListModel { id: listModel }
    ListView {
        id: list
        anchors.fill: parent
        model: listModel
        delegate: Rectangle { width: 300; height: 20; Text { text: model.name + index } }
    }
}`, core)
        let { JQ, root } = context
        await plateau(context, (i) => {
            JQ.batch(() => {
                root.model.clear()
                for(let k = 0; k < 20; k++) root.model.append({ name: 'n' + k })
            })
            JQ.engine.flushFrame()
            let made = root.view.itemAtIndex(3)
            JQ.batch(() => { root.model.insert(2, { name: 'x' }); root.model.remove(5, 3); root.model.move(0, 4, 2) })
            JQ.engine.flushFrame()
            JQ.batch(() => { root.view.model = i % 2 ? 7 : root.model })
            JQ.engine.flushFrame()
            return made
        }, { cycles: 400 })
        assert.deepEqual(context.errors, [])
    })

    test(name + ': objects of Component.createObject destroyed, and Connections changing their target', { timeout: 120000 }, async () => {
        let context = run(`import QtQuick 2.15
Item {
    id: root
    property int v: 1
    property int count: 0
    property var other: null
    Component { id: c; Item { property int w: root.v; signal ping() } }
    Connections { id: conn; target: root.other; function onPing(){ root.count++ } }
    function make(){ return c.createObject(root, { width: 5 }) }
}`, core)
        let { JQ, root } = context
        let links = linkCount(root)
        await plateau(context, () => {
            let a = root.make(), b = root.make()
            JQ.batch(() => { root.other = a })
            a.ping()
            JQ.batch(() => { root.other = b })
            b.ping()
            JQ.batch(() => { root.other = null; a.destroy(); b.destroy() })
            return [a, b]
        })
        assert.equal(root.count, CYCLES * 2)
        assert.equal(linkCount(root), links)
        assert.deepEqual(context.errors, [])
    })
}
