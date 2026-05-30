import QtQuick

Item {
    id: root
    width: 300
    height: 180

    Text {
        id: label
        width: 220
        text: "Rich text sample with <b>bold</b>, <i>italic</i> and wrapping words"
        textFormat: Text.RichText
        wrapMode: Text.WordWrap
        font.pixelSize: 18
    }

    Component.onCompleted: {
        console.log("wd133", "text-rich", label.contentWidth, label.contentHeight, label.width, label.height)
        Qt.quit()
    }
}
