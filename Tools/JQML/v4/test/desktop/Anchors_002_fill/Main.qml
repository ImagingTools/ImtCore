import QtQuick

Item {
    id: root
    width: 200
    height: 80

    Rectangle {
        id: panel
        anchors.fill: parent
        anchors.margins: 10
    }

    Component.onCompleted: {
        console.log("panel", panel.x, panel.y, panel.width, panel.height)
        Qt.quit()
    }
}
