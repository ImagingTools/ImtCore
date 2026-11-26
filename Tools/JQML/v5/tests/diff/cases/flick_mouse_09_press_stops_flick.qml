import QtQuick
import QtTest

// A press while the content runs on stops it. The area under the pointer gets the press.
Item {
    id: root
    width: 400
    height: 400
    property string log: ""
    TestCase { id: tc; name: "mouse"; when: windowShown
        function test_run() {
            tc.mousePress(flick, 100, 190)
            for (var i = 1; i <= 6; i++) tc.mouseMove(flick, 100, 190 - i * 25, 10)
            tc.mouseRelease(flick, 100, 40, Qt.LeftButton, Qt.NoModifier, 10)
            out("flicked")
            tc.wait(200)
            out("running")
            tc.mousePress(flick, 100, 100)
            out("pressed")
            tc.mouseRelease(flick, 100, 100)
            out("released")
            
        }
    }

    Flickable {
        id: flick
        x: 0; y: 0; width: 200; height: 200
        contentWidth: 200
        contentHeight: 10000
        MouseArea {
            width: 200; height: 10000
            onPressed: root.log += "P "
            onReleased: root.log += "R "
            onClicked: root.log += "C "
            onCanceled: root.log += "X "
        }
    }

    function out(name) {
        console.log(name, "[" + root.log.trim() + "]", "flicking", flick.flicking, "moving", flick.moving, "ran far", flick.contentY > 200)
        root.log = ""
    }

}
