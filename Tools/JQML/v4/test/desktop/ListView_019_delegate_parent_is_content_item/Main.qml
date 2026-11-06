import QtQuick

Item {
    width: 300
    height: 300

    property var results: ({})

    ListView {
        id: vertical
        width: 200
        height: 60
        model: 5
        delegate: Item {
            width: 50
            height: 20
            Component.onCompleted: {
                if (index === 0) {
                    results["vertical"] = [parent === vertical.contentItem, parent.parent === vertical]
                }
            }
        }
    }

    ListView {
        id: horizontal
        y: 100
        width: 120
        height: 40
        orientation: ListView.Horizontal
        model: 4
        delegate: Item {
            width: 50
            height: 40
            Component.onCompleted: {
                if (index === 0) {
                    results["horizontal"] = [parent === horizontal.contentItem, parent.parent === horizontal]
                }
            }
        }
    }

    GridView {
        id: grid
        y: 200
        width: 100
        height: 60
        cellWidth: 50
        cellHeight: 30
        model: 6
        delegate: Rectangle {
            width: parent.parent.cellWidth
            height: parent.parent.cellHeight
            Component.onCompleted: {
                if (index === 0) results["grid"] = [parent.parent === grid, width, height]
            }
        }
    }

    Timer {
        interval: 100
        running: true
        onTriggered: {
            console.log("vertical", results["vertical"])
            console.log("horizontal", results["horizontal"])
            console.log("grid", results["grid"])
            console.log("vertical content", vertical.contentItem.width, vertical.contentItem.height, vertical.contentItem.x, vertical.contentItem.y)
            console.log("horizontal content", horizontal.contentItem.width, horizontal.contentItem.height)
            vertical.contentY = 20
            console.log("scrolled content", vertical.contentItem.x, vertical.contentItem.y)
            Qt.quit()
        }
    }
}
