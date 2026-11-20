// The queues of the update phase (Qt.callLater, polish, deleteLater, timers) given work from inside their own run:
// nothing is lost, nothing waits for an unrelated event, nothing loops for ever.
const test = require('node:test')
const assert = require('node:assert/strict')
const { cores, run } = require('./stress')

const tick = () => new Promise((resolve) => setTimeout(resolve, 0))

for(let [name, core] of cores()){
    test(name + ': Qt.callLater called from Component.onDestruction of an object deleted later runs without another event', async () => {
        let { JQ, root, errors } = run(`import QtQuick 2.15
Item {
    id: root
    property int called: 0
    Component { id: c; Item { Component.onDestruction: Qt.callLater(function(){ root.called++ }) } }
    function make(){ return c.createObject(root) }
}`, core)
        let obj = root.make()
        JQ.batch(() => obj.destroy())
        await tick()
        await tick()
        assert.equal(root.called, 1)
        assert.deepEqual(errors, [])
    })

    test(name + ': an object destroyed while the deferred deletes run is deleted in the same update', async () => {
        let { JQ, root, errors } = run(`import QtQuick 2.15
Item {
    id: root
    property var second: null
    property bool secondGone: false
    Component { id: c; Item { property var partner: null; Component.onDestruction: if(partner) partner.destroy() } }
    Component { id: d; Item { Component.onDestruction: root.secondGone = true } }
    function make(){ let a = c.createObject(root); a.partner = d.createObject(root); return a }
}`, core)
        let a = root.make()
        JQ.batch(() => a.destroy())
        await tick()
        await tick()
        assert.equal(root.secondGone, true)
        assert.deepEqual(errors, [])
    })

    test(name + ': callLater chains of 10000, callLater spawning callLater', async () => {
        let { JQ, root } = run(`import QtQuick 2.15
Item {
    id: root
    property int n: 0
    function step(){ n++; if(n < 10000) Qt.callLater(step) }
}`, core)
        JQ.batch(() => root.step())
        for(let i = 0; i < 5 && root.n < 10000; i++) await tick()
        assert.equal(root.n, 10000)
    })

    test(name + ': 10000 timers of interval 0 to 50, timers that start and stop each other', { timeout: 60000 }, async () => {
        let { JQ, root, errors } = run(`import QtQuick 2.15
Item {
    id: root
    property int fired: 0
    property int pingPong: 0
    Component { id: c; Timer { interval: Math.floor(Math.random() * 50); running: true; onTriggered: { root.fired++; destroy() } } }
    Timer { id: ping; interval: 1; onTriggered: { root.pingPong++; if(root.pingPong < 200) pong.start() } }
    Timer { id: pong; interval: 0; onTriggered: { root.pingPong++; ping.restart() } }
    function make(n){ for(let i = 0; i < n; i++) c.createObject(root); ping.start() }
}`, core)
        JQ.batch(() => root.make(10000))
        let start = Date.now()
        while((root.fired < 10000 || root.pingPong < 200) && Date.now() - start < 30000){
            await new Promise((resolve) => setTimeout(resolve, 20))
            JQ.engine.flushFrame()
        }
        assert.equal(root.fired, 10000)
        assert.ok(root.pingPong >= 200)
        assert.deepEqual(errors, [])
    })

    test(name + ': a polish that changes geometry polishes again in the same frame, a positioner inside a positioner', () => {
        let { JQ, root } = run(`import QtQuick 2.15
Item {
    id: root
    property int h: 10
    property alias outer: outer
    Column {
        id: outer
        Repeater {
            model: 50
            Row {
                Repeater { model: 5; Rectangle { width: 5; height: root.h + index } }
            }
        }
    }
}`, core)
        JQ.engine.flushFrame()
        assert.equal(root.outer.height, 50 * 14)
        JQ.batch(() => { root.h = 20 })
        JQ.engine.flushFrame()
        assert.equal(root.outer.height, 50 * 24)
    })
}
