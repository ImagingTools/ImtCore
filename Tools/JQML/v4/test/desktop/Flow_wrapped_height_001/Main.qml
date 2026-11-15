import QtQuick

Item {
    width: 400
    height: 300

    // A Flow with a width: its height is the height of all its lines, and a child anchored by it to the centre follows.
    Item {
        anchors.fill: parent

        Flow {
            id: flow
            width: 250
            spacing: 10
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: parent.height / 2 - height / 2

            Rectangle { width: 120; height: 60; color: "red" }
            Rectangle { width: 120; height: 60; color: "green" }
            Rectangle { width: 120; height: 40; color: "blue" }
        }
    }

    Timer {
        interval: 50
        running: true
        onTriggered: {
            console.log("size", flow.width, flow.height, flow.implicitWidth, flow.implicitHeight)
            console.log("y", flow.y)
            Qt.quit()
        }
    }
}
