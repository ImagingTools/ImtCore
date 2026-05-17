import QtQuick 2.0

Item {
    id: root
    width: 900
    height: 540

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
            "prolife21-before",
            root.width,
            root.height,
            shell.width,
            shell.height,
            application.width,
            application.height
        )

        root.width = 1280
        root.height = 720

        console.log(
            "prolife21-after",
            root.width,
            root.height,
            shell.width,
            shell.height,
            application.width,
            application.height
        )

        Qt.quit()
    }
}
