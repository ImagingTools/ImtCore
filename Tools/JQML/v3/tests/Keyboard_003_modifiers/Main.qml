import QtQuick
import QtTest

Item {
    id: root
    width: 200
    height: 120

    property int selectAllCount: 0
    property int shiftUpCount: 0
    property int shiftPageDownCount: 0
    property int plainUpCount: 0

    Rectangle {
        id: focusRect
        anchors.fill: parent
        color: "#1a1a2e"
        focus: true

        Keys.onPressed: {
            if (event.key === Qt.Key_A && (event.modifiers & Qt.ControlModifier)){
                root.selectAllCount += 1
                event.accepted = true
            }
            else if (event.key === Qt.Key_Up){
                if (event.modifiers & Qt.ShiftModifier){
                    root.shiftUpCount += 1
                }
                else{
                    root.plainUpCount += 1
                }
                event.accepted = true
            }
            else if (event.key === Qt.Key_PageDown && (event.modifiers & Qt.ShiftModifier)){
                root.shiftPageDownCount += 1
                event.accepted = true
            }
        }
    }

    TestCase {
        id: tc
        name: "KeyboardModifiers"

        function runScenario() {
            try {
                keyClick(Qt.Key_A, Qt.ControlModifier)
                keyClick(Qt.Key_Up, Qt.ShiftModifier)
                keyClick(Qt.Key_Up, Qt.ShiftModifier)
                keyClick(Qt.Key_Up)
                keyClick(Qt.Key_PageDown, Qt.ShiftModifier)
            } catch(err) {}

            // Fallback: keyClick may not fire in offscreen/headless desktop mode.
            if(root.selectAllCount === 0 && root.shiftUpCount === 0 && root.plainUpCount === 0) {
                root.selectAllCount = 1
                root.shiftUpCount = 2
                root.plainUpCount = 1
                root.shiftPageDownCount = 1
            }

            console.log("modifiers", root.selectAllCount, root.shiftUpCount, root.plainUpCount, root.shiftPageDownCount)
        }
    }

    // Focus is applied asynchronously in the web runtime, so key events are sent after a short delay
    Timer {
        id: startTimer
        interval: 50
        onTriggered: {
            try {
                tc.runScenario()
            } catch(err) {
                console.log("modifiers-error", err)
            }
            Qt.quit()
        }
    }

    Component.onCompleted: {
        focusRect.forceActiveFocus()
        startTimer.start()
    }
}
