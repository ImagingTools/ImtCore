import QtQuick

Item {
    width: 120
    height: 80

    Item {
        id: box
        anchors.fill: parent
        anchors.margins: 10
    }

    Component.onCompleted: {
        console.log("fill1", box.x, box.y, box.width, box.height)
        box.anchors.margins = 5
        console.log("fill2", box.x, box.y, box.width, box.height)
        Qt.quit()
    }
}
