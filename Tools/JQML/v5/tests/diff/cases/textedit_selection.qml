import QtQuick
import QtTest

// TextEdit.cursorSelection and TextSelection (QQuickTextSelection): the selected text, its font, color and the alignment
// of its blocks, read and written (a write without a selection takes the word under the cursor); moving the start and the
// end by QTextCursor's operations, duplicate and linkTo; a TextSelection of its own over the TextEdit's document;
// selectByKeyboard; textDocument.modified and status
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
        text: "<p>one <b>two</b> three</p><p>four five</p>"
        TextSelection { id: own }
    }
    function esc(s) {
        let out = ""
        for (let i = 0; i < s.length; i++) {
            let c = s.charCodeAt(i)
            out += c < 32 || c > 126 ? "\\u" + c.toString(16) : s[i]
        }
        return out
    }
    function body(s) { return s.slice(s.indexOf("<body")).split("\n").join("|") }
    function sel(n, s) {
        console.log(n, s.selectionStart, s.selectionEnd, esc(s.text), s.font.family, s.font.bold, s.font.italic, s.font.pixelSize, s.color, s.alignment)
    }
    function dump(n) {
        console.log(n, esc(edit.getText(0, edit.length)), edit.cursorPosition, edit.selectionStart, edit.selectionEnd, edit.canUndo)
        console.log(n, "html", body(edit.text))
    }
    TestCase {
        name: "TextEditSelection"
        when: windowShown
        function test_1_read() {
            edit.select(4, 7)
            root.sel("bold", edit.cursorSelection)
            edit.select(0, 3)
            root.sel("plain", edit.cursorSelection)
            edit.cursorPosition = 15
            root.sel("cursor", edit.cursorSelection)
            console.log("textDocument", edit.textDocument.modified, edit.textDocument.status)
        }
        function test_2_write() {
            edit.select(0, 3)
            edit.cursorSelection.color = "red"
            root.dump("color")
            edit.cursorSelection.font.italic = true
            root.dump("italic")
            edit.cursorSelection.alignment = Qt.AlignHCenter
            root.dump("alignment")
            edit.cursorPosition = 16
            edit.cursorSelection.color = "blue"
            root.dump("color of the word")
            edit.select(8, 13)
            edit.cursorSelection.text = "3"
            root.dump("text")
            root.sel("after text", edit.cursorSelection)
            console.log("modified", edit.textDocument.modified)
            edit.undo()
            root.dump("undo text")
        }
        function test_3_move() {
            edit.text = "<p>alpha beta gamma</p><p>delta</p>"
            let s = edit.cursorSelection
            console.log("move", s.moveSelectionStart(TextSelection.NextWord, 1), edit.cursorPosition)
            console.log("move end", s.moveSelectionEnd(TextSelection.EndOfWord), edit.selectionStart, edit.selectionEnd, root.esc(s.text))
            console.log("move end block", s.moveSelectionEnd(TextSelection.NextBlock), edit.selectionStart, edit.selectionEnd)
            console.log("move start fails", s.moveSelectionStart(TextSelection.PreviousBlock), edit.cursorPosition)
            s.moveSelectionStart(TextSelection.Start)
            s.moveSelectionEnd(TextSelection.EndOfWord)
            s.duplicate()
            root.dump("duplicate")
            root.sel("duplicated", s)
            s.linkTo("http://x.y/")
            root.dump("link")
        }
        function test_4_own() {
            edit.text = "<p>first second</p>"
            own.selectionStart = 6
            own.selectionEnd = 12
            root.sel("own", own)
            console.log("edit cursor stays", edit.cursorPosition, edit.selectionStart, edit.selectionEnd)
            own.font.bold = true
            root.dump("own bold")
            own.text = "2nd"
            root.dump("own text")
            root.sel("own after", own)
        }
        function test_5_keyboard() {
            edit.text = "<p>abc</p>"
            edit.forceActiveFocus()
            edit.cursorPosition = 0
            console.log("selectByKeyboard", edit.selectByKeyboard)
            keyClick(Qt.Key_Right, Qt.ShiftModifier)
            console.log("shift right", edit.selectionStart, edit.selectionEnd)
            edit.selectByKeyboard = false
            keyClick(Qt.Key_Right, Qt.ShiftModifier)
            console.log("shift right off", edit.selectionStart, edit.selectionEnd, edit.cursorPosition)
            edit.readOnly = true
            console.log("read-only", edit.selectByKeyboard)
            edit.selectByKeyboard = true
            edit.readOnly = false
        }
    }
}
