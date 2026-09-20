const path = require('path')
const assert = require('assert')
const { LanguageService, positionToOffset } = require('../../compiler/language')

const enginePath = path.resolve(__dirname, '../..')
const fixtures = path.join(__dirname, 'fixtures')
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

function service() {
    const lang = new LanguageService({ enginePath })
    lang.indexQmldir(path.join(fixtures, 'mod', 'qmldir'))
    lang.indexQmlFile(path.join(fixtures, 'MyBox.qml'))
    lang.indexQmlFile(path.join(fixtures, 'App.qml'))
    lang.indexQmlFile(path.join(fixtures, 'mod', 'Card.qml'))
    return lang
}

function labels(items) {
    return items.map(item => item.label)
}

function hasLabel(items, name) {
    assert.ok(labels(items).indexOf(name) >= 0, 'missing completion ' + name + ' in ' + labels(items).slice(0, 30).join(', '))
}

test('syntax error diagnostic', () => {
    const lang = service()
    const file = path.join(fixtures, 'SyntaxError.qml')
    const diags = lang.getDiagnostics(file)
    assert.ok(diags.length > 0, 'expected syntax diagnostics')
    assert.strictEqual(diags[0].severity, 'error')
})

test('unknown type diagnostic', () => {
    const lang = service()
    const file = path.join(fixtures, 'UnknownType.qml')
    const diags = lang.getDiagnostics(file)
    const messages = diags.map(item => item.message)
    assert.ok(messages.some(msg => msg.indexOf('NotAType') >= 0), messages.join(' | '))
})

test('unknown property diagnostic', () => {
    const lang = service()
    const file = path.join(fixtures, 'UnknownProp.qml')
    const diags = lang.getDiagnostics(file)
    const messages = diags.map(item => item.message)
    assert.ok(messages.some(msg => msg.indexOf('notAProperty') >= 0), messages.join(' | '))
    assert.ok(messages.some(msg => msg.indexOf('anchors.notAFill') >= 0), messages.join(' | '))
})

test('valid file has no syntax errors', () => {
    const lang = service()
    const file = path.join(fixtures, 'App.qml')
    const diags = lang.getDiagnostics(file)
    const errors = diags.filter(item => item.severity === 'error')
    assert.deepStrictEqual(errors, [])
})

test('completes engine properties', () => {
    const lang = service()
    const file = path.join(fixtures, 'App.qml')
    const text = require('fs').readFileSync(file, 'utf8')
    const offset = text.indexOf('width:')
    const items = lang.getCompletions(file, text, offset)
    hasLabel(items, 'width')
    hasLabel(items, 'visible')
    hasLabel(items, 'anchors')
})

test('completes custom qml members', () => {
    const lang = service()
    const file = path.join(fixtures, 'App.qml')
    const text = require('fs').readFileSync(file, 'utf8')
    const offset = text.indexOf('onTapped:')
    const items = lang.getCompletions(file, text, offset)
    hasLabel(items, 'onTapped')
    hasLabel(items, 'count')
    hasLabel(items, 'bump')
})

test('completes same-directory component', () => {
    const lang = service()
    const file = path.join(fixtures, 'App.qml')
    const text = require('fs').readFileSync(file, 'utf8')
    const offset = text.indexOf('MyBox')
    const items = lang.getCompletions(file, text, offset)
    hasLabel(items, 'MyBox')
})

test('completes anchors group members', () => {
    const lang = service()
    const file = path.join(fixtures, 'App.qml')
    const text = require('fs').readFileSync(file, 'utf8')
    const offset = text.indexOf('anchors.fill') + 'anchors.'.length
    const items = lang.getCompletions(file, text, offset)
    hasLabel(items, 'fill')
    hasLabel(items, 'centerIn')
})

test('completes import modules', () => {
    const lang = service()
    const file = path.join(fixtures, 'App.qml')
    const text = 'import Qt'
    const items = lang.getCompletions(file, text, text.length)
    hasLabel(items, 'QtQuick')
    hasLabel(items, 'QtQml')
})

test('completes property types', () => {
    const lang = service()
    const file = path.join(fixtures, 'App.qml')
    const intText = 'import QtQuick\n\nItem {\n    property in'
    const intItems = lang.getCompletions(file, intText, intText.length)
    hasLabel(intItems, 'int')
    const typeText = 'import QtQuick\n\nItem {\n    property I'
    const typeItems = lang.getCompletions(file, typeText, typeText.length)
    hasLabel(typeItems, 'Item')
})

test('engine index json matches live types', () => {
    const { loadEngine, serializeEngine, LanguageService } = require('../../compiler/language')
    const fs = require('fs')
    const os = require('os')
    const live = loadEngine(enginePath)
    const data = serializeEngine(live)
    const tmp = path.join(os.tmpdir(), 'jqml-engine-index.json')
    fs.writeFileSync(tmp, JSON.stringify(data))
    const lang = new LanguageService({ engineIndexPath: tmp, enginePath: '' })
    assert.ok(!lang.engineLoadError, lang.engineLoadError && lang.engineLoadError.message)
    assert.ok(lang.engineTypes.has('QtQuick.Item'))
    lang.indexQmlFile(path.join(fixtures, 'MyBox.qml'))
    lang.indexQmlFile(path.join(fixtures, 'App.qml'))
    const file = path.join(fixtures, 'App.qml')
    const text = fs.readFileSync(file, 'utf8')
    const offset = text.indexOf('anchors.fill') + 'anchors.'.length
    const items = lang.getCompletions(file, text, offset)
    hasLabel(items, 'fill')
})

test('qmldir module type is available after import', () => {
    const lang = service()
    const file = path.join(fixtures, 'ModuleUse.qml')
    const text = 'import QtQuick\nimport testmod\n\nItem {\n    Card {\n    }\n}\n'
    const diags = lang.getDiagnostics(file, text)
    const errors = diags.filter(item => item.severity === 'error')
    assert.deepStrictEqual(errors, [], errors.map(item => item.message).join(' | '))
    const offset = positionToOffset(text, { line: 4, character: 4 })
    const items = lang.getCompletions(file, text, offset)
    hasLabel(items, 'Card')
})

test('absolute include path is not prefixed with config directory', () => {
    const { resolveConfigRef } = require('../../compiler/language')
    const configDir = path.join('D:', 'Work', 'IT', 'ImtCore', 'Qml', 'web')
    const includeAbs = path.join('D:', 'Work', 'IT', 'ImtCore', 'AuxInclude', 'generation_info.json')
    const resolved = resolveConfigRef(configDir, includeAbs)
    assert.strictEqual(path.normalize(resolved).toLowerCase(), path.normalize(includeAbs).toLowerCase())
    assert.ok(resolved.toLowerCase().indexOf(path.join('web', 'd:').toLowerCase()) < 0, resolved)
})

test('env include path is not prefixed with config directory', () => {
    const { resolveConfigRef } = require('../../compiler/language')
    process.env.JQML_TEST_ABSROOT = path.join('D:', 'Work', 'IT', 'ImtCore')
    const configDir = path.join('D:', 'Work', 'IT', 'ImtCore', 'Qml', 'web')
    const resolved = resolveConfigRef(configDir, '${JQML_TEST_ABSROOT}/AuxInclude/generation_info.json')
    delete process.env.JQML_TEST_ABSROOT
    const expected = path.normalize(path.join('D:', 'Work', 'IT', 'ImtCore', 'AuxInclude', 'generation_info.json'))
    assert.strictEqual(path.normalize(resolved), expected)
})

test('loadConfig with absolute path keeps inherited json absolute', () => {
    const fs = require('fs')
    const os = require('os')
    const tmp = fs.mkdtempSync(path.join(os.tmpdir(), 'jqml-cfg-'))
    const childDir = path.join(tmp, 'child')
    fs.mkdirSync(childDir)
    const child = path.join(tmp, 'nested.json')
    const root = path.join(tmp, 'root.json')
    fs.writeFileSync(child, JSON.stringify({ includes: [], dirs: [childDir.replace(/\\/g, '/')] }))
    fs.writeFileSync(root, JSON.stringify({ includes: [child.replace(/\\/g, '/')], dirs: [] }))
    const lang = new LanguageService({ enginePath })
    lang.loadConfig(root)
    assert.ok(lang.configDirs.some(dir => path.normalize(dir) === path.normalize(childDir)), String(lang.configDirs))
})

test('infers IMTCOREDIR from ProLife config without process env', () => {
    const { inferProjectEnv } = require('../../compiler/language')
    const configPath = path.resolve(__dirname, '../../../../../ProLife/Qml/Include/prolifeqml/prolife.json')
    if (!require('fs').existsSync(configPath)) return
    const saved = {
        IMTCOREDIR: process.env.IMTCOREDIR,
        IMTCOREDIR_BUILD: process.env.IMTCOREDIR_BUILD,
        PROLIFEDIR: process.env.PROLIFEDIR,
        TARGETNAME: process.env.TARGETNAME,
    }
    delete process.env.IMTCOREDIR
    delete process.env.IMTCOREDIR_BUILD
    delete process.env.PROLIFEDIR
    delete process.env.TARGETNAME
    try {
        const inferred = inferProjectEnv([configPath])
        assert.ok(inferred.IMTCOREDIR, 'IMTCOREDIR')
        assert.ok(inferred.PROLIFEDIR, 'PROLIFEDIR')
        assert.ok(inferred.TARGETNAME, 'TARGETNAME')
        assert.ok(require('fs').existsSync(path.join(inferred.IMTCOREDIR, 'Qml', 'web', 'imtcore.json')))
        assert.ok(require('fs').existsSync(path.join(inferred.PROLIFEDIR, 'Qml', 'Include', 'prolifeqml', 'prolife.json')))
    } finally {
        for (const key of Object.keys(saved)) {
            if (saved[key] == null) delete process.env[key]
            else process.env[key] = saved[key]
        }
    }
})

test('ProLife config without env finds Acf and Item anchors', () => {
    const fs = require('fs')
    const configPath = path.resolve(__dirname, '../../../../../ProLife/Qml/Include/prolifeqml/prolife.json')
    const webPath = path.resolve(__dirname, '../../../../../ProLife/Qml/Include/prolifeqml/ProLifeWeb.qml')
    if (!fs.existsSync(configPath) || !fs.existsSync(webPath)) return
    const saved = {
        IMTCOREDIR: process.env.IMTCOREDIR,
        IMTCOREDIR_BUILD: process.env.IMTCOREDIR_BUILD,
        PROLIFEDIR: process.env.PROLIFEDIR,
        TARGETNAME: process.env.TARGETNAME,
    }
    delete process.env.IMTCOREDIR
    delete process.env.IMTCOREDIR_BUILD
    delete process.env.PROLIFEDIR
    delete process.env.TARGETNAME
    try {
        const lang = new LanguageService({ enginePath })
        lang.setWorkspaceFolders([path.resolve(__dirname, '../../../../../ProLife')])
        lang.loadConfig(configPath)
        lang.indexQmlFile(webPath)
        lang.indexQmlFile(path.resolve(__dirname, '../../../../../ProLife/Qml/Include/prolifeqml/ProLifeMain.qml'))
        const hasAcf = [...lang.localTypes.values()].some(entry => entry.module === 'Acf')
        assert.ok(hasAcf, 'Acf module was not indexed from inferred IMTCOREDIR')
        const diags = lang.getDiagnostics(webPath, fs.readFileSync(webPath, 'utf8'))
        const messages = diags.map(item => item.message)
        assert.ok(!messages.some(msg => msg === 'Acf is not found'), messages.join(' | '))
        assert.ok(!messages.some(msg => msg === 'com.imtcore.imtqml is not found'), messages.join(' | '))
        assert.ok(!messages.some(msg => msg === 'anchors is not found'), messages.join(' | '))
    } finally {
        for (const key of Object.keys(saved)) {
            if (saved[key] == null) delete process.env[key]
            else process.env[key] = saved[key]
        }
    }
})

test('does not complete engine names starting with __', () => {
    const lang = service()
    const file = path.join(fixtures, 'App.qml')
    const text = require('fs').readFileSync(file, 'utf8')
    const offset = text.indexOf('width:')
    const items = lang.getCompletions(file, text, offset)
    const hidden = labels(items).filter(name => name.slice(0, 2) === '__' || name.split('.').some(part => part.slice(0, 2) === '__'))
    assert.deepStrictEqual(hidden, [])
    const importItems = lang.getCompletions(file, 'import Qt', 'import Qt'.length)
    const hiddenModules = labels(importItems).filter(name => name.split('.').some(part => part.slice(0, 2) === '__'))
    assert.deepStrictEqual(hiddenModules, [])
})

test('definition of qml property method and signal', () => {
    const lang = service()
    const file = path.join(fixtures, 'App.qml')
    const text = require('fs').readFileSync(file, 'utf8')
    const boxFile = path.join(fixtures, 'MyBox.qml')

    const countLocs = lang.getDefinition(file, text, text.indexOf('box.count') + 'box.'.length)
    assert.ok(countLocs.length, 'count definition')
    assert.strictEqual(path.normalize(countLocs[0].filePath).toLowerCase(), path.normalize(boxFile).toLowerCase())
    assert.ok(countLocs[0].range.start.line >= 6)

    const tappedLocs = lang.getDefinition(file, text, text.indexOf('onTapped') + 2)
    assert.ok(tappedLocs.length, 'tapped definition')
    assert.strictEqual(path.normalize(tappedLocs[0].filePath).toLowerCase(), path.normalize(boxFile).toLowerCase())

    const bumpText = 'import QtQuick\n\nItem {\n    MyBox { id: box }\n    Component.onCompleted: box.bump()\n}\n'
    const bumpFile = path.join(fixtures, 'BumpUse.qml')
    const bumpLocs = lang.getDefinition(bumpFile, bumpText, bumpText.indexOf('bump'))
    assert.ok(bumpLocs.length, 'bump definition')
    assert.strictEqual(path.normalize(bumpLocs[0].filePath).toLowerCase(), path.normalize(boxFile).toLowerCase())

    const typeLocs = lang.getDefinition(file, text, text.indexOf('MyBox'))
    assert.ok(typeLocs.length, 'MyBox type definition')
    assert.strictEqual(path.normalize(typeLocs[0].filePath).toLowerCase(), path.normalize(boxFile).toLowerCase())
})

test('definition of inherited engine property goes to engine file', () => {
    const lang = service()
    const file = path.join(fixtures, 'App.qml')
    const text = require('fs').readFileSync(file, 'utf8')
    const locs = lang.getDefinition(file, text, text.indexOf('width:'))
    assert.ok(locs.length, 'width definition')
    assert.ok(/Item\.js$/i.test(locs[0].filePath.replace(/\\/g, '/')), locs[0].filePath)
})

test('follows typed property to nested method', () => {
    const lang = service()
    const providerFile = path.join(fixtures, 'DeepProvider.qml')
    const file = path.join(fixtures, 'DeepAccess.qml')
    lang.indexQmlFile(providerFile)
    lang.indexQmlFile(file)
    const text = require('fs').readFileSync(file, 'utf8')
    const offset = text.indexOf('designProvider.setDesignSchema') + 'designProvider.'.length
    const locs = lang.getDefinition(file, text, offset)
    assert.ok(locs.length, 'setDesignSchema definition')
    assert.strictEqual(path.normalize(locs[0].filePath).toLowerCase(), path.normalize(providerFile).toLowerCase())

    const nestedOffset = text.indexOf('window.designProvider.setDesignSchema') + 'window.designProvider.'.length
    const nestedLocs = lang.getDefinition(file, text, nestedOffset)
    assert.ok(nestedLocs.length, 'window.designProvider.setDesignSchema definition')
    assert.strictEqual(path.normalize(nestedLocs[0].filePath).toLowerCase(), path.normalize(providerFile).toLowerCase())

    const items = lang.getCompletions(file, text, text.indexOf('designProvider.setDesignSchema') + 'designProvider.'.length)
    hasLabel(items, 'setDesignSchema')

    const hover = lang.getHover(file, text, offset)
    assert.ok(hover && hover.detail, 'hover')
    assert.ok(String(hover.detail).indexOf('setDesignSchema') >= 0, hover.detail)
})

test('Gallery designProvider.setDesignSchema goes to DesignSchemaProvider', () => {
    const fs = require('fs')
    const web = path.resolve(__dirname, '../../../../Impl/ImtControlsGallery/Qml/controlsgalleryqml/ImtControlsGalleryWeb.qml')
    const provider = path.resolve(__dirname, '../../../../Qml/imtgui/Application/DesignSchemaProvider.qml')
    const qmldir = path.resolve(__dirname, '../../../../Qml/imtgui/qmldir')
    if (!fs.existsSync(web) || !fs.existsSync(provider) || !fs.existsSync(qmldir)) return
    const lang = new LanguageService({ enginePath })
    lang.indexQmldir(qmldir)
    lang.indexQmlFile(provider)
    lang.indexQmlFile(path.resolve(__dirname, '../../../../Impl/ImtControlsGallery/Qml/controlsgalleryqml/ImtControlsGalleryMain.qml'))
    lang.indexQmlFile(web)
    const text = fs.readFileSync(web, 'utf8')
    const marker = 'designProvider.setDesignSchema'
    const offset = text.indexOf(marker) + 'designProvider.'.length
    const locs = lang.getDefinition(web, text, offset)
    assert.ok(locs.length, 'setDesignSchema definition in gallery')
    assert.ok(/DesignSchemaProvider\.qml$/i.test(String(locs[0].filePath).replace(/\\/g, '/')), locs[0].filePath)
})

console.log('')
console.log(passed + ' passed, ' + failed + ' failed')
if (failed) process.exit(1)

