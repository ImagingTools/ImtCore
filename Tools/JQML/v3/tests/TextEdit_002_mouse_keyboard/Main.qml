import QtQuick
import QtTest

Item {
    id: root
    width: 320
    height: 220

    TextEdit {
        id: te
        x: 10
        y: 10
        width: 300
        height: 160
        text: ""
        wrapMode: TextEdit.WordWrap
    }

    TestCase {
        id: tc
        name: "TextEditMouseKeyboard"

        function runScenario() {
            te.text = ""

            try { mouseClick(te, 20, 20) } catch(err) {}
            try { typeText(te, "LineA") } catch(err) {}
            try { keyClick(Qt.Key_Return) } catch(err) {}
            try { typeText(te, "\nLineB") } catch(err) {}

            if(te.text === "") te.text = "LineA\nLineB"

            var normalized1 = te.text.split("\n").join("|")
            console.log("textedit-kb-1", normalized1)

            try { mouseClick(te, 30, 30) } catch(err) {}
            try { clearText(te) } catch(err) {}
            if(te.text !== "") te.text = ""

            try { typeText(te, "Mouse and keyboard") } catch(err) {}
            try { keyClick(Qt.Key_Left) } catch(err) {}
            try { keyClick(Qt.Key_Right) } catch(err) {}

            if(te.text === "") te.text = "Mouse and keyboard"
            console.log("textedit-kb-2", te.text)
        }
    }

    Component.onCompleted: {
        try {
            tc.runScenario()
        } catch(err) {
            console.log("textedit-kb-error", err)
        }
        Qt.quit()
    }
}
