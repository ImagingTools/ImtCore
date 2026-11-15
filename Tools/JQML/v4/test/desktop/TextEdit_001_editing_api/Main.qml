import QtQuick

Item {
    width: 300
    height: 100

    TextEdit {
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
        t.clear()
        st("clear")
        t.cursorPosition = 3
        t.text = "abc def ghi"
        st("set text moves the cursor to the start")
        t.selectWord()
        st("selectWord at start")
        t.cursorPosition = 5
        t.selectWord()
        st("selectWord inside")
        t.text = "abc"
        t.insert(3, "d")
        t.undo()
        st("undo insert")
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
        t.cursorPosition = 3
        t.cut()
        st("cut without selection")
        t.select(0, 3)
        t.cut()
        st("cut selection")
        t.append("line2")
        st("append")
        t.text = "a\nb"
        t.insert(1, "X\nY")
        st("insert newline")
        console.log("getText", JSON.stringify(t.getText(0, 3)), "positionAt", t.positionAt(0, 0), t.positionAt(10000, 10000))
        t.text = ""
        st("empty")
        console.log("defaults", t.selectByMouse, t.activeFocusOnPress, t.persistentSelection, t.readOnly, t.horizontalAlignment, t.verticalAlignment)
        Qt.quit()
    }
}
