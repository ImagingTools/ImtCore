import QtQuick 2.15
Item {
    id: root
    property int v: 1
    property int a: root.v + 1
    property int b: root.v + 2
    property alias ip: inner.p
    ip: root.v
    onAChanged: console.log("a", a)
    onBChanged: console.log("b", b)
    Item { id: inner; property int p; onPChanged: console.log("alias p", p) }
    Item { property int c: root.v; onCChanged: console.log("first child", c) }
    Item { property int c: root.v; onCChanged: console.log("second child", c) }
    Item { x: root.v; property int q: root.v; onXChanged: console.log("x", x); onQChanged: console.log("q", q) }
    Component.onCompleted: {
        console.log("start")
        root.v = 5
        console.log("end")
        Qt.quit()
    }
}
