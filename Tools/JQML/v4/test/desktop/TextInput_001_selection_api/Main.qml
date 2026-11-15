import QtQuick

Item {
    width: 200
    height: 100

    TextInput {
        id: input
        text: "hello world"
    }

    Component.onCompleted: {
        input.selectAll()
        console.log("selectAll", input.selectionStart, input.selectionEnd, input.selectedText, input.cursorPosition)
        input.deselect()
        console.log("deselect", input.selectionStart, input.selectionEnd, JSON.stringify(input.selectedText))
        input.select(0, 5)
        console.log("select", input.selectedText, input.cursorPosition)
        input.insert(5, ",")
        console.log("insert", input.text, input.length)
        input.remove(0, 6)
        console.log("remove", input.text)
        console.log("getText", input.getText(0, 3))
        input.cursorPosition = 2
        console.log("cursor", input.cursorPosition, input.selectionStart, input.selectionEnd)
        input.clear()
        console.log("clear", JSON.stringify(input.text), input.length)
        Qt.quit()
    }
}
