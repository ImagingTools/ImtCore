import QtQuick

// A binding that is an object literal without parentheses ({ "a": x }), and a block of statements ({ return x })
Item {
    id: root
    property int n: 3
    readonly property var map: {
        "one": root.n,
        "two": root.n * 2
    }
    property var block: { let k = n + 1; return k }
    property int plain: { n * 10 }
    Component.onCompleted: { console.log(map.one, map.two, block, plain); n = 4; console.log(map.two, block, plain); Qt.quit() }
}
