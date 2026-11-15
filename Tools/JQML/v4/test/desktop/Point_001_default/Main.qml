import QtQuick

Item {
    id: root

    property point startPoint
    property size itemSize
    property rect bounds

    Component.onCompleted: {
        console.log("point", startPoint.x, startPoint.y)
        console.log("size", itemSize.width, itemSize.height)
        console.log("rect", bounds.x, bounds.y, bounds.width, bounds.height)
        startPoint = Qt.point(12, 34)
        console.log("set", startPoint.x, startPoint.y)
        Qt.quit()
    }
}
