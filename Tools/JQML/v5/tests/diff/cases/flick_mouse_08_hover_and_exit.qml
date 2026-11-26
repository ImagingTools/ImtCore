import QtQuick
import QtTest

// hoverEnabled area in a Flickable: what entered, exited and canceled say when the Flickable takes the drag.
Item {
    id: root
    width: 400
    height: 400
    property string log: ""
    TestCase { id: tc; name: "mouse"; when: windowShown
        function test_run() {
            tc.mouseMove(flick, 100, 150, 50)
            out("entered")
            tc.mousePress(flick, 100, 150)
            for (var i = 1; i <= 5; i++) tc.mouseMove(flick, 100, 150 - i * 20, 100)
            out("dragged")
            tc.mouseRelease(flick, 100, 50, Qt.LeftButton, Qt.NoModifier, 250)
            out("released")
            
        }
    }

    Flickable {
        id: flick
        x: 0; y: 0; width: 200; height: 200
        contentWidth: 200
        contentHeight: 1000
        MouseArea {
            width: 200; height: 1000
            hoverEnabled: true
            onEntered: root.log += "E "
            onExited: root.log += "L "
            onPressed: root.log += "P "
            onReleased: root.log += "R "
            onClicked: root.log += "C "
            onCanceled: root.log += "X(" + containsMouse + "," + pressed + ") "
        }
    }

    function out(name) {
        console.log(name, "[" + root.log.trim() + "]", "contentY", Math.round(flick.contentY))
        root.log = ""
    }

}
