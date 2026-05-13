import QtQuick

Item {
    id: root
    width: 200
    height: 100

    Item {
        id: box
        width: 40
        height: 20
        anchors.centerIn: parent
    }

    Component.onCompleted: {
        console.log("center1", box.x, box.y)
        root.width = 300
        root.height = 200
        console.log("center2", box.x, box.y)
        Qt.quit()
    }
}
