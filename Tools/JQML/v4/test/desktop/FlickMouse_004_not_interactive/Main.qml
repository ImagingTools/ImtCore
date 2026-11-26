import QtQuick
import QtTest

// interactive: false: the Flickable takes no drag, the area has the whole gesture.
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
        interactive: false
        MouseArea {
            width: 200; height: 1000
            onPressed: root.log += "P "
            onReleased: root.log += "R "
            onClicked: root.log += "C "
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
            tc.mousePress(flick, 100, 150)
            for (var i = 1; i <= 5; i++) tc.mouseMove(flick, 100, 150 - i * 20, 100)
            tc.mouseRelease(flick, 100, 50, Qt.LeftButton, Qt.NoModifier, 50)
            out("drag")
            Qt.quit()
        }
    }
}
