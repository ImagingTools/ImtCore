import QtQuick
import QtTest

// A digit key reaches its own Keys signal (Keys.digit5Pressed); a digit without a handler of its own goes to Keys.pressed with its Qt key code
Window {
    id: root
    width: 200
    height: 120
    visible: true

    property int digit5Count: 0
    property int pressedCount: 0
    property int lastPressedKey: 0

    Item {
        id: target
        anchors.fill: parent
        focus: true

        Keys.onDigit5Pressed: function(event) {
            root.digit5Count += 1
        }
        Keys.onPressed: function(event) {
            root.pressedCount += 1
            root.lastPressedKey = event.key
        }
    }

    TestCase {
        id: tc
        name: "DigitKeys"
        when: false
    }

    Timer {
        interval: 100
        running: true
        onTriggered: {
            try {
                target.forceActiveFocus()
                tc.keyClick(Qt.Key_5)
                console.log("digit5", root.digit5Count, root.pressedCount)
                tc.keyClick(Qt.Key_7)
                console.log("digit7", root.digit5Count, root.pressedCount, root.lastPressedKey === Qt.Key_7)
            } catch(err) {
                console.log("digit-keys-error", err)
            }
            Qt.quit()
        }
    }
}
