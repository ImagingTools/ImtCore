import QtQuick
import QtTest

// An area with drag.target inside a Flickable: who gets the drag, the target or the content?
Item {
    id: root
    width: 400
    height: 400
    property string log: ""
    TestCase { id: tc; name: "mouse"; when: windowShown }

    Flickable {
        id: flick
        x: 0; y: 0; width: 300; height: 300
        contentWidth: 300
        contentHeight: 1000
        Rectangle {
            id: box
            x: 100; y: 100; width: 60; height: 60
            color: "red"
            MouseArea {
                anchors.fill: parent
                drag.target: box
                onPressed: root.log += "P "
                onReleased: root.log += "R(" + drag.active + ") "
                onClicked: root.log += "C "
                onCanceled: root.log += "X "
            }
        }
    }

    function out(name) {
        console.log(name, "[" + root.log.trim() + "]", "contentY", Math.round(flick.contentY), "box", Math.round(box.x), Math.round(box.y), "dragging", flick.dragging)
        root.log = ""
    }

    Timer {
        interval: 100
        running: true
        onTriggered: {
            tc.mousePress(box, 30, 30)
            for (var i = 1; i <= 5; i++) tc.mouseMove(box, 30 + i * 12, 30 + i * 12, 100)
            out("drag of the box")
            tc.mouseRelease(box, 90, 90, Qt.LeftButton, Qt.NoModifier, 250)
            out("released")
            Qt.quit()
        }
    }
}
