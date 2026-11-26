// Run: node --test tests/web   (Playwright: npm install playwright in v5, or JQML_PLAYWRIGHT=<path to the package>)
// Canvas and Context2D in Chromium against Qt 6.12: the pages of canvas/*.qml log what they read and the pixels they
// draw; canvas/*.qt.txt is what Qt logs for them (qml -platform offscreen). The lines are the same, at a device pixel
// ratio of 1 and of 2 (Qt with a ratio of 1: on a ratio of 2 Qt reads the pixels of the device at the item's
// coordinates, v5 reads the item's), but for:
// - the components of a pixel, which may differ by 3 (the browser's rasterizer, Qt's premultiplied alpha);
// - the names of the generic font families (Qt: the platform's family, v5: serif, sans-serif, monospace);
// - Qt.rgba(): a color of QML is 8 bits a component in v5 (16 in Qt);
// - the message of a TypeError of the JavaScript engine.
const test = require('node:test')
const assert = require('node:assert/strict')
const fs = require('fs')
const path = require('path')
const { playwright, compilePage } = require('./lib')

const SKIP = playwright() ? false : 'no Playwright (npm install playwright, or JQML_PLAYWRIGHT=<path>)'
const DIR = path.join(__dirname, 'canvas')

let browser = null
test.before(async () => { if(!SKIP) browser = await playwright().chromium.launch() })
test.after(async () => { if(browser) await browser.close() })

// the lines a page logs until DONE (warnings and the browser's own messages left out)
async function linesOf(qml, ratio){
    let compiled = compilePage({ 'Main.qml': qml })
    let context = await browser.newContext({ viewport: { width: 400, height: 300 }, deviceScaleFactor: ratio })
    let page = await context.newPage()
    let lines = []
    let errors = []
    page.on('console', (m) => {
        if(m.type() === 'log') lines.push(...m.text().split('\n'))
    })
    page.on('pageerror', (e) => errors.push(String(e)))
    try {
        await page.goto(compiled.url)
        let start = Date.now()
        while(!lines.includes('DONE') && Date.now() - start < 15000) await new Promise((resolve) => setTimeout(resolve, 20))
    } finally {
        await context.close()
        compiled.dispose()
    }
    assert.deepEqual(errors, [])
    return lines.filter((l) => l !== 'painted')
}

const KNOWN = [/^font "(italic 12pt serif|small-caps 14px monospace|20px|bogus|oblique bold 9px sans-serif)"/, /^color qt\.rgba /, /^measureText none throws/]

// a line of Qt and a line of v5: the same words, the same numbers but for a pixel's components (by 3)
function same(qt, v5){
    if(qt === v5) return true
    let a = qt.split(' '), b = v5.split(' ')
    if(a.length !== b.length) return false
    return a.every((word, i) => {
        if(word === b[i]) return true
        let p = word.split(','), q = b[i].split(',')
        return p.length === 4 && q.length === 4 && p.every((v, k) => /^\d+$/.test(v) && /^\d+$/.test(q[k]) && Math.abs(v - q[k]) <= 3)
    })
}

for(let name of ['CanvasState', 'CanvasPaths', 'CanvasImages']){
    for(let ratio of [1, 2]){
        test(name + ' as in Qt 6.12, device pixel ratio ' + ratio, { skip: SKIP }, async () => {
            let qml = fs.readFileSync(path.join(DIR, name + '.qml'), 'utf8')
            let expected = fs.readFileSync(path.join(DIR, name + '.qt.txt'), 'utf8').split('\n').filter((l) => l)
            let got = await linesOf(qml, ratio)
            assert.equal(got.length, expected.length, got.join('\n'))
            for(let i = 0; i < expected.length; i++){
                if(KNOWN.some((re) => re.test(expected[i]))) continue
                assert.ok(same(expected[i], got[i]), 'line ' + (i + 1) + '\n  Qt: ' + expected[i] + '\n  v5: ' + got[i])
            }
        })
    }
}

test('painted: once for the reset of a new context, again only when commands went to the canvas', { skip: SKIP }, async () => {
    let qml = `import QtQuick
Item {
    width: 200; height: 200
    property int a: 0
    property int b: 0
    property int c: 0
    Canvas { id: c1; width: 50; height: 50; contextType: "2d"; onPaint: {}
        onPainted: parent.a++ }
    Canvas { id: c2; width: 50; height: 50; onPaint: { let ctx = getContext("2d") }
        onPainted: parent.b++ }
    Canvas { id: c3; width: 50; height: 50; onPaint: { let ctx = getContext("2d"); ctx.fillStyle = "red" }
        onPainted: parent.c++ }
    Timer { interval: 300; running: true; onTriggered: { console.log("painted", a, b, c); c1.requestPaint(); c2.requestPaint() } }
    Timer { interval: 600; running: true; onTriggered: { console.log("painted again", a, b, c); console.log("DONE") } }
}`
    // Qt 6.12: painted 1 1 1, painted again 1 1 1
    assert.deepEqual(await linesOf(qml, 1), ['painted 1 1 1', 'painted again 1 1 1', 'DONE'])
})
