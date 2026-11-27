// Browser tests: a QML document compiled with dist/core.js into a page (as the compiler makes it for an application) and
// opened in Chromium through Playwright. Playwright is found as the package "playwright" (npm install in v5) or in the
// directory JQML_PLAYWRIGHT names; without it the tests are skipped.
const fs = require('fs')
const os = require('os')
const path = require('path')

const V5 = path.resolve(__dirname, '../..')

function playwright(){
    let candidates = []
    if(process.env.JQML_PLAYWRIGHT) candidates.push(process.env.JQML_PLAYWRIGHT)
    candidates.push('playwright')
    for(let candidate of candidates){
        try {
            return require(candidate)
        } catch(e){
            // the next one
        }
    }
    return null
}

// the QML files { 'Main.qml': text } as a page: { dir, url, dispose() }
function compilePage(documents){
    const { compile } = require(path.join(V5, 'compiler/index'))
    let root = fs.mkdtempSync(path.join(os.tmpdir(), 'jqml-web-'))
    for(let [name, text] of Object.entries(documents)){
        let file = path.join(root, 'app', name)
        fs.mkdirSync(path.dirname(file), { recursive: true })
        fs.writeFileSync(file, text)
    }
    let output = path.join(root, 'out')
    let result = compile({ mode: 'js', entry: path.join(root, 'app', 'Main.qml'), output, name: 'index' })
    if(result.warnings.length) throw new Error(result.warnings.join('\n'))
    compile({ mode: 'html', output, name: 'index' })
    return { dir: output, url: 'file://' + path.join(output, 'index.html'), dispose: () => fs.rmSync(root, { recursive: true, force: true }) }
}

// a browser for the tests of a file: open(qml) gives a page with the application mounted (window.__root)
async function browser(){
    let pw = playwright()
    if(!pw) return null
    let instance = await pw.chromium.launch()
    let pages = []
    return {
        async open(qml, options = {}){
            let compiled = compilePage({ 'Main.qml': qml })
            let context = await instance.newContext({ viewport: options.viewport || { width: 800, height: 600 }, deviceScaleFactor: 1 })
            if(options.clipboard) await context.grantPermissions(['clipboard-read', 'clipboard-write'])
            let page = await context.newPage()
            let errors = []
            page.on('pageerror', (e) => errors.push(String(e)))
            page.on('console', (m) => { if(m.type() === 'error') errors.push(m.text()) })
            await page.goto(compiled.url)
            await page.waitForFunction(() => window.__root)
            pages.push({ context, compiled })
            page.errors = errors
            return page
        },
        async close(){
            for(let p of pages){
                await p.context.close()
                p.compiled.dispose()
            }
            await instance.close()
        },
    }
}

module.exports = { browser, playwright, compilePage }
