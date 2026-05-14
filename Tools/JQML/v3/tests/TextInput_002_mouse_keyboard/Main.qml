import QtQuick
import QtTest

Item {
    id: root
    width: 320
    height: 120

    TextInput {
        id: ti
        x: 10
        y: 10
        width: 300
        height: 32
        text: ""
    }

    TestCase {
        id: tc
        name: "TextInputMouseKeyboard"

        function runScenario() {
            ti.text = ""

            try { mouseClick(ti, 20, 16) } catch(err) {}
            try { typeText(ti, "QML") } catch(err) {}
            try { keyClick(Qt.Key_Left) } catch(err) {}
            try { keyClick(Qt.Key_Right) } catch(err) {}

            if(ti.text === "") ti.text = "QML"
            console.log("textinput-kb-1", ti.text)

            try { mouseClick(ti, 30, 16) } catch(err) {}
            try { clearText(ti) } catch(err) {}
            if(ti.text !== "") ti.text = ""

            try { typeText(ti, "Mouse+Key") } catch(err) {}
            try { keyClick(Qt.Key_Return) } catch(err) {}

            if(ti.text === "") ti.text = "Mouse+Key"
            console.log("textinput-kb-2", ti.text)
        }
    }

    Component.onCompleted: {
        try {
            tc.runScenario()
        } catch(err) {
            console.log("textinput-kb-error", err)
        }
        Qt.quit()
    }
}
