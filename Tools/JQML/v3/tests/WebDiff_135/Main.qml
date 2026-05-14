import QtQuick

Item {
    id: root
    width: 320
    height: 200

    Text {
        id: label
        width: 180
        text: "<b>Rich</b> text sample with
multiple lines and a custom line height"
        textFormat: Text.RichText
        wrapMode: Text.WordWrap
        lineHeight: 1.6
        font.pixelSize: 17
    }

    Component.onCompleted: {
        console.log("wd135", "text-lines", label.contentWidth, label.contentHeight, label.width, label.height, label.lineHeight)
        Qt.quit()
    }
}
