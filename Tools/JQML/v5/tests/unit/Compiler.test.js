// Run: node --test tests/unit
// Names the compiler resolves as Qt does, without false warnings: the functions of a Connections are its methods as
// well as its handlers; a group in an attached group (Drag.hotSpot.x); the types only the C++ of the desktop host
// registers in com.imtcore.imtqml. An unknown name still warns.
const test = require('node:test')
const assert = require('node:assert/strict')
const fs = require('fs')
const os = require('os')
const path = require('path')
const { compile } = require('../../compiler/index')

function build(main){
    let root = fs.mkdtempSync(path.join(os.tmpdir(), 'jqml-compiler-'))
    try {
        let entry = path.join(root, 'app', 'Main.qml')
        fs.mkdirSync(path.dirname(entry), { recursive: true })
        fs.writeFileSync(entry, main)
        let output = path.join(root, 'out')
        let result = compile({ mode: 'js', entry, output, name: 'index' })
        return { warnings: result.warnings, code: fs.readFileSync(path.join(output, 'index.js'), 'utf8') }
    } finally {
        fs.rmSync(root, { recursive: true, force: true })
    }
}

const connections = (extra) => `import QtQuick 2.15
Item {
    id: root
    signal ping(int n)
    signal closed()
    Connections {
        target: root
        function onPing(n){ console.log(n) }
        function onClosed(){ onPing(0) }
        ${extra}
    }
}
`

test('a function of a Connections is called by its name from another one, as a method', () => {
    let { warnings, code } = build(connections(''))
    assert.deepEqual(warnings, [])
    assert.match(code, /\$self\.onPing\(0\)/)
    assert.match(code, /\$handle\("ping"/)
    assert.match(code, /\$handle\("closed"/)
})

test('Component.onCompleted in a Connections leaves its functions handlers', () => {
    let { warnings, code } = build(connections('Component.onCompleted: console.log("done")'))
    assert.deepEqual(warnings, [])
    assert.match(code, /\$handle\("ping"/)
})

test('Drag.hotSpot.x and Drag.YAxis compile without warnings', () => {
    let { warnings } = build(`import QtQuick 2.15
Item {
    MouseArea { drag.axis: Drag.YAxis }
    Rectangle {
        Drag.active: true
        Drag.hotSpot.x: width / 2
        Drag.hotSpot.y: 5
    }
    DropArea { onEntered: console.log(drag.x) }
}
`)
    assert.deepEqual(warnings, [])
})

test('a type the C++ host registers in com.imtcore.imtqml does not warn in a file importing the module', () => {
    let code = (imports) => `import QtQuick 2.15
${imports}
Item {
    function connect(){
        if(Qt.platform.os !== "web") NetworkEventInterceptor.unauthorized.connect(connect)
    }
}
`
    assert.deepEqual(build(code('import com.imtcore.imtqml 1.0')).warnings, [])
    assert.equal(build(code('')).warnings.length, 1)
})

test('an unknown name warns', () => {
    let { warnings } = build(`import QtQuick 2.15
Item { function f(){ return notDeclaredAnywhere } }
`)
    assert.equal(warnings.length, 1)
    assert.match(warnings[0], /notDeclaredAnywhere is not found/)
})
