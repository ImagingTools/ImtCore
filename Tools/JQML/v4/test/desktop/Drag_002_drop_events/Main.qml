import QtQuick

// A drag sequence made by a script (the drag by the pointer is checked in test/drag-drop.mjs): the item with Drag.active moves over a DropArea
// (entered, positionChanged, containsDrag, Drag.target), moves out (exited), moves in again and is dropped with Drag.drop() (dropped, the action
// it returns) or ends with Drag.active = false (the end of a drag sequence without a drop). An area with another key takes nothing.
Item {
    width: 300
    height: 200
    property string log: ""

    Rectangle {
        id: box
        x: 10
        y: 10
        width: 40
        height: 40
        Drag.active: false
        Drag.keys: ["row"]
        Drag.hotSpot.x: 20
        Drag.hotSpot.y: 20
        Drag.dragType: Drag.Internal
    }
    DropArea {
        id: area
        x: 150
        y: 10
        width: 100
        height: 100
        keys: ["row"]
        onEntered: (drag) => log += "enter(" + (drag.source === box) + "," + drag.x + "," + drag.y + ") "
        onPositionChanged: (drag) => log += "move(" + drag.x + "," + drag.y + ") "
        onExited: log += "exit "
        onDropped: (drop) => log += "drop(" + (drop.source === box) + "," + drop.x + "," + drop.y + "," + drop.accepted + ") "
    }
    DropArea {
        id: other
        x: 150
        y: 120
        width: 100
        height: 70
        keys: ["column"]
        onEntered: log += "WRONG-ENTER "
        onDropped: log += "WRONG-DROP "
    }
    DropArea {
        id: anyKey
        x: 20
        y: 120
        width: 100
        height: 70
        onEntered: log += "any-enter "
        onExited: log += "any-exit "
        onDropped: log += "any-drop "
    }

    function step(name) {
        console.log(name, log.trim(), "| active", box.Drag.active, "contains", area.containsDrag, "target", box.Drag.target === area ? "area" : box.Drag.target === null ? "null" : "other")
        log = ""
    }

    property int stepNumber: 0
    property var steps: [
        function () { box.Drag.active = true },
        function () { step("started") },
        function () { box.x = 150; box.y = 20 },
        function () { step("over the area") },
        function () { box.x = 160 },
        function () { step("moved inside") },
        function () { box.x = 10; box.y = 10 },
        function () { step("moved out") },
        function () { box.x = 170; box.y = 30 },
        function () { step("moved in again"); log = ""; var action = box.Drag.drop(); console.log("drop action", action, "active", box.Drag.active) },
        function () { step("dropped") },
        function () { box.x = 160; box.y = 20 },
        function () { step("after the drop, moving") },
        function () { box.Drag.active = true },
        function () { box.x = 165 },
        function () { step("started again over the area") },
        function () { box.Drag.active = false },
        function () { step("active = false") },
        function () { box.x = 150; box.y = 130; box.Drag.active = true },
        function () { step("over the area of another key") },
        function () { box.x = 30; box.y = 130 },
        function () { step("over an area with no keys") },
        function () { box.Drag.cancel() },
        function () { step("cancel"); Qt.quit() }
    ]

    Timer {
        interval: 60
        running: true
        repeat: true
        onTriggered: {
            if (stepNumber < steps.length) steps[stepNumber++]()
        }
    }
}
