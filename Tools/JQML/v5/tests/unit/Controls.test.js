// Run: node --test tests/unit
// QtQuick.Controls and QtQuick.Templates: the types the runtime registers, and the imports that make them visible, as
// QQmlImports of Qt 6.12 resolves them (checked with qml of Qt 6.12):
//  - a control is a type only with `import QtQuick.Controls` (or .Basic, or Templates); a project type of the same name
//    (imtcontrols Button) is used without it;
//  - QtQuick.Controls has no types of its own: its controls are those of QtQuick.Controls.Basic, which it imports. A
//    module the document imports itself comes before them, whatever the order of the imports; QtQuick.Controls.Basic
//    imported directly goes by the order of the imports;
//  - the document's directory comes after every module (a Button.qml next to it loses to QtQuick.Controls);
//  - `import QtQuick.Templates as T`: T.Button is the control without a style.
const test = require('node:test')
const assert = require('node:assert/strict')
const fs = require('fs')
const os = require('os')
const path = require('path')
const { compileQml, load, applicationPart, V5 } = require('../bench/lib')
const modules = require('../../Runtime/Qt/modules')

test('every control of QtQuick.Controls and QtQuick.Templates is a type of the runtime', () => {
    const jq = require('../../Runtime/index')
    for(let name of modules.CONTROLS) assert.ok(jq.types[name], 'QtQuick.Controls ' + name)
    for(let name of modules.TEMPLATES) assert.ok(jq.types['T.' + name], 'QtQuick.Templates ' + name)
    // the templates have no style: the Basic Button is a T.Button that makes a background
    assert.ok(jq.types.Button.prototype instanceof jq.types['T.Button'])
    for(let name of modules.CONTROLS) assert.ok(modules.IMPORTED_TYPES.has(name), name + ' is visible only through an import')
})

// compiles Main.qml with files next to it and modules (name -> { 'qmldir': ..., 'X.qml': ... }) of a config
function compileApp(main, { files = {}, mods = {} } = {}){
    const { compile } = require(path.join(V5, 'compiler/index'))
    let root = fs.mkdtempSync(path.join(os.tmpdir(), 'jqml-controls-'))
    try {
        fs.mkdirSync(path.join(root, 'app'))
        fs.writeFileSync(path.join(root, 'app', 'Main.qml'), main)
        for(let [name, text] of Object.entries(files)) fs.writeFileSync(path.join(root, 'app', name), text)
        let dirs = []
        for(let [name, content] of Object.entries(mods)){
            let dir = path.join(root, 'mods', name)
            fs.mkdirSync(dir, { recursive: true })
            for(let [file, text] of Object.entries(content)) fs.writeFileSync(path.join(dir, file), text)
            dirs.push(dir)
        }
        let config = path.join(root, 'config.json')
        fs.writeFileSync(config, JSON.stringify({ dirs }))
        let out = path.join(root, 'out')
        let result = compile({ mode: 'js', config, entry: path.join(root, 'app', 'Main.qml'), output: out, name: 'index' })
        return { app: applicationPart(fs.readFileSync(path.join(out, 'index.js'), 'utf8')), warnings: result.warnings }
    } finally {
        fs.rmSync(root, { recursive: true, force: true })
    }
}

const MOD_A = { ModA: { 'qmldir': 'module ModA\nButton 1.0 Button.qml\n', 'Button.qml': 'import QtQuick\nItem { property string from: "ModA" }\n' } }
// which Button the document made: the one of a module (its `from`), or a control of the runtime (its class)
function whichButton(imports, options = {}){
    let { app, warnings } = compileApp(imports + '\nItem {\n    property var made: b\n    Button { id: b }\n}\n', options)
    let { root, JQ, dispose } = load(app)
    try {
        let b = root.made
        if(b.from !== undefined) return b.from
        if(b instanceof JQ.types.Button) return 'Controls'
        if(b instanceof JQ.types['T.Button']) return 'Templates'
        return 'Item'
    } finally {
        dispose()
    }
}

test('a control is a type only through an import of its module', () => {
    assert.equal(whichButton('import QtQuick\nimport QtQuick.Controls'), 'Controls')
    assert.equal(whichButton('import QtQuick\nimport QtQuick.Controls.Basic'), 'Controls')
    assert.equal(whichButton('import QtQuick\nimport QtQuick.Templates'), 'Templates')
    // no import: not a type (Qt fails to load; the build falls back to Item with an error)
    let { warnings } = compileApp('import QtQuick\nItem { Button {} }\n')
    assert.ok(warnings.some(w => /type Button is not found|Button is not a type/.test(w)), warnings.join("\n"))
})

test('a module the document imports comes before the controls QtQuick.Controls imports, in either order', () => {
    assert.equal(whichButton('import QtQuick\nimport QtQuick.Controls\nimport ModA', { mods: MOD_A }), 'ModA')
    assert.equal(whichButton('import QtQuick\nimport ModA\nimport QtQuick.Controls', { mods: MOD_A }), 'ModA')
    // QtQuick.Controls.Basic imported itself: the last import wins
    assert.equal(whichButton('import QtQuick\nimport ModA\nimport QtQuick.Controls.Basic', { mods: MOD_A }), 'Controls')
    assert.equal(whichButton('import QtQuick\nimport QtQuick.Controls.Basic\nimport ModA', { mods: MOD_A }), 'ModA')
    // a module without the import: its Button is not seen, the control is
    assert.equal(whichButton('import QtQuick\nimport QtQuick.Controls', { mods: MOD_A }), 'Controls')
})

test('the directory of the document comes after QtQuick.Controls', () => {
    let files = { 'Button.qml': 'import QtQuick\nItem { property string from: "dir" }\n' }
    assert.equal(whichButton('import QtQuick\nimport QtQuick.Controls', { files }), 'Controls')
    assert.equal(whichButton('import QtQuick', { files }), 'dir')
})

test('qualified imports: T.Button is the template, C.Button the control, and the plain name stays the module one', () => {
    let { app } = compileApp(`import QtQuick
import QtQuick.Controls as C
import QtQuick.Templates as T
import ModA
Item {
    property var styled: s
    property var template: t
    property var plain: p
    C.Button { id: s }
    T.Button { id: t }
    Button { id: p }
}
`, { mods: MOD_A })
    let { root, JQ, dispose } = load(app)
    try {
        assert.ok(root.styled instanceof JQ.types.Button)
        assert.ok(root.template instanceof JQ.types['T.Button'] && !(root.template instanceof JQ.types.Button))
        assert.equal(root.template.background, null)
        assert.ok(root.styled.background)
        assert.equal(root.plain.from, 'ModA')
    } finally {
        dispose()
    }
})

test('Qt.createComponent takes a control from QtQuick.Controls (the Basic one it imports)', () => {
    let app = compileQml({ 'Main.qml': `import QtQuick
Item {
    function make() {
        let c = Qt.createComponent("QtQuick.Controls", "Button")
        return [c.status, c.createObject(this)]
    }
}` }, { allowWarnings: true })
    let { root, JQ, dispose } = load(app)
    try {
        let [status, button] = root.make()
        assert.equal(status, 1)
        assert.ok(button instanceof JQ.types.Button)
    } finally {
        dispose()
    }
})
