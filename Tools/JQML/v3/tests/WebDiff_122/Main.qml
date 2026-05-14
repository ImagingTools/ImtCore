import QtQuick

Item {
    id: root
    width: 240
    height: 160

    ListModel {
        id: model
        ListElement { label: "A" }
        ListElement { label: "B" }
        ListElement { label: "C" }
        ListElement { label: "D" }
    }

    GridView {
        id: grid
        width: 200
        height: 120
        cellWidth: 50
        cellHeight: 40
        model: model
        delegate: Rectangle {
            width: 50
            height: 40
            color: index % 2 === 0 ? "#88ccff" : "#cc88ff"
            property string tag: label
        }
    }

    Component.onCompleted: {
        var first = grid.itemAtIndex(0)
        var last = grid.itemAtIndex(3)
        console.log("wd122", "gridview-state", grid.count, first ? first.x : -1, first ? first.y : -1, last ? last.x : -1, last ? last.y : -1)
        Qt.quit()
    }
}
