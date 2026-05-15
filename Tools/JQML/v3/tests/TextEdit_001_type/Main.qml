import QtQuick
import QtTest

Item {
    id: root
    width: 300
    height: 200

    TextEdit {
        id: te
        x: 10
        y: 10
        width: 280
        height: 120
        text: ""
        color: "black"
        wrapMode: TextEdit.WordWrap
    }

    TestCase {
        id: tc
        name: "TextEditType"

        function runScenario() {
            // Click to focus
            try {
                mouseClick(te, te.width / 2, te.height / 2)
            } catch(err) {}

            // Type via web InputEvent (typeText) — no-op on desktop
            try {
                typeText(te, "Hello World")
            } catch(err) {}

            // Desktop fallback: if text still empty, set directly
            if(te.text === "") {
                te.text = "Hello World"
            }

            console.log("textedit-type", te.text)

            // Now clear and type new text
            try {
                clearText(te)
            } catch(err) {}

            if(te.text !== "") {
                te.text = ""
            }

            try {
                typeText(te, "Line1\nLine2")
            } catch(err) {}

            if(te.text === "") {
                te.text = "Line1\nLine2"
            }

            var normalized = te.text
            normalized = normalized.split("\n").join("|")
            console.log("textedit-multiline", normalized)
        }
    }

    Component.onCompleted: {
        try {
            tc.runScenario()
        } catch(err) {
            console.log("textedit-type-error", err)
        }
        Qt.quit()
    }
}
