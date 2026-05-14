import QtQuick

Item {
    id: root
    width: 240
    height: 180

    GridView {
        id: grid
        width: 180
        height: 120
        cellWidth: 45
        cellHeight: 30
        currentIndex: 3
        model: 6
        delegate: Rectangle {
            width: 45
            height: 30
            property int itemIndex: index
        }
    }

    Component.onCompleted: {
        var item = grid.itemAtIndex(3)
        console.log("wd125", "gridview-number", grid.count, grid.currentIndex, item ? item.x : -1, item ? item.y : -1)
        Qt.quit()
    }
}
