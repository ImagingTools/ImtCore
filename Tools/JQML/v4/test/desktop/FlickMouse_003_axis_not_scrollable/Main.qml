import QtQuick
import QtTest

// A Flickable that can scroll vertically only does not take a horizontal drag: the area keeps it. A vertical drag is taken.
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
        console.log(name, "[" + root.log.trim() + "]", "contentX", Math.round(flick.contentX), "contentY", Math.round(flick.contentY), "dragging", flick.dragging)
        root.log = ""
    }

    Timer {
        interval: 100
        running: true
        onTriggered: {
            tc.mousePress(flick, 150, 100)
            for (var i = 1; i <= 5; i++) tc.mouseMove(flick, 150 - i * 20, 100, 100)
            out("horizontal drag")
            tc.mouseRelease(flick, 50, 100, Qt.LeftButton, Qt.NoModifier, 50)
            out("released")
            tc.mousePress(flick, 100, 150)
            for (var j = 1; j <= 5; j++) tc.mouseMove(flick, 100 + (j % 2) * 3, 150 - j * 20, 100)
            out("vertical drag")
            tc.mouseRelease(flick, 103, 50, Qt.LeftButton, Qt.NoModifier, 250)
            out("released")
            Qt.quit()
        }
    }
}
