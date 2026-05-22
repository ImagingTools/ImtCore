import QtQuick

Item {
    id: root
    width: 280
    height: 200

    GridView {
        id: grid
        width: 200
        height: 150
        cellWidth: 40
        cellHeight: 50
        model: [10, 20, 30, 40, 50]
        delegate: Rectangle {
            width: 40
            height: 50
            property int valueCopy: modelData
        }
    }

    Component.onCompleted: {
        grid.positionViewAtBeginning()
        var first = grid.itemAtIndex(0)
        var second = grid.itemAtIndex(1)
        console.log("wd127", "gridview-array", grid.count, first ? first.x : -1, first ? first.y : -1, second ? second.x : -1, second ? second.y : -1)
        Qt.quit()
    }
}
