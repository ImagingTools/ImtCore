import QtQuick 2.15

// A standalone fixture for the --obfuscate-light / --no-obfuscate-light tests: a local variable that only mangleLocals (not
// minifyScript, which never renames) could possibly touch, and an id read by the same binding, which it must never touch.
Item {
    id: root
    width: 300
    height: 200
    property int total: 0

    function sum(upTo) {
        var acc = 0
        for (var i = 0; i <= upTo; i++) acc = acc + i
        return acc + root.width
    }

    Component.onCompleted: {
        var n = sum(3)
        total = n
    }
}
