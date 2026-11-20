// Micro benchmarks, written in QML and compiled: bindings, signals, the frame, memory per object.
const { compileQml, load, measure, heapUsed, now } = require('./lib')

const compiled = new Map()
function app(name, documents, options){
    if(!compiled.has(name)) compiled.set(name, compileQml(typeof documents === 'string' ? { 'Main.qml': documents } : documents))
    return load(compiled.get(name), options)
}

const lines = (n, fn) => Array.from({ length: n }, (_, i) => fn(i)).join('\n')

// one source property, N bindings reading it
const FANOUT = `import QtQuick 2.15
Item {
    id: root
    property int v: 0
    property int sum: 0
    Repeater {
        model: 1000
        Item { property int c: root.v + index }
    }
}`

// a chain p0 <- p1 <- ... <- p999
const CHAIN = `import QtQuick 2.15
Item {
    id: root
    property int v: 0
${lines(1000, i => `    property int p${i}: ${i ? 'p' + (i - 1) : 'v'} + 1`)}
}`

// a <- b, c <- d: one change of a evaluates d once per path the runtime takes; counted too
const DIAMOND = `import QtQuick 2.15
Item {
    id: root
    property int v: 0
    property int count: 0
${lines(100, i => `    property int b${i}: v + ${i}
    property int c${i}: v * 2 + ${i}
    property int d${i}: b${i} + c${i}
    onD${i}Changed: count++`)}
}`

// a binding whose dependencies change with a condition
const CONDITIONAL = `import QtQuick 2.15
Item {
    id: root
    property bool flag: false
    property int a: 0
    property int b: 0
    Repeater {
        model: 500
        Item { property int c: root.flag ? root.a + index : root.b - index }
    }
}`

const SIGNALS = `import QtQuick 2.15
Item {
    id: root
    signal ping(int x)
    property int count: 0
    onPing: count += x
}`

const COLUMN = `import QtQuick 2.15
Item {
    id: root
    property int h: 10
    width: 400
    Column {
        id: column
        Repeater {
            model: 1000
            Rectangle { width: 100; height: root.h + index % 3; color: "red" }
        }
    }
}`

const ANCHORS = `import QtQuick 2.15
Item {
    id: root
    width: 1000
    height: 1000
    property int w: 10
    Item { id: a0; anchors.left: parent.left; anchors.top: parent.top; width: root.w; height: 10 }
${lines(99, i => `    Item { id: a${i + 1}; anchors.left: a${i}.right; anchors.top: a${i}.bottom; width: root.w; height: 10 }`)}
}`

const LISTVIEW = `import QtQuick 2.15
Item {
    id: root
    width: 400
    height: 600
    property alias view: list
    ListView {
        id: list
        anchors.fill: parent
        model: 100000
        delegate: Item {
            width: 400
            height: 30
            Rectangle { anchors.fill: parent; color: index % 2 ? "white" : "lightgray" }
            Text { anchors.verticalCenter: parent.verticalCenter; text: "Row " + index }
            MouseArea { anchors.fill: parent }
        }
    }
}`

const ANIMATION = `import QtQuick 2.15
Item {
    id: root
    property alias rep: repeater
    Repeater {
        id: repeater
        model: 300
        Rectangle {
            width: 10; height: 10
            NumberAnimation on x { from: 0; to: 1000; duration: 100000; running: true }
        }
    }
}`

const CREATE = `import QtQuick 2.15
Item {
    id: root
    property int n: 0
    property alias rep: repeater
    Repeater {
        id: repeater
        model: root.n
        Rectangle {
            width: parent ? parent.width : 0
            height: 20
            color: index % 2 ? "white" : "gray"
            border.width: 1
            Text { anchors.centerIn: parent; text: "Item " + index }
            MouseArea { anchors.fill: parent; hoverEnabled: true; onClicked: root.n = root.n }
        }
    }
}`

const CHILDREN = `import QtQuick 2.15
Item {
    id: root
    property int n: 0
    Item { Repeater { model: root.n; Item { width: 1; height: 1 } } }
}`

const benchmarks = [
    {
        name: 'bind.fanout_1000x100', run(runs){
            let { JQ, root } = app('fanout', FANOUT)
            return measure(() => { for(let i = 0; i < 100; i++) JQ.batch(() => { root.v++ }) }, { runs })
        },
    },
    {
        name: 'bind.chain_1000x100', run(runs){
            let { JQ, root } = app('chain', CHAIN)
            return measure(() => { for(let i = 0; i < 100; i++) JQ.batch(() => { root.v++ }) }, { runs })
        },
    },
    {
        name: 'bind.diamond_100x1000', run(runs){
            let { JQ, root } = app('diamond', DIAMOND)
            return measure(() => { for(let i = 0; i < 1000; i++) JQ.batch(() => { root.v++ }) }, { runs })
        },
    },
    {
        name: 'bind.diamond_evaluations', unit: 'n', run(){
            let { JQ, root } = app('diamond', DIAMOND)
            let before = JQ.stats.evaluations
            JQ.batch(() => { root.v++ })
            return [JQ.stats.evaluations - before]
        },
    },
    {
        name: 'bind.conditional_500x200', run(runs){
            let { JQ, root } = app('conditional', CONDITIONAL)
            return measure(() => {
                for(let i = 0; i < 100; i++){
                    JQ.batch(() => { root.flag = !root.flag })
                    JQ.batch(() => { root.a++; root.b++ })
                }
            }, { runs })
        },
    },
    {
        name: 'signal.emit_handler_100k', run(runs){
            let { root } = app('signals', SIGNALS)
            return measure(() => { for(let i = 0; i < 100000; i++) root.ping(1) }, { runs })
        },
    },
    {
        name: 'signal.emit_100connections_1k', run(runs){
            let { root } = app('signals', SIGNALS)
            let total = 0
            for(let i = 0; i < 100; i++) root.ping.connect((x) => { total += x })
            return measure(() => { for(let i = 0; i < 1000; i++) root.ping(1) }, { runs })
        },
    },
    {
        name: 'signal.connect_disconnect_10k', run(runs){
            let { root } = app('signals', SIGNALS)
            let fns = Array.from({ length: 10000 }, () => () => {})
            return measure(() => {
                for(let fn of fns) root.ping.connect(fn)
                for(let fn of fns) root.ping.disconnect(fn)
            }, { runs })
        },
    },
    {
        name: 'frame.column_1000x20', run(runs){
            let { JQ, root } = app('column', COLUMN)
            return measure(() => {
                for(let i = 0; i < 20; i++){
                    JQ.batch(() => { root.h++ })
                    JQ.engine.flushFrame()
                }
            }, { runs })
        },
    },
    {
        name: 'frame.anchors_chain100x100', run(runs){
            let { JQ, root } = app('anchors', ANCHORS)
            return measure(() => {
                for(let i = 0; i < 100; i++){
                    JQ.batch(() => { root.w = 10 + i % 7 })
                    JQ.engine.flushFrame()
                }
            }, { runs })
        },
    },
    {
        name: 'frame.listview_scroll_200', run(runs){
            let { JQ, root } = app('listview', LISTVIEW)
            JQ.engine.flushFrame()
            let view = root.view
            let y = 0
            return measure(() => {
                for(let i = 0; i < 200; i++){
                    y += 97
                    JQ.batch(() => { view.contentY = y })
                    JQ.engine.flushFrame()
                }
            }, { runs })
        },
    },
    {
        // far down a long list: rows 80000 and on
        name: 'frame.listview_scroll_far_200', run(runs){
            let { JQ, root } = app('listview', LISTVIEW)
            JQ.engine.flushFrame()
            let view = root.view
            JQ.batch(() => view.positionViewAtIndex(80000, 0))
            JQ.engine.flushFrame()
            let y = view.contentY
            return measure(() => {
                for(let i = 0; i < 200; i++){
                    y += 97
                    JQ.batch(() => { view.contentY = y })
                    JQ.engine.flushFrame()
                }
            }, { runs })
        },
    },
    {
        name: 'frame.animation_300x60', run(runs){
            let { frame } = app('animation', ANIMATION, { manualFrames: true })
            return measure(() => {
                for(let i = 0; i < 60; i++) frame()
            }, { runs })
        },
    },
    {
        // 10000 children of one parent made, then destroyed (a Repeater filling an item)
        name: 'scale.children_10000_create', run(runs){
            let { JQ, root } = app('children', CHILDREN)
            return measure(() => {
                let start = now()
                JQ.batch(() => { root.n = 10000 })
                let time = now() - start
                JQ.batch(() => { root.n = 0 })
                return time
            }, { runs: Math.min(runs, 5), warmup: 1 })
        },
    },
    {
        name: 'scale.children_10000_destroy', run(runs){
            let { JQ, root } = app('children', CHILDREN)
            return measure(() => {
                JQ.batch(() => { root.n = 10000 })
                let start = now()
                JQ.batch(() => { root.n = 0 })
                return now() - start
            }, { runs: Math.min(runs, 5), warmup: 1 })
        },
    },
    {
        name: 'create.delegates_1000', run(runs){
            let { JQ, root } = app('create', CREATE)
            return measure(() => {
                let start = now()
                JQ.batch(() => { root.n = 1000 })
                JQ.engine.flushFrame()
                let time = now() - start
                JQ.batch(() => { root.n = 0 })
                JQ.engine.flushFrame()
                return time
            }, { runs })
        },
    },
    {
        name: 'destroy.delegates_1000', run(runs){
            let { JQ, root } = app('create', CREATE)
            return measure(() => {
                JQ.batch(() => { root.n = 1000 })
                JQ.engine.flushFrame()
                let start = now()
                JQ.batch(() => { root.n = 0 })
                JQ.engine.flushFrame()
                return now() - start
            }, { runs })
        },
    },
]

// retained heap per object: N objects made by a Repeater, after gc
function perObject(name, delegate, n = 5000){
    let text = `import QtQuick 2.15
Item {
    id: root
    property int n: 0
    Repeater { model: root.n; delegate: ${delegate} }
}`
    return {
        name, run(runs){
            let { JQ, root } = app('mem-' + name, text)
            let samples = []
            for(let i = 0; i < Math.min(runs, 5) + 1; i++){
                let before = heapUsed()
                JQ.batch(() => { root.n = n })
                JQ.engine.flushFrame()
                let after = heapUsed()
                JQ.batch(() => { root.n = 0 })
                JQ.engine.flushFrame()
                if(i) samples.push((after - before) / n)
            }
            return samples
        },
    }
}

const memory = [
    perObject('mem.item', 'Item { width: 10; height: 10 }'),
    perObject('mem.rectangle', 'Rectangle { width: 10; height: 10; color: "red" }'),
    perObject('mem.text', 'Text { text: "Row " + index }'),
    perObject('mem.delegate', `Item {
        width: 400; height: 30
        Rectangle { anchors.fill: parent; color: index % 2 ? "white" : "lightgray" }
        Text { anchors.verticalCenter: parent.verticalCenter; text: "Row " + index }
        MouseArea { anchors.fill: parent }
    }`, 2000),
]

module.exports = { benchmarks, memory, app }
