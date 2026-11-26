import QtQuick
import QtTest

// ListView follows its current item (the highlight, a SmoothedAnimation of highlightMoveVelocity 400) after the keys and
// after currentIndex, also once positionViewAtBeginning put the highlight at the current item at once
Item {
    width: 600; height: 400
    TestCase { id: tc; name: "k"; when: windowShown
        function test_a() {
            list.forceActiveFocus(); tc.wait(50)
            for (var k = 0; k < 5; k++) { tc.keyClick(Qt.Key_Down); tc.wait(30); console.log("A Down", list.currentIndex, list.contentY) }
        }
        function test_b() {
            list.currentIndex = 19; tc.wait(50)
            list.positionViewAtBeginning()
            list.contentY = 0; list.currentIndex = 0; tc.wait(50)
            for (var k = 0; k < 5; k++) { tc.keyClick(Qt.Key_Down); tc.wait(30); console.log("B Down", list.currentIndex, list.contentY) }
        }
        function test_c() {
            list.currentIndex = 19; tc.wait(50)
            list.contentY = 0; list.currentIndex = 0; tc.wait(50)
            for (var k = 0; k < 5; k++) { list.currentIndex++; tc.wait(30); console.log("C set", list.currentIndex, list.contentY) }
        }
    }
    ListView {
        id: list
        width: 200; height: 200
        model: 20
        delegate: Rectangle { width: 200; height: 50 }
    }
}
