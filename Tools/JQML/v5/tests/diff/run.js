// Differential tests: the same QML document in Qt 6 (qml -platform offscreen) and in JQML v5, their console.log lines
// compared. A case logs what it checks (values, the order of signals) and ends with Qt.quit().
//   node tests/diff/run.js [case.qml ...]          QML: the qml tool of Qt (QML env, or ~/Qt/<version>/<platform>/bin/qml)
// A case that imports QtTest is a TestCase run by qmltestrunner in Qt (its pointer and key events are real): its
// console.log lines (QDEBUG) and its failures ("FAIL <test>") are compared; v5 runs it with the root in the pointer scene.
// QtQuick.Controls is the Basic style in v5: Qt runs with QT_QUICK_CONTROLS_STYLE=Basic (not the style of the platform).
// Exit code 1 when a case differs.
const fs = require('fs')
const os = require('os')
const path = require('path')
const { execFileSync } = require('child_process')
const { compileQml, load } = require('../bench/lib')

const QT_ENV = Object.assign({}, process.env, { QT_QUICK_CONTROLS_STYLE: 'Basic' })

function qmlTool(){
    if(process.env.QML) return process.env.QML
    let root = path.join(os.homedir(), 'Qt')
    for(let version of fs.existsSync(root) ? fs.readdirSync(root).sort().reverse() : []){
        for(let platform of ['macos', 'gcc_64', 'msvc2022_64', 'msvc2019_64']){
            let tool = path.join(root, version, platform, 'bin', process.platform === 'win32' ? 'qml.exe' : 'qml')
            if(fs.existsSync(tool)) return tool
        }
    }
    return null
}

const isTestCase = (file) => /^import QtTest/m.test(fs.readFileSync(file, 'utf8'))

// qmltestrunner of the same Qt: "QDEBUG : <name>::<test>() text" and "FAIL!  : <name>::<test>() message"
function qtTestLines(file){
    let runner = path.join(path.dirname(qmlTool()), process.platform === 'win32' ? 'qmltestrunner.exe' : 'qmltestrunner')
    let out = execFileSync('/bin/sh', ['-c', `"${runner}" -platform offscreen -input "${file}" 2>&1 || true`], { encoding: 'utf8', timeout: 60000, env: QT_ENV })
    let lines = []
    for(let line of out.split('\n')){
        let debug = /^QDEBUG : \S+?\(\) (?:qml: )?(.*)$/.exec(line)
        if(debug){ lines.push(debug[1]); continue }
        let fail = /^FAIL!  : \S+::(\w+)\(\)/.exec(line)
        if(fail) lines.push('FAIL ' + fail[1])
    }
    return lines
}

// Qt prints console.log as "qml: text" on stderr; the lines of v5 are collected from its console
async function v5Lines(file){
    let lines = []
    // console.log of Qt: the arguments as strings, null and undefined by name
    let text = (args) => args.map(a => a === null ? 'null' : a === undefined ? 'undefined' : String(a)).join(' ')
    let testCase = isTestCase(file)
    // a failure of a TestCase: "FAIL!  : <test> message"
    let error = (...a) => {
        let fail = testCase && /^FAIL!  : (\w+)/.exec(text(a))
        lines.push(fail ? 'FAIL ' + fail[1] : 'ERROR ' + text(a))
    }
    let consoleOf = { log: (...a) => lines.push(text(a)), info(){}, debug(){}, warn(){}, error }
    let app = compileQml({ 'Main.qml': fs.readFileSync(file, 'utf8') }, { allowWarnings: true })
    // the platform Qt runs on (offscreen): its navigator for the focus policies of macOS, no hover effects (no mouse)
    let globals = { navigator: { platform: process.platform === 'darwin' ? 'MacIntel' : process.platform === 'win32' ? 'Win32' : 'Linux x86_64', language: 'en-US' } }
    let beforeMount = (JQ) => { JQ.Qt.styleHints.useHoverEffects = false }
    let { context, JQ, root, dispose } = load(app, { console: consoleOf, globals, beforeMount })
    let tests = []
    // QQuickView::ResizeViewToRootObject (qml, qmltestrunner): the window is the size of the root; its content item (the
    // parent of the root, where the Overlay of popups is) is in the pointer scene
    let top = root && root.$parent && root.$parent.$isItem ? root.$parent : root
    if(top !== root) JQ.batch(() => {
        top.width = root.width
        top.height = root.height
    })
    if(testCase){
        JQ.mouse.scene.roots = [top]
        let visit = (obj) => {
            if(obj instanceof JQ.types.TestCase) tests.push(obj)
            for(let child of obj.$children || []) visit(child)
        }
        visit(root)
    }
    let done = () => context.__jqmlQuit || (testCase && tests.every(t => t.completed))
    let start = Date.now()
    while(!done() && Date.now() - start < 30000) await new Promise((resolve) => setTimeout(resolve, 5))
    if(testCase) JQ.mouse.scene.roots = []
    dispose()
    return lines
}

async function main(){
    if(!qmlTool()){
        console.log('no qml tool of Qt 6: set QML=<path to qml>')
        process.exit(0)
    }
    let files = process.argv.slice(2)
    if(!files.length) files = fs.readdirSync(path.join(__dirname, 'cases')).filter(f => f.endsWith('.qml')).map(f => path.join(__dirname, 'cases', f))
    let failed = 0
    for(let file of files){
        let qtOut = isTestCase(file) ? qtTestLines(file)
            : execFileSync('/bin/sh', ['-c', `"${qmlTool()}" -platform offscreen "${file}" 2>&1 || true`], { encoding: 'utf8', timeout: 30000, env: QT_ENV })
                .split('\n').filter(l => l.startsWith('qml: ')).map(l => l.slice(5))
        let v5 = await v5Lines(file)
        let same = qtOut.length === v5.length && qtOut.every((l, i) => l === v5[i])
        console.log((same ? 'same    ' : 'DIFFERS ') + path.basename(file) + ' (' + qtOut.length + ' lines)')
        if(!same){
            failed++
            for(let i = 0; i < Math.max(qtOut.length, v5.length); i++){
                if(qtOut[i] !== v5[i]) console.log('  ' + i + '\n    Qt: ' + qtOut[i] + '\n    v5: ' + v5[i])
            }
        }
    }
    process.exit(failed ? 1 : 0)
}

main()
