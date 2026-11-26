// Run: node --test tests/unit
// The border of a Rectangle is a QQuickPen: 1 px and black, drawn once its color or width is written (the same value
// too), while the color is visible and the width rounds to 1 or more. Checked against the pixels of Qt 6.12. On the
// sources and on dist/core.js.
const test = require('node:test')
const assert = require('node:assert/strict')
const fs = require('fs')
const os = require('os')
const path = require('path')
const { compileQml, load, CORE } = require('../bench/lib')
const { sourcesBundle } = require('../sourcesBundle')

const sourcesCore = path.join(fs.mkdtempSync(path.join(os.tmpdir(), 'jqml-border-')), 'sources.js')
fs.writeFileSync(sourcesCore, sourcesBundle())

const DOC = compileQml({ 'Main.qml': `import QtQuick
Item {
    property alias none: none
    property alias colorOnly: colorOnly
    property alias defaultColor: defaultColor
    property alias defaultWidth: defaultWidth
    property alias wide: wide
    property alias zero: zero
    property alias transparent: transparent
    property alias half: half
    property alias later: later
    property alias bound: bound
    property bool on: false
    Rectangle { id: none }
    Rectangle { id: colorOnly; border.color: "red" }
    Rectangle { id: defaultColor; border.color: "black" }
    Rectangle { id: defaultWidth; border.width: 1 }
    Rectangle { id: wide; border.width: 2 }
    Rectangle { id: zero; border.color: "red"; border.width: 0 }
    Rectangle { id: transparent; border.color: "transparent"; border.width: 3 }
    Rectangle { id: half; border.width: 0.5 }
    Rectangle { id: later }
    Rectangle { id: bound; border.width: on ? 2 : 0 }
}` })

for(let [name, core] of [['Runtime', sourcesCore], ['dist/core.js', CORE]]){
    test(name + ': a border is drawn once its color or width is written, as QQuickPen', () => {
        let { root, JQ, dispose } = load(DOC, { core })
        let drawn = (rect) => rect.border.$valid
        assert.equal(root.none.border.width, 1)
        assert.equal(root.none.border.color, '#000000')
        assert.equal(drawn(root.none), false)
        assert.equal(drawn(root.colorOnly), true)
        assert.equal(drawn(root.defaultColor), true)
        assert.equal(drawn(root.defaultWidth), true)
        assert.equal(drawn(root.wide), true)
        assert.equal(drawn(root.zero), false)
        assert.equal(drawn(root.transparent), false)
        assert.equal(drawn(root.half), true)
        assert.equal(drawn(root.later), false)
        JQ.batch(() => { root.later.border.color = '#00ff00' })
        assert.equal(drawn(root.later), true)
        assert.equal(drawn(root.bound), false)
        JQ.batch(() => { root.on = true })
        assert.equal(drawn(root.bound), true)
        JQ.batch(() => { root.on = false })
        assert.equal(drawn(root.bound), false)
        dispose()
    })
}
