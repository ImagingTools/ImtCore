import QtQuick

Item {
    width: 300
    height: 200

    // A text whose width comes from its anchors wraps at that width: its height is the height of the lines.
    Item {
        id: box
        width: 120
        height: 100

        Text {
            id: label
            anchors.left: parent.left
            anchors.right: parent.right
            wrapMode: Text.WordWrap
            font.pixelSize: 20
            text: "Counters in the system"
        }
    }

    Timer {
        interval: 50
        running: true
        onTriggered: {
            console.log("width", label.width)
            console.log("lines", label.lineCount > 1)
            console.log("tall", label.height > 30, label.contentHeight > 30)
            Qt.quit()
        }
    }
}
