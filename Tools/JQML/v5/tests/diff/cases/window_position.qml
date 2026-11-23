import QtQuick
import QtQuick.Window

// x and y of a window are its place on the screen: its scene starts at 0, 0 (QmlAppTemplate puts its window at 64, 64)
Window {
    id: w
    x: 64; y: 30
    width: 200; height: 100
    property real seenX: x
    Item { id: inner; x: 10; y: 5; width: 20; height: 20 }
    onXChanged: console.log("x changed", x)
    Component.onCompleted: {
        let p = inner.mapToItem(null, 0, 0)
        let q = inner.mapToGlobal(0, 0)
        console.log(x, y, seenX, p.x, p.y, q.x - x, q.y - y)
        x = 80
        console.log(x, seenX, inner.mapToItem(null, 0, 0).x)
        Qt.quit()
    }
}
