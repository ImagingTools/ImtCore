import QtQuick
import QtTest

// TextEdit undo and redo as QTextDocument keeps them: typed characters one step while they go on, Backspace and Delete
// one step while they go on, a replaced selection, Return and Shift+Return, the methods (insert, remove, append, clear),
// the cursor after undo and redo where the step changed the text; text of a plain TextEdit as QTextDocument::toPlainText
Item {
    id: root
    width: 400
    height: 300
    TextEdit { id: edit; width: 300; font.family: "Arial"; font.pixelSize: 12; text: "abc def\nsecond" }
    function esc(s) {
        let out = ""
        for (let i = 0; i < s.length; i++) {
            let c = s.charCodeAt(i)
            out += c < 32 || c > 126 ? "\\u" + c.toString(16) : s[i]
        }
        return out
    }
    function dump(n) {
        console.log(n, esc(edit.text), esc(edit.getText(0, edit.length)), edit.length, edit.cursorPosition, edit.selectionStart, edit.selectionEnd, edit.lineCount, edit.canUndo, edit.canRedo)
    }
    TestCase {
        name: "TextEditUndo"
        when: windowShown
        function test_1_typing() {
            edit.forceActiveFocus()
            edit.cursorPosition = 3
            keyClick(Qt.Key_X); keyClick(Qt.Key_Y)
            root.dump("typed")
            edit.cursorPosition = 9
            keyClick(Qt.Key_Backspace)
            root.dump("backspace")
            edit.undo(); root.dump("undo 1")
            edit.undo(); root.dump("undo 2")
            edit.redo(); root.dump("redo")
            keyClick(Qt.Key_Return); root.dump("return")
            edit.select(1, 4); keyClick(Qt.Key_Q); root.dump("replace")
            edit.undo(); root.dump("undo replace")
            edit.redo(); root.dump("redo replace")
            console.log("formatted", root.esc(edit.getFormattedText(0, 5)))
        }
        function test_2_deleting() {
            edit.text = "one two three"
            root.dump("set")
            edit.cursorPosition = 7
            keyClick(Qt.Key_Backspace); keyClick(Qt.Key_Backspace); keyClick(Qt.Key_Backspace)
            root.dump("backspaces")
            keyClick(Qt.Key_Delete); keyClick(Qt.Key_Delete)
            root.dump("deletes")
            edit.undo(); root.dump("undo deletes")
            edit.undo(); root.dump("undo backspaces")
            keyClick(Qt.Key_Z)
            root.dump("typed after undo")
            console.log("redo after typing", edit.canRedo)
        }
        function test_3_lines() {
            edit.text = "ab"
            edit.cursorPosition = 1
            keyClick(Qt.Key_Return)
            keyClick(Qt.Key_Return, Qt.ShiftModifier)
            keyClick(Qt.Key_C)
            root.dump("lines")
            edit.undo(); root.dump("undo c")
            edit.undo(); root.dump("undo shift return")
            edit.undo(); root.dump("undo return")
            edit.text = "a b\tc d e"
            root.dump("special characters")
        }
        function test_4_methods() {
            edit.text = "start"
            edit.cursorPosition = 5
            edit.insert(5, " more")
            root.dump("insert")
            edit.remove(0, 3)
            root.dump("remove")
            edit.append("line")
            root.dump("append")
            edit.undo(); root.dump("undo append")
            edit.undo(); root.dump("undo remove")
            edit.undo(); root.dump("undo insert")
            edit.redo(); edit.redo(); root.dump("redo two")
            edit.clear()
            root.dump("clear")
            edit.undo(); root.dump("undo clear")
            edit.cursorPosition = 2
            keyClick(Qt.Key_A); keyClick(Qt.Key_B)
            edit.cursorPosition = 0
            keyClick(Qt.Key_C)
            edit.undo(); root.dump("undo after moving")
            edit.undo(); root.dump("undo typing")
        }
        function test_5_rich() {
            edit.textFormat = TextEdit.RichText
            edit.text = "<b>bold</b> text"
            edit.cursorPosition = 4
            keyClick(Qt.Key_X); keyClick(Qt.Key_Y)
            root.dump("rich typed")
            edit.cursorPosition = 7
            keyClick(Qt.Key_Z)
            root.dump("rich typed elsewhere")
            edit.undo(); root.dump("rich undo 1")
            edit.undo(); root.dump("rich undo 2")
            edit.redo(); root.dump("rich redo")
            edit.select(2, 9)
            keyClick(Qt.Key_Q)
            edit.undo(); root.dump("rich undo replace")
            edit.textFormat = TextEdit.PlainText
        }
    }
}
