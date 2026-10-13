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
        maximumLength: 17

        property string prevText: ""
        property bool block: false

        function convert(value) {
            var macAddress = value
            var len = macAddress.length
            var maxLen = 17

            if (len >= 2 && len < maxLen) {
                while (!(macAddress.indexOf(":") < 0)) {
                    macAddress = macAddress.replace(":", "")
                }

                var resultMacAddress = ""
                for (var i = 1; i <= macAddress.length; i++) {
                    resultMacAddress += macAddress[i - 1]
                    if (i % 2 == 0 && i != 12) {
                        resultMacAddress += ":"
                    }
                }
                return resultMacAddress
            }

            return value
        }

        onTextChanged: {
            if (block) {
                return
            }

            if (prevText.length < text.length) {
                block = true
                ti.text = convert(ti.text)
                block = false
            }

            prevText = text
        }
    }

    TestCase {
        id: tc
        name: "TextInputFormatOnType"

        function typeMac(value) {
            try {
                typeText(ti, value)
            } catch (err) {
            }

            if (ti.text === "") {
                for (var i = 0; i < value.length; i++) {
                    ti.text = ti.text + value[i]
                }
            }
        }

        function runScenario() {
            ti.text = ""
            ti.prevText = ""
            typeMac("AABBCC")
            console.log("mac-typed", ti.text)

            ti.text = ""
            ti.prevText = ""
            typeMac("112233445566")
            console.log("mac-full", ti.text)
        }
    }

    Component.onCompleted: {
        try {
            tc.runScenario()
        } catch (err) {
            console.log("mac-error", err)
        }
        Qt.quit()
    }
}
