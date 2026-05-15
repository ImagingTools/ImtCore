import QtQuick

Item {
    id: root
    width: 300
    height: 150

    TextEdit {
        id: edit
        width: 240
        height: 60
        text: "selection works here"
        selectByMouse: false
    }

    Component.onCompleted: {
        edit.select(0, 9)
        console.log("wd128", "textedit-select", edit.selectionStart, edit.selectionEnd, edit.selectedText, edit.contentWidth)
        Qt.quit()
    }
}
