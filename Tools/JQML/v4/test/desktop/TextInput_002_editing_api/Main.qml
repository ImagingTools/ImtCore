import QtQuick

Item {
    width: 300
    height: 100

    TextInput {
        id: t
        text: "hello world"
    }

    function st(tag) {
        console.log(tag, JSON.stringify(t.text), "cur", t.cursorPosition, "sel", t.selectionStart, t.selectionEnd, "undo", t.canUndo, "redo", t.canRedo, "len", t.length)
    }

    Component.onCompleted: {
        st("init")
        t.cursorPosition = 3
        t.insert(0, "AB")
        st("insert before cursor")
        t.cursorPosition = 3
        t.insert(3, "CD")
        st("insert at cursor")
        t.cursorPosition = 1
        t.insert(5, "EF")
        st("insert after cursor")
        t.cursorPosition = 8
        t.remove(2, 5)
        st("remove before cursor")
        t.cursorPosition = 3
        t.remove(2, 5)
        st("remove around cursor")
        t.cursorPosition = 1
        t.remove(4, 6)
        st("remove after cursor")
        t.select(2, 5)
        st("select")
        t.select(5, 2)
        st("select reversed")
        t.cursorPosition = 4
        t.select(1, 3)
        t.insert(0, "Z")
        st("insert with selection")
        t.undo()
        st("undo")
        t.clear()
        st("clear")
        t.text = "abc def ghi"
        st("set text")
        t.selectWord()
        st("selectWord at end")
        t.cursorPosition = 5
        t.selectWord()
        st("selectWord inside")
        t.cursorPosition = 0
        st("cursorPosition clears selection")
        t.text = "abc"
        t.insert(3, "d")
        t.undo()
        st("undo insert")
        t.redo()
        st("redo insert")
        t.maximumLength = 5
        st("maximumLength cuts")
        t.maximumLength = 32767
        t.readOnly = true
        t.insert(0, "Q")
        st("insert while readOnly")
        t.remove(0, 1)
        st("remove while readOnly")
        t.selectAll()
        t.cut()
        st("cut while readOnly")
        t.paste()
        st("paste while readOnly")
        t.readOnly = false
        t.text = "one two three"
        t.select(4, 7)
        t.copy()
        t.cursorPosition = 0
        t.paste()
        st("paste at start")
        t.select(0, 4)
        t.paste()
        st("paste over selection")
        t.select(0, 3)
        t.cut()
        st("cut selection")
        t.paste()
        st("paste again")
        console.log("positionAt", t.positionAt(0), t.positionAt(10000))
        t.echoMode = TextInput.Password
        console.log("displayText", t.displayText.length, t.passwordCharacter.charCodeAt(0))
        console.log("defaults", t.selectByMouse, t.activeFocusOnPress, t.persistentSelection, t.acceptableInput, t.horizontalAlignment, t.verticalAlignment)
        Qt.quit()
    }
}
