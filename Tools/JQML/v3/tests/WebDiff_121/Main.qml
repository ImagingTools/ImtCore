import QtQuick

Window {
    id: root
    width: 320
    height: 240
    visible: true

    Rectangle {
        anchors.fill: parent
        color: "#334455"
    }

    Component.onCompleted: {
        console.log("wd121", "window-geometry", width, height, visible)
        Qt.quit()
    }
}
