import QtQuick

Item {
    id: root
    width: 300
    height: 200

    Rectangle {
        id: child
        anchors.fill: parent
        color: "purple"

        Component.onCompleted: {
            console.log("before", width, height)
        }
    }

    Timer {
        interval: 10
        running: true
        repeat: false
        onTriggered: {
            root.width = 500
            root.height = 400
            console.log("after", child.width, child.height)
            Qt.quit()
        }
    }
}
