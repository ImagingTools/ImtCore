import QtQuick

Item {
    id: root
    width: 320
    height: 200

    Flickable {
        id: flick
        width: 180
        height: 100
        contentWidth: label.contentWidth + 40
        contentHeight: label.contentHeight + 30
        interactive: false

        Text {
            id: label
            width: 140
            text: "Flickable text with several words to force wrapping on one side"
            wrapMode: Text.WordWrap
            font.pixelSize: 16
        }
    }

    Component.onCompleted: {
        console.log("wd134", "flickable-text", flick.contentWidth, flick.contentHeight, label.contentWidth, label.contentHeight)
        Qt.quit()
    }
}
