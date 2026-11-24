import QtQuick
import QtTest

// TextEdit with RichText: the HTML it gives back (text, getFormattedText), its plain text and positions (getText,
// length), the keys that edit it (typing in the format of the cursor, Return, Shift+Return, Backspace and Delete over
// block and list boundaries), undo and redo, the methods (insert of HTML, remove, append, clear) and textFormat
Item {
    id: root
    width: 400
    height: 300
    TextEdit {
        id: edit
        width: 300
        textFormat: TextEdit.RichText
        font.family: "Arial"
        font.pixelSize: 12
        text: "<b>Hi</b> there"
    }
    function body(s) { return s.slice(s.indexOf("<body")).split("\n").join("|") }
    function raw(text) {
        let s = text === undefined ? edit.getText(0, edit.length) : text
        let out = ""
        for (let i = 0; i < s.length; i++) {
            let c = s.charCodeAt(i)
            out += c < 32 || c > 126 ? "\\u" + c.toString(16) : s[i]
        }
        return out
    }
    function dump(name) {
        console.log(name, raw(), edit.length, edit.cursorPosition, edit.selectionStart, edit.selectionEnd, edit.lineCount, edit.canUndo, edit.canRedo)
        console.log(name, "html", body(edit.text))
    }
    TestCase {
        name: "TextEditRichText"
        when: windowShown
        function test_1_initial() {
            root.dump("initial")
            console.log("formatted", body(edit.getFormattedText(1, 5)))
            console.log("selected", root.raw(edit.selectedText))
        }
        function test_2_typing() {
            edit.forceActiveFocus()
            edit.cursorPosition = 1
            keyClick(Qt.Key_X)
            root.dump("typed in bold")
            edit.cursorPosition = edit.length
            keyClick(Qt.Key_Y)
            root.dump("typed at end")
            keyClick(Qt.Key_Return)
            keyClick(Qt.Key_Z)
            root.dump("return")
            keyClick(Qt.Key_Return, Qt.ShiftModifier)
            keyClick(Qt.Key_W)
            root.dump("shift return")
            keyClick(Qt.Key_Backspace)
            keyClick(Qt.Key_Backspace)
            root.dump("backspace")
            edit.cursorPosition = 4
            keyClick(Qt.Key_Delete)
            root.dump("delete")
        }
        function test_3_undo() {
            edit.undo()
            root.dump("undo")
            edit.undo()
            root.dump("undo 2")
            edit.redo()
            root.dump("redo")
        }
        function test_4_selection() {
            edit.text = "<p align=\"center\">one <i>two</i></p><p align=\"right\">three</p>"
            root.dump("set")
            edit.select(5, 10)
            console.log("selected", root.raw(edit.selectedText))
            console.log("formatted", body(edit.getFormattedText(5, 10)))
            keyClick(Qt.Key_Q)
            root.dump("replaced")
            edit.cursorPosition = 0
            keyClick(Qt.Key_End)
            keyClick(Qt.Key_Delete)
            root.dump("joined")
        }
        function test_5_list() {
            edit.text = "<ul><li>a</li><li>b</li></ul><p style=\"-qt-block-indent:1\">c</p>"
            root.dump("list")
            edit.cursorPosition = 2
            keyClick(Qt.Key_Backspace)
            root.dump("out of list")
            edit.cursorPosition = 4
            keyClick(Qt.Key_Backspace)
            root.dump("indent")
            edit.cursorPosition = 1
            keyClick(Qt.Key_Return)
            root.dump("list return")
        }
        function test_6_methods() {
            edit.text = "<b>bold</b> plain"
            edit.insert(2, "<i>I</i>")
            root.dump("insert html")
            edit.insert(0, "plain ")
            root.dump("insert plain")
            edit.remove(3, 8)
            root.dump("remove")
            edit.append("<u>u</u>")
            root.dump("append")
            edit.clear()
            root.dump("clear")
            keyClick(Qt.Key_A)
            root.dump("typed after clear")
        }
        function test_7_format() {
            edit.text = "<b>b</b>"
            edit.textFormat = TextEdit.PlainText
            console.log("plain", edit.length, edit.text.length, edit.lineCount)
            edit.text = "<i>i</i>"
            console.log("plain set", edit.length, edit.text)
            edit.textFormat = TextEdit.AutoText
            root.dump("auto")
            edit.text = "no tags"
            root.dump("auto plain")
            edit.insert(0, "<b>B</b>")
            root.dump("auto insert html")
        }
        function test_8_table() {
            edit.textFormat = TextEdit.RichText
            edit.text = "x<table border=\"1\"><tr><td>a</td><td>b</td></tr><tr><td>c</td><td>d</td></tr></table>y"
            root.dump("table")
            edit.cursorPosition = 3
            keyClick(Qt.Key_Backspace)
            root.dump("backspace at cell start")
            keyClick(Qt.Key_Q)
            keyClick(Qt.Key_Delete)
            root.dump("delete at cell end")
            keyClick(Qt.Key_Return)
            keyClick(Qt.Key_R)
            root.dump("return in cell")
            edit.select(2, 8)
            console.log("cells selected", root.raw(edit.selectedText))
            keyClick(Qt.Key_S)
            root.dump("typed over cells")
            edit.remove(1, 4)
            root.dump("removed over the table's edge")
        }
        function test_9_typing_format() {
            edit.text = "<b>bold</b>"
            keyClick(Qt.Key_N)
            root.dump("typed at start after setText")
            edit.cursorPosition = 3
            keyClick(Qt.Key_M)
            root.dump("typed in bold")
            edit.text = "<p>a</p><p><i>b</i></p>"
            edit.cursorPosition = 2
            keyClick(Qt.Key_Backspace)
            root.dump("joined blocks")
            keyClick(Qt.Key_C)
            root.dump("typed after join")
        }
    }
}
