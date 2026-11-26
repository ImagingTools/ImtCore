import QtQuick

// Drag and DropArea (ReorderableListDelegate): what exists with no drag going on: the attached properties, the defaults of an area and its
// signals. A drag itself is made by the pointer and is checked in test/drag-drop.mjs.
Item {
    width: 100
    height: 100

    Rectangle {
        id: box
        width: 20
        height: 20
        Drag.active: false
        Drag.hotSpot.x: 5
        Drag.hotSpot.y: 6
        Drag.keys: ["row"]
        Drag.dragType: Drag.Internal
    }
    DropArea {
        id: area
        width: 50
        height: 50
        keys: ["row"]
        onEntered: console.log("entered")
        onExited: console.log("exited")
        onDropped: console.log("dropped")
        onPositionChanged: console.log("moved")
    }

    Component.onCompleted: {
        console.log("drag", box.Drag.active, box.Drag.hotSpot.x, box.Drag.hotSpot.y, box.Drag.keys.length, box.Drag.keys[0], box.Drag.dragType === Drag.Internal)
        console.log("area", area.containsDrag, area.keys.length, area.keys[0], typeof area.drag, area.enabled)
        console.log("enums", Drag.None, Drag.Automatic, Drag.Internal)
        Qt.quit()
    }
}
