import QtQuick 2.0

Item {
    id: root
    width: 1100
    height: 700

    property var appContext: ({ application: [], location: "wss://prolife.example/ws" })
    property string designSchema: ""

    Item {
        id: shell
        anchors.fill: parent

        Item {
            id: application
            anchors.fill: parent
            property bool serverReady: true

            Component.onCompleted: {
                root.designSchema = "Light"
                root.appContext.application = ["ImtCore", "ProLife"]
            }

            function getServerUrl() {
                return root.appContext.location
            }
        }
    }

    Component.onCompleted: {
        console.log(
            "prolife22",
            shell.width,
            shell.height,
            application.width,
            application.height,
            designSchema,
            appContext.application.join("|"),
            application.getServerUrl()
        )
        Qt.quit()
    }
}
