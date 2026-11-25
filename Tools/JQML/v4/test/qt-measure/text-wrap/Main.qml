// The lines of a plain text in desktop Qt for each wrap mode, in a monospace font of 10 and 25 columns: where each line starts
// (TextEdit.positionToRectangle) and how many lines Text has. The expected lines of test/text-wrap.mjs come from here.
// QT_QPA_PLATFORM=offscreen QT_FORCE_STDERR_LOGGING=1 qml.exe Main.qml
import QtQuick

Item {
    id: root
    width: 800
    height: 600

    property var texts: [
        "Connecting to http://files.example-server.cloud:7380/Files/Views/html/index.html ...",
        "one two abcdefghijklmnopqrstuvwxyz three",
        "one two three four five",
        "abc defghijklmnop",
        "x  abcdefghijklmnopqrstu  y",
        "one-two-three-four-five-six"
    ]
    property var modes: [["WordWrap", Text.WordWrap], ["WrapAnywhere", Text.WrapAnywhere], ["Wrap", Text.Wrap]]

    TextMetrics {
        id: metrics
        font.family: "Courier New"
        font.pixelSize: 10
        text: "abcdefghij"
    }

    TextEdit {
        id: edit
        font: metrics.font
        readOnly: true
        textFormat: TextEdit.PlainText
    }

    Text {
        id: label
        font: metrics.font
    }

    function lines(text) {
        let starts = []
        let lastY = -1
        for (let i = 0; i <= text.length; i++) {
            let y = edit.positionToRectangle(i).y
            if (y !== lastY) {
                starts.push(i)
                lastY = y
            }
        }
        let parts = []
        for (let s = 0; s < starts.length; s++)
            parts.push(JSON.stringify(text.substring(starts[s], s + 1 < starts.length ? starts[s + 1] : text.length)))
        return parts
    }

    Component.onCompleted: {
        let column = metrics.advanceWidth / 10
        for (let t = 0; t < texts.length; t++) {
            for (let columns of [10, 25]) {
                for (let m = 0; m < modes.length; m++) {
                    edit.width = columns * column + 0.5
                    edit.wrapMode = modes[m][1]
                    edit.text = texts[t]
                    label.width = edit.width
                    label.wrapMode = modes[m][1]
                    label.text = texts[t]
                    console.log(columns, modes[m][0], lines(texts[t]).join(" | "), "lineCount", label.lineCount)
                }
            }
        }
        Qt.quit()
    }
}
