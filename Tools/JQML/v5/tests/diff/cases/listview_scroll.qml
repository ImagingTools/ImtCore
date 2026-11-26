import QtQuick
import QtTest

// The scroll position of a ListView: currentIndex, positionViewAtIndex, the keys, the drag and the flick (also over a
// MouseArea that fills each row), GridView's drag
Item {
    id: root
    width: 600
    height: 400
    property string log: ""

    TestCase { id: tc; name: "measure"; when: windowShown
        function test_run() {
            console.log("defaults", list.currentIndex, list.keyNavigationEnabled, list.highlightRangeMode, list.boundsBehavior, grid.currentIndex)
            list.currentIndex = 3; tc.wait(50); out("currentIndex=3 (row 150..200 visible 0..200)", list.contentY)
            list.currentIndex = 4; tc.wait(50); out("currentIndex=4", list.contentY)
            list.currentIndex = 8; tc.wait(50); out("currentIndex=8", list.contentY)
            list.currentIndex = 7; tc.wait(50); out("currentIndex=7", list.contentY)
            list.currentIndex = 0; tc.wait(50); out("currentIndex=0", list.contentY)
            list.currentIndex = 19; tc.wait(50); out("currentIndex=19", list.contentY)
            list.contentY = 0
            list.positionViewAtIndex(15, ListView.Contain); out("pos 15 Contain", list.contentY)
            list.positionViewAtIndex(14, ListView.Contain); out("pos 14 Contain (visible now)", list.contentY)
            list.positionViewAtIndex(2, ListView.Contain); out("pos 2 Contain", list.contentY)
            list.positionViewAtIndex(3, ListView.Beginning); out("pos 3 Beginning", list.contentY)
            list.positionViewAtIndex(10, ListView.Center); out("pos 10 Center", list.contentY)
            list.positionViewAtIndex(10, ListView.End); out("pos 10 End", list.contentY)
            list.positionViewAtIndex(1, ListView.Visible); out("pos 1 Visible", list.contentY)
            list.positionViewAtIndex(19, ListView.Visible); out("pos 19 Visible", list.contentY)
            list.positionViewAtIndex(19, ListView.Beginning); out("pos 19 Beginning", list.contentY)
            list.positionViewAtEnd(); out("pos End", list.contentY)
            list.positionViewAtBeginning(); out("pos Beginning", list.contentY)
            // keys
            list.contentY = 0; list.currentIndex = 0; list.forceActiveFocus(); tc.wait(50)
            for (var k = 0; k < 6; k++) { tc.keyClick(Qt.Key_Down); tc.wait(30); out("Down " + (k + 1) + " current " + list.currentIndex, list.contentY) }
            tc.keyClick(Qt.Key_PageDown); tc.wait(30); out("PageDown current " + list.currentIndex, list.contentY)
            tc.keyClick(Qt.Key_Up); tc.wait(30); out("Up current " + list.currentIndex, list.contentY)
            // drag on a plain list: 60 px up in steps, slowly
            list.contentY = 0
            tc.mousePress(list, 100, 150)
            for (var i = 1; i <= 6; i++) { tc.mouseMove(list, 100, 150 - i * 20, 100) }
            out("list drag, moved 120 px, before release", list.contentY)
            tc.mouseRelease(list, 100, 30, Qt.LeftButton, Qt.NoModifier, 0)
            tc.wait(1500); out("list drag released and settled", list.contentY)
            // drag over rows with a MouseArea
            tc.mousePress(rows, 100, 125)
            for (var j = 1; j <= 6; j++) { tc.mouseMove(rows, 100, 125 - j * 20, 100) }
            out("rows drag before release", rows.contentY)
            tc.mouseRelease(rows, 100, 5, Qt.LeftButton, Qt.NoModifier, 0)
            tc.wait(1500); out("rows drag released and settled", rows.contentY)
            // click without moving
            tc.mouseClick(rows, 100, 25); out("rows click")
            // grid drag
            tc.mousePress(grid, 75, 150)
            for (var m = 1; m <= 6; m++) { tc.mouseMove(grid, 75, 150 - m * 20, 100) }
            out("grid drag before release", grid.contentY)
            tc.mouseRelease(grid, 75, 30, Qt.LeftButton, Qt.NoModifier, 0)
            tc.wait(1500); out("grid drag released and settled", grid.contentY)
            // quick flick on the list
            list.contentY = 0
            tc.mousePress(list, 100, 190)
            for (var n = 1; n <= 6; n++) { tc.mouseMove(list, 100, 190 - n * 25, 10) }
            tc.mouseRelease(list, 100, 40, Qt.LeftButton, Qt.NoModifier, 10)
            tc.wait(2500); out("list flicked", list.contentY)
            
        }
    }

    ListView {
        id: list
        x: 0; y: 0; width: 200; height: 200
        model: 20
        spacing: 0
        delegate: Rectangle { width: 200; height: 50; color: index % 2 ? "lightgray" : "white" }
        onMovementStarted: root.log += "MS "
        onMovementEnded: root.log += "ME "
        onFlickStarted: root.log += "FS "
        onFlickEnded: root.log += "FE "
    }
    ListView {
        id: rows
        x: 220; y: 0; width: 200; height: 200
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
        onMovementStarted: root.log += "MS "
        onMovementEnded: root.log += "ME "
    }
    GridView {
        id: grid
        x: 440; y: 0; width: 150; height: 200
        cellWidth: 50; cellHeight: 50
        model: 60
        delegate: Rectangle { width: 48; height: 48; color: "gray" }
        onMovementStarted: root.log += "MS "
        onMovementEnded: root.log += "ME "
        onFlickStarted: root.log += "FS "
    }

    function out(name, v) { console.log(name, v === undefined ? "" : v, "[" + root.log.trim() + "]"); root.log = "" }

}
