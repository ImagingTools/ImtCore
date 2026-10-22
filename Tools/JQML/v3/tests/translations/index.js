const fs = require('fs')
const os = require('os')
const path = require('path')
const assert = require('assert')

const originalArgv = process.argv
process.argv = [originalArgv[0], originalArgv[1]]
const compiler = require('../../compiler/compiler')
process.argv = originalArgv

const { parseTsCatalog, loadTranslationCatalogs } = compiler

let failed = 0
let passed = 0

function test(name, fn) {
    try {
        fn()
        passed += 1
        console.log('ok  ' + name)
    } catch (error) {
        failed += 1
        console.error('fail  ' + name)
        console.error('  ' + (error.stack || error.message || error))
    }
}

function ts(messages) {
    const body = messages.map(message => {
        return '<message><source>' + message[0] + '</source><translation>' + message[1] + '</translation></message>'
    }).join('')
    return '<?xml version="1.0" encoding="utf-8"?><TS version="2.1" language="ru_RU">' + body + '</TS>'
}

test('parses ts messages and decodes entities', () => {
    const dict = parseTsCatalog(ts([
        ['No results for &quot;%1&quot;', 'Нет результатов для &quot;%1&quot;'],
        ['&lt;no name&gt;', '&lt;без имени&gt;'],
        ['Country', 'Страна'],
        ['Country', 'Страна 2'],
    ]))
    assert.strictEqual(dict['No results for "%1"'], 'Нет результатов для "%1"')
    assert.strictEqual(dict['<no name>'], '<без имени>')
    assert.strictEqual(dict.Country, 'Страна 2')
})

test('uses the first numerus form', () => {
    const xml = '<TS><message><source>%n item(s)</source><translation><numerusform>%n элемент</numerusform><numerusform>%n элемента</numerusform></translation></message></TS>'
    const dict = parseTsCatalog(xml)
    assert.strictEqual(dict['%n item(s)'], '%n элемент')
})

test('merges inherited translation directories with later values winning', () => {
    const tmp = fs.mkdtempSync(path.join(os.tmpdir(), 'jqml-tr-'))
    const baseDir = path.join(tmp, 'base')
    const appDir = path.join(tmp, 'app')
    fs.mkdirSync(baseDir)
    fs.mkdirSync(appDir)
    fs.writeFileSync(path.join(baseDir, 'ImtCore_ru_RU.ts'), ts([
        ['Country', 'Страна'],
        ['OnlyParent', 'Родитель'],
    ]))
    fs.writeFileSync(path.join(appDir, 'ImtCore_ru_RU.ts'), ts([
        ['Country', 'Страна-2'],
    ]))
    fs.writeFileSync(path.join(appDir, 'IotPlatform_ru_RU.ts'), ts([
        ['Hello', 'Привет'],
    ]))

    fs.writeFileSync(path.join(appDir, 'IotPlatform_en_US.ts'), '<?xml version="1.0"?><TS language="en_US"><message><source>Hello</source><translation>Hello</translation></message></TS>')

    const loaded = loadTranslationCatalogs([baseDir, appDir])
    assert.strictEqual(loaded.fileCount, 4)
    assert.strictEqual(loaded.catalogs.ru_RU.Country, 'Страна-2')
    assert.strictEqual(loaded.catalogs.ru_RU.OnlyParent, 'Родитель')
    assert.strictEqual(loaded.catalogs.ru_RU.Hello, 'Привет')
    assert.strictEqual(loaded.catalogs.en_US.Hello, 'Hello')
    assert.strictEqual(loaded.catalogs.ImtCore_ru_RU, undefined)
})

test('compile embeds merged catalogs from included configs', () => {
    const tmp = fs.mkdtempSync(path.join(os.tmpdir(), 'jqml-tr-compile-'))
    const baseTr = path.join(tmp, 'base-tr')
    const appTr = path.join(tmp, 'app-tr')
    const outDir = path.join(tmp, 'out')
    fs.mkdirSync(baseTr)
    fs.mkdirSync(appTr)
    fs.mkdirSync(outDir)
    fs.writeFileSync(path.join(baseTr, 'ImtCore_ru_RU.ts'), ts([
        ['Country', 'Страна'],
        ['OnlyParent', 'Родитель'],
    ]))
    fs.writeFileSync(path.join(appTr, 'ImtCore_ru_RU.ts'), ts([
        ['Country', 'Страна-2'],
    ]))
    fs.writeFileSync(path.join(appTr, 'IotPlatform_ru_RU.ts'), ts([
        ['Hello', 'Привет'],
    ]))
    fs.writeFileSync(path.join(tmp, 'base.json'), JSON.stringify({
        includes: [],
        dirs: [],
        translation_dir: 'base-tr',
    }))
    fs.writeFileSync(path.join(tmp, 'app.json'), JSON.stringify({
        includes: ['base.json'],
        dirs: [],
        translation_dir: 'app-tr',
    }))
    fs.writeFileSync(path.join(tmp, 'Main.qml'), 'import QtQuick\nItem {\n    Component.onCompleted: Qt.quit()\n}\n')

    compiler.compile({
        config: path.join(tmp, 'app.json'),
        output: outDir,
        entry: path.join(tmp, 'Main.qml'),
        name: 'app',
        root: '/',
        mode: 'js',
    })

    const js = fs.readFileSync(path.join(outDir, 'app.js'), 'utf8')
    const marker = 'JQApplication.embeddedTranslations='
    const start = js.indexOf(marker)
    assert.ok(start >= 0, 'embedded translations were not written')
    const lineEnd = js.indexOf('\n', start)
    const catalogs = JSON.parse(js.slice(start + marker.length, lineEnd))
    assert.strictEqual(catalogs.ru_RU.Country, 'Страна-2')
    assert.strictEqual(catalogs.ru_RU.OnlyParent, 'Родитель')
    assert.strictEqual(catalogs.ru_RU.Hello, 'Привет')
    assert.strictEqual(catalogs.ImtCore_ru_RU, undefined)
})

const realTs = path.resolve(__dirname, '../../../../../Impl/ImtCoreLoc/Translations/ImtCore_ru_RU.ts')
if (fs.existsSync(realTs)) {
    test('reads a real ImtCore catalog', () => {
        const dict = parseTsCatalog(fs.readFileSync(realTs, 'utf8'))
        assert.strictEqual(dict.Country, 'Страна')
        assert.strictEqual(dict['No results for "%1"'], 'Нет результатов для "%1"')
    })
}

console.log(passed + ' passed, ' + failed + ' failed')
if (failed) process.exit(1)
