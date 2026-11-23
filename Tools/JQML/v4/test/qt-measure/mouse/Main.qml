import QtQuick
import QtTest

// What desktop Qt 6.11 does with the mouse, measured with QtTest (the mouse events are sent to a window by a TestCase). The tests
// test/mouse-area-canceled-hold.mjs, test/drag-drop.mjs and test/drag-reorder.mjs hold the values of the output below.
// Run: QT_QPA_PLATFORM=offscreen QT_FORCE_STDERR_LOGGING=1 qml -f Main.qml
Item {
    id: root
    width: 400
    height: 400
    property string log: ""

    TestCase { id: tc; name: "measure"; when: windowShown }

    MouseArea {
        id: plain
        x: 0; y: 0; width: 100; height: 100
        onPressed: root.log += "P "
        onReleased: root.log += "R "
        onClicked: root.log += "C "
        onCanceled: root.log += "X "
    }
    MouseArea {
        id: held
        x: 100; y: 0; width: 100; height: 100
        onPressed: root.log += "P "
        onReleased: root.log += "R "
        onClicked: root.log += "C "
        onPressAndHold: root.log += "H "
    }
    Item {
        id: dragger
        x: 0; y: 100; width: 200; height: 100
        DragHandler { target: null }
        MouseArea {
            anchors.fill: parent
            onPressed: root.log += "P "
            onReleased: root.log += "R "
            onClicked: root.log += "C "
            onCanceled: root.log += "X "
        }
    }
    Rectangle {
        id: box
        x: 250; y: 250; width: 30; height: 30
        Drag.active: grab.drag.active
        MouseArea {
            id: grab
            anchors.fill: parent
            drag.target: box
            onPressed: root.log += "P "
            onReleased: root.log += "R(drag.active=" + drag.active + ",Drag.active=" + box.Drag.active + ") "
            onClicked: root.log += "C "
        }
    }
    Rectangle {
        id: axisBox
        x: 300; y: 250; width: 30; height: 30
        MouseArea {
            id: axisArea
            anchors.fill: parent
            drag.target: axisBox
            drag.axis: Drag.YAxis
            drag.minimumY: 200
            drag.maximumY: 300
        }
    }

    function out(name) {
        console.log(name, "[" + root.log.trim() + "]")
        root.log = ""
    }
    function where(name) { console.log(name, "box", box.x, box.y, "active", grab.drag.active) }
    function press(item, x, y) { tc.mousePress(item, x, y) }
    function move(item, x, y) { tc.mouseMove(item, x, y, -1, Qt.LeftButton) }
    function release(item, x, y) { tc.mouseRelease(item, x, y) }

    // a pause of n steps of 60 ms
    function wait(n) {
        var list = []
        for (var i = 0; i < n; i++) list.push(function () { })
        return list
    }

    property int stepNumber: 0
    property var steps: [].concat(
        [function () { console.log("defaults", "interval", held.pressAndHoldInterval, "threshold", grab.drag.threshold, "axis", grab.drag.axis, "min", grab.drag.minimumX, "max", grab.drag.maximumX, "dragType", box.Drag.dragType, "supported", box.Drag.supportedActions, "proposed", box.Drag.proposedAction, "keys", box.Drag.keys.length, "hot", box.Drag.hotSpot.x, box.Drag.hotSpot.y) }],
        // click
        [function () { tc.mouseClick(plain, 50, 50) }, function () { out("click") }],
        // disabled while pressed
        [function () { press(plain, 50, 50) }, function () { plain.enabled = false }, function () { out("disabled while pressed"); console.log("pressed", plain.pressed) }, function () { release(plain, 50, 50) }, function () { out("release"); plain.enabled = true }],
        // hidden while pressed
        [function () { press(plain, 50, 50) }, function () { plain.visible = false }, function () { out("hidden while pressed"); console.log("pressed", plain.pressed) }, function () { release(plain, 50, 50) }, function () { out("release"); plain.visible = true }],
        // a DragHandler takes the drag
        [function () { press(dragger, 50, 50) }, function () { move(dragger, 50, 40) }, function () { move(dragger, 50, 20) }, function () { move(dragger, 50, 0) }, function () { out("drag handler steals") }, function () { release(dragger, 50, 0) }, function () { out("release") }],
        // hold
        [function () { press(held, 50, 50) }],
        wait(14),
        [function () { out("hold, no move, 0.9 s") }, function () { release(held, 50, 50) }, function () { out("release after hold") }],
        // hold with a move inside
        [function () { press(held, 20, 50) }, function () { move(held, 35, 50) }],
        wait(14),
        [function () { out("hold, moved 15 px inside, 0.9 s") }, function () { release(held, 35, 50) }, function () { out("release") }],
        // hold with a move outside
        [function () { press(held, 50, 50) }, function () { move(held, 50, 130) }],
        wait(14),
        [function () { out("hold, moved outside, 0.9 s") }, function () { release(held, 50, 130) }, function () { out("release outside") }],
        // hold, moved outside and back inside
        [function () { press(held, 50, 50) }, function () { move(held, 50, 130) }, function () { move(held, 50, 60) }],
        wait(14),
        [function () { out("hold, out and back, 0.9 s") }, function () { release(held, 50, 60) }, function () { out("release") }],
        // the drag of a target
        [function () { press(grab, 15, 15) }, function () { where("pressed") }, function () { move(grab, 20, 15) }, function () { where("moved 5 px") }, function () { move(grab, 24, 15) }, function () { where("moved 9 px") }, function () { move(grab, 26, 15) }, function () { where("moved 11 px") }, function () { move(grab, 30, 15) }, function () { where("moved 15 px") }, function () { move(grab, 55, 45) }, function () { where("moved far") }, function () { release(grab, 55, 45) }, function () { out("release after drag"); where("after release") }],
        // the threshold is crossed by one move and the press is released
        [function () { box.x = 250; box.y = 250 }, function () { press(grab, 15, 15) }, function () { move(grab, 30, 15) }, function () { where("crossed") }, function () { release(grab, 30, 15) }, function () { out("release after crossing only"); where("after release") }],
        // the axis and the limits
        [function () { press(axisArea, 15, 15) }, function () { move(axisArea, 40, 40) }, function () { move(axisArea, 45, 60) }, function () { move(axisArea, 60, 200) }, function () { console.log("axis box", axisBox.x, axisBox.y) }, function () { move(axisArea, 60, -300) }, function () { console.log("axis box top", axisBox.x, axisBox.y) }, function () { release(axisArea, 60, -300) }],
        [function () { Qt.quit() }]
    )

    Timer {
        interval: 60
        running: true
        repeat: true
        onTriggered: {
            if (stepNumber < steps.length) steps[stepNumber++]()
        }
    }
}
