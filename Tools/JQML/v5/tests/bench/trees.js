// The tree of ImtCore (imtcontrols BasicTreeView: a ListView over a flattened ListModel of the visible nodes) against
// TreeView (Qt's, over a TreeModel, with the Basic TreeViewDelegate): a tree of N top nodes with K children each and
// C columns: the time to make it, to expand and collapse all, to toggle one node, to scroll the expanded tree, and the
// memory. The engine only (node, no DOM).
//   node --expose-gc tests/bench/trees.js [--nodes 100,1000,10000] [--children 10] [--columns 4] [--runs 3] [--out file.json]
const fs = require('fs')
const os = require('os')
const path = require('path')
const { execFileSync } = require('child_process')
const { stats, now, gc, heapUsed, load } = require('./lib')
const { compileWithImtCore } = require('./tables')

const WIDTH = 1000, HEIGHT = 600

function args(){
    let options = { nodes: '100,1000,10000', children: 10, columns: 4, runs: 3, out: '', child: '' }
    let argv = process.argv.slice(2)
    for(let i = 0; i < argv.length; i++) options[argv[i].replace(/^--/, '')] = argv[++i]
    for(let key of ['children', 'columns', 'runs']) options[key] = +options[key]
    return options
}

const IMTCORE_DOC = (children, columns) => `import QtQuick
import Acf 1.0
import imtcontrols 1.0

Item {
    id: root
    width: ${WIDTH}
    height: ${HEIGHT}
    BasicTreeView {
        id: tree
        anchors.fill: parent
        columns: [${Array.from({ length: columns }, (_, c) => '{ id: "c' + c + '", name: "Column ' + c + '"' + (c === 0 ? ', tree: true' : '') + ' }').join(', ')}]
    }
    function fill(count) {
        let nodes = []
        for (let n = 0; n < count; n++) {
            let kids = []
            for (let k = 0; k < ${children}; k++) {
                let data = {}
                for (let c = 0; c < ${columns}; c++) data["c" + c] = "n" + n + "." + k + " c" + c
                kids.push({ key: "n" + n + "." + k, text: "n" + n + "." + k, data: data })
            }
            let data = {}
            for (let c = 0; c < ${columns}; c++) data["c" + c] = "n" + n + " c" + c
            nodes.push({ key: "n" + n, text: "n" + n, data: data, children: kids })
        }
        tree.model = nodes
    }
    function expandAll() { tree.expandAll() }
    function collapseAll() { tree.collapseAll() }
    function toggle(n) { tree.toggleExpanded("n" + n) }
    function list(item) {
        for (let i = 0; i < item.children.length; i++) {
            let c = item.children[i]
            if (c.cacheBuffer !== undefined && c.contentY !== undefined) return c
            let found = list(c)
            if (found) return found
        }
        return null
    }
    function scrollTo(y) { list(tree).contentY = y }
}
`

const TREEVIEW_DOC = (children, columns) => `import QtQuick
import QtQuick.Controls
import Qt.labs.qmlmodels

Item {
    id: root
    width: ${WIDTH}
    height: ${HEIGHT}
    TreeModel {
        id: treeModel
${Array.from({ length: columns }, (_, c) => '        TableModelColumn { display: "c' + c + '" }').join('\n')}
    }
    TreeView {
        id: treeView
        anchors.fill: parent
        clip: true
        model: treeModel
        columnWidthProvider: function(column) { return treeView.width / ${columns} }
        // the row height of BasicTreeView (28)
        delegate: TreeViewDelegate { implicitHeight: 28 }
    }
    function fill(count) {
        let nodes = []
        for (let n = 0; n < count; n++) {
            let kids = []
            for (let k = 0; k < ${children}; k++) {
                let row = {}
                for (let c = 0; c < ${columns}; c++) row["c" + c] = "n" + n + "." + k + " c" + c
                kids.push(row)
            }
            let row = { rows: kids }
            for (let c = 0; c < ${columns}; c++) row["c" + c] = "n" + n + " c" + c
            nodes.push(row)
        }
        treeModel.rows = nodes
    }
    function expandAll() { treeView.expandRecursively() }
    function collapseAll() { treeView.collapseRecursively() }
    function toggle(n) { treeView.toggleExpanded(treeView.rowAtIndex(treeModel.index([n], 0))) }
    function scrollTo(y) { treeView.contentY = y }
}
`

function runCase(nodes){
    let app = fs.readFileSync(process.env.TREES_APP, 'utf8')
    gc()
    let { JQ, root, frame, dispose } = load(app, { manualFrames: true })
    let settle = () => { for(let i = 0; i < 4; i++) frame() }
    settle()
    gc()
    let empty = heapUsed()
    let time = (fn) => { let t = now(); JQ.batch(fn); settle(); return now() - t }
    let create = time(() => root.fill(nodes))
    gc()
    let heapCollapsed = heapUsed() - empty
    let expandAll = time(() => root.expandAll())
    gc()
    let heapExpanded = heapUsed() - empty
    let t = now()
    for(let i = 1; i <= 100; i++){
        JQ.batch(() => root.scrollTo(i * 300))
        frame()
    }
    let scroll = (now() - t) / 100
    JQ.batch(() => root.scrollTo(0))
    settle()
    let collapseAll = time(() => root.collapseAll())
    let t2 = now()
    for(let i = 0; i < 40; i++){
        JQ.batch(() => root.toggle(i % 4))
        frame()
    }
    let toggle = (now() - t2) / 40
    dispose()
    return { create, expandAll, collapseAll, toggle, scroll, heapCollapsed, heapExpanded }
}

function main(){
    let options = args()
    if(options.child){
        process.stdout.write(JSON.stringify(runCase(+options.child)))
        return
    }
    let tmp = fs.mkdtempSync(path.join(os.tmpdir(), 'jqml-trees-app-'))
    let apps = {
        ImtCore: path.join(tmp, 'imtcore.js'),
        TreeView: path.join(tmp, 'treeview.js'),
    }
    fs.writeFileSync(apps.ImtCore, compileWithImtCore(IMTCORE_DOC(options.children, options.columns)))
    fs.writeFileSync(apps.TreeView, compileWithImtCore(TREEVIEW_DOC(options.children, options.columns)))
    let results = []
    for(let nodes of options.nodes.split(',').map(Number)){
        for(let kind of Object.keys(apps)){
            let samples = []
            for(let run = 0; run < options.runs; run++){
                let out = execFileSync(process.execPath, ['--expose-gc', '--max-old-space-size=8192', __filename, '--child', String(nodes), '--children', String(options.children), '--columns', String(options.columns)],
                    { env: Object.assign({}, process.env, { TREES_APP: apps[kind] }), encoding: 'utf8', maxBuffer: 1 << 26, timeout: 30 * 60 * 1000 })
                samples.push(JSON.parse(out.slice(out.lastIndexOf('{"create"'))))
            }
            let result = { kind, nodes, total: nodes * (1 + options.children), columns: options.columns }
            for(let key of Object.keys(samples[0])) result[key] = stats(samples.map(s => s[key])).median
            results.push(result)
            let mb = (b) => (b / 1048576).toFixed(1) + ' MB'
            console.log(`${kind.padEnd(9)} ${String(result.total).padStart(7)} nodes | create ${result.create.toFixed(0).padStart(6)} ms | expand all ${result.expandAll.toFixed(0).padStart(6)} ms`
                + ` | collapse all ${result.collapseAll.toFixed(0).padStart(5)} ms | toggle ${result.toggle.toFixed(2)} ms | scroll ${result.scroll.toFixed(2)} ms`
                + ` | heap ${mb(result.heapCollapsed)} / expanded ${mb(result.heapExpanded)}`)
        }
    }
    fs.rmSync(tmp, { recursive: true, force: true })
    if(options.out) fs.writeFileSync(options.out, JSON.stringify({ date: new Date().toISOString(), results }, null, 2))
}

if(require.main === module) main()
module.exports = { IMTCORE_DOC, TREEVIEW_DOC }
