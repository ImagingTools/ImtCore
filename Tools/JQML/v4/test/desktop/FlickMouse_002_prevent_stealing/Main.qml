import QtQuick
import QtTest

// preventStealing: the area keeps the drag of the pointer, the Flickable does not move.
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
            preventStealing: true
            onPressed: root.log += "P "
            onReleased: root.log += "R "
            onClicked: root.log += "C "
            onCanceled: root.log += "X "
            onPositionChanged: root.log += "m "
        }
    }

    function out(name) {
        console.log(name, "[" + root.log.trim() + "]", "contentY", Math.round(flick.contentY), "dragging", flick.dragging, "moving", flick.moving)
        root.log = ""
    }

    Timer {
        interval: 100
        running: true
        onTriggered: {
            tc.mousePress(flick, 100, 150)
            for (var i = 1; i <= 5; i++) tc.mouseMove(flick, 100, 150 - i * 20, 100)
            out("dragging 100 px")
            tc.mouseRelease(flick, 100, 50, Qt.LeftButton, Qt.NoModifier, 50)
            out("released")
            Qt.quit()
        }
    }
}
