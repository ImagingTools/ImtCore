import QtQuick

Item {
    id: root
    width: 300
    height: 200

    Rectangle {
        id: panel
        anchors {
            left: parent.left
            top: parent.top
            margins: 12
        }
        width: 100
        height: 60
        color: "white"
        border {
            width: 3
            color: "red"
        }

        Text {
            id: label
            anchors.centerIn: parent
            text: "A"
            font {
                pixelSize: 20
                bold: true
            }
        }
    }

    Rectangle {
        id: filler
        anchors {
            fill: parent
            margins: 50
        }
        color: "transparent"
    }

    Component.onCompleted: {
        console.log("panel", panel.x, panel.y, panel.border.width, Qt.colorEqual(panel.border.color, "red"))
        console.log("font", label.font.pixelSize, label.font.bold)
        console.log("filler", filler.x, filler.y, filler.width, filler.height)
        Qt.quit()
    }
}
