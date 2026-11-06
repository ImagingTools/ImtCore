import QtQuick

Item {
    width: 200
    height: 100

    ListModel {
        id: rows
        ListElement { title: "a" }
        ListElement { title: "b" }
        ListElement { title: "c" }
    }

    ListView {
        id: view
        anchors.fill: parent
        model: rows
        contentWidth: 600
        clip: true

        delegate: Rectangle {
            id: row
            width: 600
            height: 30
            color: "red"

            Rectangle {
                id: far
                x: 500
                width: 10
                height: 10
            }
        }
    }

    Timer {
        interval: 100
        running: true
        onTriggered: {
            var row = view.itemAtIndex(1)
            console.log("before", row.mapToItem(view, 0, 0).x, row.width, view.contentWidth)
            view.contentX = 150
            console.log("contentX", view.contentX)
            console.log("row", row.mapToItem(view, 0, 0).x, row.width)
            console.log("far", row.children[0].mapToItem(view, 0, 0).x)
            Qt.quit()
        }
    }
}
