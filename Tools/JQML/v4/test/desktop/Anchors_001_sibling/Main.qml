import QtQuick

Item {
    id: root
    width: 320
    height: 200

    Rectangle {
        id: box
        width: 40
        height: 20
        x: 15
        y: 8
    }

    Item {
        id: next
        width: 10
        height: 10
        anchors.left: box.right
        anchors.leftMargin: 5
        anchors.top: box.top
    }

    Component.onCompleted: {
        console.log("root", root.width, root.height)
        console.log("box", box.x, box.y, box.width, box.height)
        console.log("next", next.x, next.y, next.width, next.height)
        Qt.quit()
    }
}
