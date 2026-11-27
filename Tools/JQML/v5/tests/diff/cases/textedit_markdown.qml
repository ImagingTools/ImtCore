import QtQuick
import QtTest

// TextEdit.MarkdownText (QTextMarkdownImporter / QTextMarkdownWriter): text is the document's Markdown; headings,
// emphasis, lists, task lists, quotes, code, rules, links, tables; getText, getFormattedText, insert and append of
// Markdown; the conversions of setTextFormat between Markdown, RichText and PlainText; undo
Item {
    id: root
    width: 400
    height: 300
    TextEdit {
        id: edit
        width: 300
        textFormat: TextEdit.MarkdownText
        font.family: "Arial"
        font.pixelSize: 12
        text: "# Title\n\nSome *em* and **strong** text"
    }
    TextEdit {
        id: points
        width: 300
        textFormat: TextEdit.MarkdownText
        font.family: "Arial"
        font.pointSize: 12
        text: "para\n\n- a\n- b"
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
    function dump(n) {
        console.log(n, esc(edit.text))
        console.log(n, "raw", esc(edit.getText(0, edit.length)), edit.length, edit.lineCount, edit.cursorPosition, edit.canUndo)
    }
    TestCase {
        name: "TextEditMarkdown"
        when: windowShown
        function test_1_initial() {
            root.dump("initial")
            console.log("points", root.esc(points.text))
        }
        function test_2_blocks() {
            let cases = [
                "## Sub\n\npara one\nstill one\n\npara two",
                "- one\n- two\n  - nested\n- three",
                "1. first\n2. second\n\n3) third",
                "- [ ] todo\n- [x] done",
                "> quote\n> more\n\n> > nested",
                "```js\nlet x = 1\n\nlet y\n```\n\nafter",
                "    indented\n    code",
                "a\n\n***\n\nb",
                "[link](http://x.y \"T\") and <http://auto.link> and www.example.com",
                "| a | b |\n|---|:-:|\n| 1 | 2 |",
                "_under_ ~~del~~ `code` \\*esc\\* &amp; &copy;",
                "<b>html</b> inline",
                "line  \nbreak",
                "This is a very long paragraph of text that should certainly be wrapped by the markdown writer because it is longer than eighty columns.",
            ]
            for (let i = 0; i < cases.length; i++) {
                edit.text = cases[i]
                root.dump("case " + i)
            }
        }
        function test_3_formatted() {
            edit.text = "# Head\n\n- **bold** item\n- *em* item\n\ntail"
            console.log("formatted all", root.esc(edit.getFormattedText(0, edit.length)))
            console.log("formatted part", root.esc(edit.getFormattedText(7, 16)))
            console.log("selected", edit.select(7, 16), root.esc(edit.selectedText))
        }
        function test_4_edits() {
            edit.text = "start"
            edit.insert(5, " **bold**")
            root.dump("insert")
            edit.append("- item")
            root.dump("append")
            edit.undo()
            root.dump("undo append")
            edit.undo()
            root.dump("undo insert")
            edit.cursorPosition = 5
            edit.cursorSelection.font.bold = true
            root.dump("word bold")
            edit.select(0, 5)
            edit.cursorSelection.font.italic = true
            root.dump("selection italic")
        }
        function test_5_conversions() {
            edit.text = "# T\n\n**b** and [l](http://x.y)"
            edit.textFormat = TextEdit.RichText
            console.log("md to rich", root.body(edit.text))
            edit.textFormat = TextEdit.MarkdownText
            root.dump("rich to md")
            edit.textFormat = TextEdit.PlainText
            root.dump("md to plain")
            edit.textFormat = TextEdit.MarkdownText
            root.dump("plain to md")
            edit.textFormat = TextEdit.AutoText
            root.dump("md to auto")
            edit.textFormat = TextEdit.PlainText
            edit.text = "<b>x</b> y"
            edit.textFormat = TextEdit.MarkdownText
            root.dump("plain html to md")
            edit.textFormat = TextEdit.RichText
            edit.text = "<p><b>bold</b> <i>it</i></p><ul><li>one</li></ul>"
            edit.textFormat = TextEdit.MarkdownText
            root.dump("rich to md 2")
        }
    }
}
