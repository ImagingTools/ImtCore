import QtQuick 2.0

Item {
    id: root
    width: 1200
    height: 760

    Item {
        id: shell
        anchors.fill: parent

        Item {
            id: application
            anchors.fill: parent
            property bool serverReady: true
        }
    }

    Component.onCompleted: {
        console.log(
            "prolife20",
            root.width,
            root.height,
            shell.x,
            shell.y,
            shell.width,
            shell.height,
            application.x,
            application.y,
            application.width,
            application.height,
            application.serverReady
        )
        Qt.quit()
    }
}
