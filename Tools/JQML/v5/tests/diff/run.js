// Differential tests: the same QML document in Qt 6 (qml -platform offscreen) and in JQML v5, their console.log lines
// compared. A case logs what it checks (values, the order of signals) and ends with Qt.quit().
//   node tests/diff/run.js [case.qml ...]          QML: the qml tool of Qt (QML env, or ~/Qt/<version>/<platform>/bin/qml)
// Exit code 1 when a case differs.
const fs = require('fs')
const os = require('os')
const path = require('path')
const { execFileSync } = require('child_process')
const { compileQml, load } = require('../bench/lib')

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

// Qt prints console.log as "qml: text" on stderr; the lines of v5 are collected from its console
async function v5Lines(file){
    let lines = []
    // console.log of Qt: the arguments as strings, null and undefined by name
    let text = (args) => args.map(a => a === null ? 'null' : a === undefined ? 'undefined' : String(a)).join(' ')
    let consoleOf = { log: (...a) => lines.push(text(a)), info(){}, debug(){}, warn(){}, error: (...a) => lines.push('ERROR ' + text(a)) }
    let app = compileQml({ 'Main.qml': fs.readFileSync(file, 'utf8') }, { allowWarnings: true })
    let { context, dispose } = load(app, { console: consoleOf })
    let start = Date.now()
    while(!context.__jqmlQuit && Date.now() - start < 20000) await new Promise((resolve) => setTimeout(resolve, 5))
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
        let qtOut = execFileSync('/bin/sh', ['-c', `"${qmlTool()}" -platform offscreen "${file}" 2>&1 || true`], { encoding: 'utf8', timeout: 30000 })
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
