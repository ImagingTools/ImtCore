import QtQuick

Item {
    id: root
    width: 260
    height: 140

    TextEdit {
        id: edit
        width: 220
        height: 80
        text: "the quick brown fox jumps"
        wrapMode: TextEdit.WordWrap
    }

    Component.onCompleted: {
        console.log("wd126", "textedit-metrics", edit.contentWidth, edit.contentHeight, edit.paintedWidth, edit.paintedHeight)
        Qt.quit()
    }
}
