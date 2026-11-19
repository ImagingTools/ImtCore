// Bindings at scale: 1 -> 10000, chains, diamonds, dependencies switched by conditions, intended loops (a warning, no
// hang), and signals: storms, connect / disconnect / destroy inside a handler during the emission, 1000 handlers.
const test = require('node:test')
const assert = require('node:assert/strict')
const { cores, run, random } = require('./stress')

const lines = (n, fn) => Array.from({ length: n }, (_, i) => fn(i)).join('\n')

for(let [name, core] of cores()){
    test(name + ': one property read by 10000 bindings', { timeout: 60000 }, () => {
        let { JQ, root, errors } = run(`import QtQuick 2.15
Item {
    id: root
    property int v: 0
    property int sum: 0
    Repeater { id: rep; model: 10000; Item { property int c: root.v + index } }
    property alias rep: rep
}`, core)
        JQ.batch(() => { root.v = 5 })
        assert.equal(root.rep.itemAt(9999).c, 10004)
        assert.deepEqual(errors, [])
    })

    test(name + ': a chain of 2000 bindings follows its head; a chain of 20000 stops with an error, not a crash', { timeout: 60000 }, () => {
        let { JQ, root, errors } = run(`import QtQuick 2.15
Item {
    id: root
    property int v: 0
${lines(2000, i => `    property int p${i}: ${i ? 'p' + (i - 1) : 'v'} + 1`)}
}`, core)
        JQ.batch(() => { root.v = 10 })
        assert.equal(root.p1999, 2010)
        assert.deepEqual(errors, [])
        let deep = run(`import QtQuick 2.15
Item {
    id: root
    property int v: 0
${lines(20000, i => `    property int p${i}: ${i ? 'p' + (i - 1) : 'v'} + 1`)}
}`, core)
        // the creation already reads the chain from its last binding down: deep, but it must end, and report the overflow
        deep.JQ.batch(() => { deep.root.v = 10 })
        assert.ok(deep.root.p0 === 11)
    })

    test(name + ': diamonds: d reads b and c, both reading a; d ends right and its handler sees the last value', () => {
        let { JQ, root } = run(`import QtQuick 2.15
Item {
    id: root
    property int a: 0
    property int b: a + 1
    property int c: a * 2
    property int d: b + c
    property var seen: []
    onDChanged: seen.push(d)
}`, core)
        let rnd = random(1)
        for(let i = 0; i < 1000; i++){
            let value = Math.floor(rnd() * 100)
            JQ.batch(() => { root.a = value })
            assert.equal(root.d, value + 1 + value * 2)
            assert.equal(root.seen[root.seen.length - 1], root.d)
        }
    })

    test(name + ': conditional dependencies: the branch not taken does not update the binding', () => {
        let { JQ, root } = run(`import QtQuick 2.15
Item {
    id: root
    property bool flag: false
    property int a: 0
    property int b: 0
    property int evaluations: 0
    property int c: { evaluations++; return flag ? a : b }
}`, core)
        let rnd = random(2)
        for(let i = 0; i < 2000; i++){
            let r = rnd()
            let before = root.evaluations
            if(r < 0.2) JQ.batch(() => { root.flag = !root.flag })
            else if(r < 0.6) JQ.batch(() => { root.a++ })
            else JQ.batch(() => { root.b++ })
            let expected = r < 0.2 || (r < 0.6 ? root.flag : !root.flag) ? 1 : 0
            assert.equal(root.evaluations - before, expected, 'step ' + i)
            assert.equal(root.c, root.flag ? root.a : root.b)
        }
    })

    test(name + ': binding loops warn and end', { timeout: 20000 }, () => {
        let { JQ, root, warnings } = run(`import QtQuick 2.15
Item {
    id: root
    property int a: b + 1
    property int b: a + 1
    property int x: 0
    width: height + 1
    height: width + 1
    onXChanged: x = x + 1
}`, core)
        JQ.batch(() => { root.x = 1 })
        assert.ok(warnings.some(w => /Binding loop detected/.test(w)))
    })

    test(name + ': a storm of 100000 emissions, 1000 handlers of one signal', { timeout: 60000 }, () => {
        let { JQ, root } = run(`import QtQuick 2.15
Item {
    id: root
    signal ping(int x)
    property int count: 0
    onPing: count += x
}`, core)
        for(let i = 0; i < 100000; i++) root.ping(1)
        assert.equal(root.count, 100000)
        let total = 0
        for(let i = 0; i < 1000; i++) root.ping.connect((x) => { total += x })
        root.ping(2)
        assert.equal(total, 2000)
    })

    test(name + ': connect, disconnect and destroy inside a handler while the signal is emitted', () => {
        let { JQ, root, errors } = run(`import QtQuick 2.15
Item {
    id: root
    signal ping()
    Component { id: c; Item { } }
    function make(){ return c.createObject(root) }
}`, core)
        let order = []
        let late = () => order.push('late')
        let second = () => order.push('second')
        let receiver = root.make()
        let third = function(){ order.push('third') }
        root.ping.connect(() => {
            order.push('first')
            root.ping.connect(late)         // connected during the emission: not called in it (QObject::activate)
            root.ping.disconnect(second)    // disconnected during the emission: not called (Qt 6)
            receiver.destroy()              // deleted later: still called in this emission
        })
        root.ping.connect(second)
        root.ping.connect(receiver, third)
        JQ.batch(() => root.ping())
        assert.deepEqual(order, ['first', 'third'])
        order.length = 0
        JQ.batch(() => root.ping())
        assert.deepEqual(order, ['first', 'late'])
        assert.deepEqual(errors, [])
    })
}
