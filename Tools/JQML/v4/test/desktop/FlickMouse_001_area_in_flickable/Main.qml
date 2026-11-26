import QtQuick
import QtTest

// A MouseArea that fills the content of a Flickable: a click that does not move is the click of the area; a drag farther than the
// threshold is taken by the Flickable (the area gets `canceled`, no `released`, no `clicked`) and the content follows the pointer.
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
            onPositionChanged: root.log += "m "
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
            tc.mousePress(flick, 100, 150)
            tc.mouseRelease(flick, 100, 150)
            out("click")
            // a move of 5 px stays a click of the area
            tc.mousePress(flick, 100, 150)
            tc.mouseMove(flick, 100, 145, 50)
            tc.mouseRelease(flick, 100, 145, Qt.LeftButton, Qt.NoModifier, 50)
            out("5 px")
            // a drag of 100 px in steps of 20, slowly
            tc.mousePress(flick, 100, 150)
            for (var i = 1; i <= 5; i++) tc.mouseMove(flick, 100, 150 - i * 20, 100)
            out("dragging 100 px")
            tc.mouseRelease(flick, 100, 50, Qt.LeftButton, Qt.NoModifier, 250)
            out("released")
            tc.wait(300)
            out("settled")
            Qt.quit()
        }
    }
}
