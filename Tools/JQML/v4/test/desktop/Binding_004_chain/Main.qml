import QtQuick

// A write to `a` updates `b`, then `c`, and runs `onBChanged`, before the next line.
Item {
    id: root
    property int a: 1
    property int b: a + 5
    property int c: b + 1
    property int seen: 0

    onBChanged: seen = b

    Component.onCompleted: {
        a = 20
        console.log("b", b)
        console.log("c", c)
        console.log("seen", seen)
        Qt.quit()
    }
}
