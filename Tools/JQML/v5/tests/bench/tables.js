// The tables of ImtCore (imtcontrols Table: a ListView of rows over a TreeItemModel) against TableView (Qt's, over a
// TableModel, with a HorizontalHeaderView): the time to make and lay out a table of N rows, the memory it keeps, the
// items it has, scrolling, editing cells and inserting rows. The engine only (node, no DOM): what the JavaScript costs.
//   node --expose-gc tests/bench/tables.js [--rows 1000,10000,100000] [--columns 6] [--runs 3] [--out file.json]
// Each case runs in a process of its own (the memory of one is not the garbage of another).
const fs = require('fs')
const os = require('os')
const path = require('path')
const { execFileSync } = require('child_process')
const { V5, stats, now, gc, heapUsed, applicationPart, load } = require('./lib')
const { IMAGING } = require('./apps')

const WIDTH = 1000, HEIGHT = 600

function args(){
    let options = { rows: '1000,10000,100000', columns: 6, runs: 3, out: '', child: '' }
    let argv = process.argv.slice(2)
    for(let i = 0; i < argv.length; i++) options[argv[i].replace(/^--/, '')] = argv[++i]
    options.columns = +options.columns
    options.runs = +options.runs
    return options
}

// ---- the documents ----

const IMTCORE_DOC = (columns) => `import QtQuick
import Acf 1.0
import imtcontrols 1.0

Item {
    id: root
    width: ${WIDTH}
    height: ${HEIGHT}
    property alias view: table
    TableHeaders { id: headersModel }
    TreeItemModel { id: rowsModel }
    // the headers and the rows are given once they are filled, as SimpleCollectionTable does
    Table {
        id: table
        anchors.fill: parent
    }
    function fill(count) {
        for (let c = 0; c < ${columns}; c++)
            headersModel.addHeader("c" + c, "Column " + c, false, false)
        table.headers = headersModel
        for (let r = 0; r < count; r++) {
            let i = rowsModel.insertNewItem()
            for (let c = 0; c < ${columns}; c++)
                rowsModel.setData("c" + c, "r" + r + " c" + c, i)
        }
        table.elements = rowsModel
    }
    function scrollTo(y) { table.contentY = y }
    function contentY() { return table.contentY }
    function edit(row, value) { rowsModel.setData("c1", value, row) }
    function insertTop() {
        let i = rowsModel.insertNewItem(0)
        for (let c = 0; c < ${columns}; c++)
            rowsModel.setData("c" + c, "new c" + c, 0)
    }
}
`

const TABLEVIEW_DOC = (columns) => `import QtQuick
import QtQuick.Controls
import Qt.labs.qmlmodels

Item {
    id: root
    width: ${WIDTH}
    height: ${HEIGHT}
    property alias view: tableView
    TableModel {
        id: tableModel
${Array.from({ length: columns }, (_, c) => '        TableModelColumn { display: "c' + c + '" }').join('\n')}
    }
    HorizontalHeaderView {
        id: header
        width: parent.width
        syncView: tableView
        textRole: "display"
        model: [${Array.from({ length: columns }, (_, c) => '"Column ' + c + '"').join(', ')}]
    }
    TableView {
        id: tableView
        anchors.top: header.bottom
        width: parent.width
        height: parent.height - header.height
        clip: true
        model: tableModel
        columnWidthProvider: function(column) { return tableView.width / ${columns} }
        delegate: Rectangle {
            required property var display
            // the height of a row of the imtcontrols Table (36)
            implicitHeight: 36
            color: row % 2 ? "#f7f7f7" : "white"
            required property int row
            Text {
                anchors.fill: parent
                anchors.leftMargin: 8
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideRight
                text: display
            }
        }
    }
    function fill(count) {
        let rows = []
        for (let r = 0; r < count; r++) {
            let row = {}
            for (let c = 0; c < ${columns}; c++)
                row["c" + c] = "r" + r + " c" + c
            rows.push(row)
        }
        tableModel.rows = rows
    }
    function scrollTo(y) { tableView.contentY = y }
    function contentY() { return tableView.contentY }
    function edit(row, value) { tableModel.setData(tableModel.index(row, 1), value, Qt.DisplayRole) }
    function insertTop() {
        let row = {}
        for (let c = 0; c < ${columns}; c++)
            row["c" + c] = "new c" + c
        tableModel.insertRow(0, row)
    }
}
`

// compiles a document with the configuration of ImtCore (its modules: imtcontrols, Acf)
function compileWithImtCore(text){
    const { compile } = require(path.join(V5, 'compiler/index'))
    require('./apps').available()   // the environment of the configs
    let dir = fs.mkdtempSync(path.join(os.tmpdir(), 'jqml-tables-'))
    try {
        fs.mkdirSync(path.join(dir, 'app'))
        fs.writeFileSync(path.join(dir, 'app', 'Main.qml'), text)
        let out = path.join(dir, 'out')
        compile({ mode: 'js', config: path.join(IMAGING, 'ImtCore/Qml/web/imtcore.json'), entry: path.join(dir, 'app', 'Main.qml'), output: out, name: 'index' })
        return applicationPart(fs.readFileSync(path.join(out, 'index.js'), 'utf8'))
    } finally {
        fs.rmSync(dir, { recursive: true, force: true })
    }
}

// ---- one case (in a child process) ----

function countItems(JQ, root){
    let items = 0, visible = 0
    let visit = (item) => {
        items++
        if(item.visible) visible++
        for(let child of item.$children || []) if(child instanceof JQ.Item || child.$v !== undefined && child.constructor.$type && 'visible' in child) visit(child)
    }
    visit(root)
    return { items, visible }
}

function runCase(kind, rows, columns){
    let app = fs.readFileSync(process.env.TABLES_APP, 'utf8')
    gc()
    let base = heapUsed()
    let { JQ, root, frame, dispose } = load(app, { manualFrames: true })
    let settle = () => { for(let i = 0; i < 4; i++) frame() }
    settle()
    gc()
    let empty = heapUsed()
    let t0 = now()
    JQ.batch(() => root.fill(rows))
    settle()
    let create = now() - t0
    gc()
    let filled = heapUsed()
    let counts = countItems(JQ, root)

    // scrolling: steps of a third of the viewport, then jumps
    let contentHeight = rows * 36
    let t1 = now()
    for(let i = 1; i <= 150; i++){
        JQ.batch(() => root.scrollTo(Math.min(i * 200, contentHeight - HEIGHT)))
        frame()
    }
    let scrollSteps = (now() - t1) / 150
    let t2 = now()
    for(let i = 0; i < 40; i++){
        let y = Math.floor(((i * 7919) % 97) / 97 * Math.max(0, contentHeight - HEIGHT))
        JQ.batch(() => root.scrollTo(y))
        frame()
    }
    let scrollJumps = (now() - t2) / 40
    JQ.batch(() => root.scrollTo(0))
    settle()

    // editing a visible cell, inserting a row at the top
    let t3 = now()
    for(let i = 0; i < 100; i++){
        JQ.batch(() => root.edit(i % 10, 'edited ' + i))
        frame()
    }
    let edit = (now() - t3) / 100
    let t4 = now()
    for(let i = 0; i < 20; i++){
        JQ.batch(() => root.insertTop())
        frame()
    }
    let insert = (now() - t4) / 20
    gc()
    let after = heapUsed()
    dispose()
    return {
        kind, rows, columns,
        create, scrollSteps, scrollJumps, edit, insert,
        heapRuntime: empty - base, heapTable: filled - empty, heapAfter: after - empty,
        items: counts.items,
    }
}

// ---- main ----

function main(){
    let options = args()
    if(options.child){
        let [kind, rows] = options.child.split(':')
        process.stdout.write(JSON.stringify(runCase(kind, +rows, options.columns)))
        return
    }
    let tmp = fs.mkdtempSync(path.join(os.tmpdir(), 'jqml-tables-app-'))
    let apps = {
        ImtCore: path.join(tmp, 'imtcore.js'),
        TableView: path.join(tmp, 'tableview.js'),
    }
    fs.writeFileSync(apps.ImtCore, compileWithImtCore(IMTCORE_DOC(options.columns)))
    fs.writeFileSync(apps.TableView, compileWithImtCore(TABLEVIEW_DOC(options.columns)))
    let results = []
    for(let rows of options.rows.split(',').map(Number)){
        for(let kind of Object.keys(apps)){
            let samples = []
            for(let run = 0; run < options.runs; run++){
                let out = execFileSync(process.execPath, ['--expose-gc', '--max-old-space-size=8192', __filename, '--child', kind + ':' + rows, '--columns', String(options.columns)],
                    { env: Object.assign({}, process.env, { TABLES_APP: apps[kind] }), encoding: 'utf8', maxBuffer: 1 << 26, timeout: 20 * 60 * 1000 })
                samples.push(JSON.parse(out.slice(out.lastIndexOf('{"kind"'))))
            }
            let median = (key) => stats(samples.map(s => s[key])).median
            let result = { kind, rows, columns: options.columns }
            for(let key of ['create', 'scrollSteps', 'scrollJumps', 'edit', 'insert', 'heapTable', 'heapAfter', 'items']) result[key] = median(key)
            results.push(result)
            let mb = (b) => (b / 1048576).toFixed(1) + ' MB'
            console.log(`${kind.padEnd(10)} ${String(rows).padStart(7)} rows | create ${result.create.toFixed(0).padStart(6)} ms | heap ${mb(result.heapTable).padStart(9)}`
                + ` | items ${String(result.items).padStart(5)} | scroll step ${result.scrollSteps.toFixed(2)} ms, jump ${result.scrollJumps.toFixed(2)} ms`
                + ` | edit ${result.edit.toFixed(2)} ms | insert ${result.insert.toFixed(2)} ms`)
        }
    }
    fs.rmSync(tmp, { recursive: true, force: true })
    if(options.out) fs.writeFileSync(options.out, JSON.stringify({ date: new Date().toISOString(), results }, null, 2))
}

if(require.main === module) main()
module.exports = { IMTCORE_DOC, TABLEVIEW_DOC, compileWithImtCore }
