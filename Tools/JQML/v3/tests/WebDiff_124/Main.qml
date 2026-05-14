import QtQuick

Item {
    id: root
    width: 260
    height: 140

    TextInput {
        id: input
        width: 200
        height: 40
        text: "alpha beta"
        selectByMouse: false
    }

    Component.onCompleted: {
        input.select(0, 5)
        console.log("wd124", "textinput-state", input.text, input.selectionStart, input.selectionEnd, input.selectedText, input.contentWidth)
        Qt.quit()
    }
}
