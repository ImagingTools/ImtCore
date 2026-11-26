import QtQuick
import QtTest

// A ListView whose rows have a MouseArea each, and a GridView whose cells have one: a click is the click of the row, a drag is the drag of
// the view (the area of the row is canceled).
Item {
    id: root
    width: 600
    height: 400
    property string log: ""
    TestCase { id: tc; name: "mouse"; when: windowShown
        function test_run() {
            tc.mouseClick(list, 100, 125)
            out("list click")
            tc.mousePress(list, 100, 125)
            for (var i = 1; i <= 6; i++) tc.mouseMove(list, 100, 125 - i * 20, 100)
            tc.mouseRelease(list, 100, 5, Qt.LeftButton, Qt.NoModifier, 250)
            tc.wait(200)
            out("list drag")
            tc.mouseClick(list, 100, 125)
            out("list click after scrolling")
            tc.mouseClick(grid, 75, 125)
            out("grid click")
            tc.mousePress(grid, 75, 125)
            for (var j = 1; j <= 6; j++) tc.mouseMove(grid, 75, 125 - j * 20, 100)
            tc.mouseRelease(grid, 75, 5, Qt.LeftButton, Qt.NoModifier, 250)
            tc.wait(200)
            out("grid drag")
            
        }
    }

    ListView {
        id: list
        x: 0; y: 0; width: 200; height: 200
        model: 20
        delegate: Rectangle {
            width: 200; height: 50; color: "white"
            MouseArea {
                anchors.fill: parent
                onPressed: root.log += "P" + index + " "
                onReleased: root.log += "R" + index + " "
                onClicked: root.log += "C" + index + " "
                onCanceled: root.log += "X" + index + " "
            }
        }
    }
    GridView {
        id: grid
        x: 300; y: 0; width: 150; height: 200
        cellWidth: 50; cellHeight: 50
        model: 60
        delegate: Rectangle {
            width: 48; height: 48; color: "gray"
            MouseArea {
                anchors.fill: parent
                onPressed: root.log += "P" + index + " "
                onReleased: root.log += "R" + index + " "
                onClicked: root.log += "C" + index + " "
                onCanceled: root.log += "X" + index + " "
            }
        }
    }

    function out(name) {
        console.log(name, "[" + root.log.trim() + "]", "list", Math.round(list.contentY), "grid", Math.round(grid.contentY))
        root.log = ""
    }

}
