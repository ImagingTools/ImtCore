// Run: node --test tests/unit
// What the update phase and the destruction of objects leave behind: work queued while the queues run, the
// subscriptions of a released Loader item, the count of live subscriptions.
const test = require('node:test')
const assert = require('node:assert/strict')
const { runtimes } = require('./runtimes')

function cases(name, runtime){
    const { engine, Item, types } = runtime
    const { QtObject, Component, Loader } = types

    // QQuickLoaderPrivate::clear removes the item change listener: the Loader lets go of the item it releases
    test(name + ': a Loader of explicit size lets go of the item it releases', () => {
        let root, loader, made = []
        engine.batch(() => {
            root = new Item(null)
            loader = new Loader(root)
            loader.width = 100
            loader.height = 50
            loader.sourceComponent = new Component(null, (parent) => {
                let item = new Item(parent)
                made.push(item)
                return item
            })
        })
        engine.batch(() => engine.scheduleComponentComplete(loader))
        let first = loader.item
        assert.equal(first.width, 100)
        engine.batch(() => { loader.active = false })
        engine.batch(() => { loader.width = 60 })
        assert.equal(first.width, 100)
        engine.batch(() => { loader.active = true })
        assert.equal(loader.item.width, 60)
        assert.equal(engine.hasSubscribers(loader, Item.SIGNAL_widthChanged), true)
    })

    // QAbstractItemModel::beginInsertRows of any count: the rows are not passed to a function as its arguments, which
    // overflows the call stack past about 100000 of them
    test(name + ': 150000 rows inserted, appended and moved at once in a ListModel shown by a ListView', () => {
        const { ListModel, ListView } = types
        let root, model, view
        engine.batch(() => {
            root = new Item(null)
            model = new ListModel(root)
            view = new ListView(root)
            view.width = 100
            view.height = 100
            view.delegate = new Component(null, (parent) => {
                let item = new Item(parent)
                item.height = 10
                return item
            })
            view.model = model
        })
        engine.batch(() => engine.scheduleComponentComplete(view))
        let rows = Array.from({ length: 150000 }, (_, i) => ({ n: i }))
        engine.batch(() => model.append(rows))
        engine.batch(() => model.insert(1, rows))
        assert.equal(model.count, 300000)
        assert.equal(view.count, 300000)
        // [r0, s0 .. s149999, r1 .. r149999]: the first 150000 rows go after the next one
        engine.batch(() => model.move(0, 1, 150000))
        assert.equal(model.get(0).n, 149999)
        assert.equal(model.get(1).n, 0)
        assert.equal(model.get(2).n, 0)
        engine.batch(() => view.forceLayout())
        assert.equal(view.itemAtIndex(0).y, 0)
    })

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
