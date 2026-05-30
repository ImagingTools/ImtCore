import QtQuick

Item {
    Loader {
        id: host
        width: 150
        height: 70
        sourceComponent: Rectangle {
            anchors.fill: parent
            Rectangle { width: 20 + 18; height: 12 }
        }
    }

    Component.onCompleted: {
        console.log("wd018", "loader-anchors", host.width, host.height)
        Qt.quit()
    }
}
