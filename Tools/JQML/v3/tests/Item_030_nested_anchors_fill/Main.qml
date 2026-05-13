import QtQuick

Item {
    id: root
    width: 500
    height: 400

    Rectangle {
        id: container
        anchors.centerIn: parent
        width: 300
        height: 200

        Rectangle {
            id: innerRect
            anchors.fill: parent

            Component.onCompleted: {
                console.log("inner", x, y, width, height)
                console.log("container", container.x, container.y, container.width, container.height)
                Qt.quit()
            }
        }
    }
}
