import QtQuick

Item {
    width: 40
    height: 40

    MouseArea {
        id: area
        anchors.fill: parent
        cursorShape: containsPress ? Qt.PointingHandCursor : Qt.ArrowCursor
    }

    Component.onCompleted: {
        console.log("idle", area.containsPress, area.pressed, area.containsMouse)
        console.log("cursor", area.cursorShape)
        Qt.quit()
    }
}
