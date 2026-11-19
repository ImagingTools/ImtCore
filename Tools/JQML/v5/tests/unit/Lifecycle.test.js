// Run: node --test tests/unit
// What the update phase and the destruction of objects leave behind: work queued while the queues run, the
// subscriptions of a released Loader item, the count of live subscriptions.
const test = require('node:test')
const assert = require('node:assert/strict')
const { runtimes } = require('./runtimes')

function cases(name, runtime){
    const { engine, Item, types } = runtime
    const { QtObject, Component, Loader } = types



}

for(let [name, load] of runtimes) cases(name, load())

// ---- compiled documents, on the sources and on dist/core.js ----

const { compileQml, load } = require('../bench/lib')
const { sourcesBundle } = require('../sourcesBundle')
const fs = require('fs')
const os = require('os')
const path = require('path')

const sourcesCore = path.join(fs.mkdtempSync(path.join(os.tmpdir(), 'jqml-lifecycle-')), 'sources.js')
fs.writeFileSync(sourcesCore, sourcesBundle())

const DELETES = compileQml({ 'Main.qml': `import QtQuick 2.15
Item {
    id: root
    property int called: 0
    property bool secondGone: false
    Component { id: first; Item { property var partner: null; Component.onDestruction: { Qt.callLater(function(){ root.called++ }); partner.destroy() } } }
    Component { id: second; Item { Component.onDestruction: root.secondGone = true } }
    function make(){ let a = first.createObject(root); a.partner = second.createObject(root); return a }
}` })

for(let [name, core] of [['Runtime', sourcesCore], ['dist/core.js', require('../bench/lib').CORE]]){
    // QObject::deleteLater and QMetaObject::invokeMethod(Qt::QueuedConnection) from ~QObject: the event loop takes both at
    // once; here no other event may come to run them
    test(name + ': a call and a delete queued by Component.onDestruction of a deleted object run in the same update', () => {
        let { JQ, root, dispose } = load(DELETES, { core })
        let a = root.make()
        JQ.batch(() => a.destroy())
        assert.equal(root.called, 1)
        assert.equal(root.secondGone, true)
        dispose()
    })
}
