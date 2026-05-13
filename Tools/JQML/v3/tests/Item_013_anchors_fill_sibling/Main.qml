import QtQuick

Item {
    width: 300
    height: 300

    Item {
        id: ref
        x: 40
        y: 60
        width: 160
        height: 100
    }

    Item {
        id: box
        anchors.fill: ref
        anchors.margins: 5
    }

    Component.onCompleted: {
        console.log("fillSibling1", box.x, box.y, box.width, box.height)
        box.anchors.leftMargin = 1
        box.anchors.topMargin = 2
        box.anchors.rightMargin = 3
        box.anchors.bottomMargin = 4
        console.log("fillSibling2", box.x, box.y, box.width, box.height)
        Qt.quit()
    }
}
