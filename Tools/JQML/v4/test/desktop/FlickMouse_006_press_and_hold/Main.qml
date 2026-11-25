import QtQuick
import QtTest

// pressAndHold of an area in a Flickable: a hold that does not move is the hold of the area; a drag that starts before the hold takes the press.
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
            onPressed: root.log += "P "
            onReleased: root.log += "R "
            onClicked: root.log += "C "
            onCanceled: root.log += "X "
            onPressAndHold: root.log += "H "
        }
    }

    function out(name) {
        console.log(name, "[" + root.log.trim() + "]", "contentY", Math.round(flick.contentY))
        root.log = ""
    }

    Timer {
        interval: 100
        running: true
        onTriggered: {
            tc.mousePress(flick, 100, 150)
            tc.wait(900)
            out("held")
            tc.mouseRelease(flick, 100, 150)
            out("released after the hold")
            tc.mousePress(flick, 100, 150)
            for (var i = 1; i <= 5; i++) tc.mouseMove(flick, 100, 150 - i * 20, 50)
            tc.wait(900)
            out("dragged, then held")
            tc.mouseRelease(flick, 100, 50)
            out("released")
            Qt.quit()
        }
    }
}
