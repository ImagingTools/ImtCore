import QtQuick
Item {
    id: root
    property int next: 5
    Box { id: box; chained: root.next }
    Component.onCompleted: {
        console.log("bound", box.chained, box.region.x, box.region.y)
        root.next = 9
        console.log("updated", box.chained, box.region.x, box.region.y)
        box.chained = 11
        root.next = 15
        console.log("written", box.chained, box.region.x, box.region.y)
        Qt.quit()
    }
}
