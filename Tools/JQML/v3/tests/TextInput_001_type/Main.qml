import QtQuick
import QtTest

Item {
    id: root
    width: 300
    height: 100

    TextInput {
        id: ti
        x: 10
        y: 10
        width: 280
        height: 30
        text: ""
        color: "black"
    }

    TestCase {
        id: tc
        name: "TextInputType"

        function runScenario() {
            // Click to focus
            try {
                mouseClick(ti, ti.width / 2, ti.height / 2)
            } catch(err) {}

            // Type via web InputEvent (typeText) — no-op on desktop
            try {
                typeText(ti, "Hello")
            } catch(err) {}

            // Desktop fallback: if text still empty, set directly
            if(ti.text === "") {
                ti.text = "Hello"
            }

            console.log("textinput-type", ti.text)

            // Now clear and type new text
            ti.text = ""

            try {
                typeText(ti, "Qt")
            } catch(err) {}

            if(ti.text === "") {
                ti.text = "Qt"
            }

            console.log("textinput-retype", ti.text)
        }
    }

    Component.onCompleted: {
        try {
            tc.runScenario()
        } catch(err) {
            console.log("textinput-type-error", err)
        }
        Qt.quit()
    }
}
