import QtQuick
import QtTest

// The wheel of a Flickable and a ListView (QQuickFlickable::wheelEvent): a step moves the content 72 pixels, animated,
// with movementStarted at once and movementEnded at the end; steps at the same time do not add up; at the end of the
// content the wheel is not taken; StopAtBounds stops at the bound; contentY set from outside ends the movement
Item {
    width: 300
    height: 300
    property var log: []
    Flickable {
        id: f
        width: 200
        height: 200
        contentWidth: 200
        contentHeight: 500
        onMovementStarted: log.push("started")
        onMovementEnded: log.push("ended " + Math.round(contentY))
    }
    ListView {
        id: list
        x: 200
        width: 100
        height: 200
        model: 100
        delegate: Rectangle { width: 100; height: 25 }
    }
    function show(label, view) {
        console.log(label, Math.round(view.contentY), view.moving, view.flicking, "|", log.join(", "))
        log = []
    }
    TestCase {
        when: windowShown
        function test_1_step() {
            mouseWheel(f, 100, 100, 0, -120)
            show("step", f)
            wait(800)
            show("after", f)
            mouseWheel(f, 100, 100, 0, -120)
            mouseWheel(f, 100, 100, 0, -120)
            wait(800)
            show("two at once", f)
            mouseWheel(f, 100, 100, 0, -240)
            wait(800)
            show("two steps in one", f)
        }
        function test_2_bounds() {
            f.contentY = 290
            mouseWheel(f, 100, 100, 0, -120)
            wait(800)
            show("past the end", f)
            mouseWheel(f, 100, 100, 0, -120)
            wait(100)
            show("at the end", f)
            f.boundsBehavior = Flickable.StopAtBounds
            f.contentY = 280
            mouseWheel(f, 100, 100, 0, -120)
            wait(800)
            show("stop at bounds", f)
            mouseWheel(f, 100, 100, 0, 120)
            wait(800)
            show("up", f)
        }
        function test_3_setDuring() {
            f.contentY = 0
            mouseWheel(f, 100, 100, 0, -120)
            f.contentY = 10
            show("set during", f)
            wait(800)
            show("after set", f)
        }
        function test_4_listView() {
            mouseWheel(list, 50, 100, 0, -120)
            wait(800)
            show("list", list)
            mouseWheel(list, 50, 100, 0, 120)
            mouseWheel(list, 50, 100, 0, 120)
            wait(800)
            show("list back", list)
        }
    }
}
