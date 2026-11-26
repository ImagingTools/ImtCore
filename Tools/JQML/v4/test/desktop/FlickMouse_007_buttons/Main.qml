import QtQuick
import QtTest

// Only the left button drags a Flickable: a drag with the right button stays with the area.
Item {
    id: root
    width: 400
    height: 400
    property string log: ""
    TestCase { id: tc; name: "mouse"; when: windowShown }

    Flickable {
        id: flick
        x: 0; y: 0; width: 200; height: 200
        contentWidth: 200
        contentHeight: 1000
        MouseArea {
            width: 200; height: 1000
            acceptedButtons: Qt.AllButtons
            onPressed: root.log += "P" + (mouse.button === Qt.RightButton ? "r" : "l") + " "
            onReleased: root.log += "R" + (mouse.button === Qt.RightButton ? "r" : "l") + " "
            onClicked: root.log += "C" + (mouse.button === Qt.RightButton ? "r" : "l") + " "
            onCanceled: root.log += "X "
        }
    }

    function out(name) {
        console.log(name, "[" + root.log.trim() + "]", "contentY", Math.round(flick.contentY), "dragging", flick.dragging)
        root.log = ""
    }

    Timer {
        interval: 100
        running: true
        onTriggered: {
            tc.mousePress(flick, 100, 150, Qt.RightButton)
            for (var i = 1; i <= 5; i++) tc.mouseMove(flick, 100, 150 - i * 20, 100)
            tc.mouseRelease(flick, 100, 50, Qt.RightButton, Qt.NoModifier, 50)
            out("right drag")
            tc.mousePress(flick, 100, 150, Qt.LeftButton)
            for (var j = 1; j <= 5; j++) tc.mouseMove(flick, 100, 150 - j * 20, 100)
            tc.mouseRelease(flick, 100, 50, Qt.LeftButton, Qt.NoModifier, 50)
            out("left drag")
            Qt.quit()
        }
    }
}
