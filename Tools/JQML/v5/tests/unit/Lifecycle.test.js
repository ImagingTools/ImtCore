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

    // QQuickWindowPrivate::polishItems, PolishLoopDetector: a polish that always polishes again gets a warning after 1000
    // loops and is given up at 10000, the frame is rendered, the item polishes again in the next frame
    test(name + ': a polish loop is given up after 10000 polishes, the frame is rendered, the next frame goes on', () => {
        let looping, other
        engine.batch(() => {
            looping = new Item(null)
            other = new Item(null)
        })
        let polishes = 0
        looping.$polish = function(){
            polishes++
            // the old runtime looped for ever: stop at 20000 so the test ends either way
            if(polishes < 20000) engine.queuePolish(this)
        }
        let warnings = []
        let warn = console.warn
        console.warn = (message) => warnings.push(String(message))
        try {
            engine.batch(() => {
                engine.queuePolish(looping)
                other.x = 5
            })
            engine.flushFrame()
        } finally {
            console.warn = warn
        }
        assert.equal(polishes, 10000)
        assert.equal(warnings.filter(w => /possible QQuickItem::polish\(\) loop/.test(w)).length, 5)
        // the DOM writes of the frame were not held back by the loop
        assert.equal(other.$dirty, 0)
        console.warn = () => {}
        try {
            engine.flushFrame()
        } finally {
            console.warn = warn
        }
        assert.equal(polishes, 20000)
    })

    // QQmlBinding::update captures the properties the expression reads, not those its write reads: a binding whose
    // evaluation reads a property with a pending binding (QQmlData::flushPendingBinding) does not take the reads of
    // that property's setter (the runtime's own code, C++ in Qt) as its dependencies
    test(name + ': a binding takes no dependency from the write of a binding it enables', () => {
        class Probe extends Item {}
        let watched
        runtime.jq.defineType(Probe, { props: { v: { kind: 'real', effect: (probe) => { probe.$peek = watched.height } } } })
        let outer, probe, evaluations = 0
        engine.batch(() => { watched = new Item(null) })
        runtime.jq.create(() => {
            let root = new Item(null)
            probe = new Probe(root)
            probe.$bind(Probe.V, () => 7, null)
            outer = new Item(root)
            // not a bindable property (x, y, width, height bind after the others): enabled while probe's is pending
            outer.$bind(Item.OPACITY, () => {
                evaluations++
                return probe.v / 10
            }, null)
            return root
        })
        assert.equal(outer.opacity, 0.7)
        assert.equal(evaluations, 1)
        engine.batch(() => { watched.height = 50 })
        assert.equal(evaluations, 1)
    })

    test(name + ': the subscriptions of a destroyed object are counted out', () => {
        let before = engine.stats.links
        let obj
        engine.batch(() => { obj = new QtObject(null) })
        engine.connect(obj, QtObject.SIGNAL_objectNameChanged, () => {})
        obj.$on(QtObject.SIGNAL_completed, () => {}, null)
        assert.equal(engine.stats.links, before + 2)
        engine.batch(() => obj.destroy())
        assert.equal(engine.stats.links, before)
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
