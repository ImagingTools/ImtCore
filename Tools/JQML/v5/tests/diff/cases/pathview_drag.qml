import QtQuick
import QtTest

// a drag of a PathView: the offset follows the percent of the path moved, the current index follows the offset, the
// release snaps to the item at the highlight; a drag on a MouseArea of a delegate is taken
Item {
    id: root
    width: 400; height: 200
    property var events: []
    PathView {
        id: pv
        anchors.fill: parent
        model: 8
        pathItemCount: 4
        preferredHighlightBegin: 0.5
        preferredHighlightEnd: 0.5
        highlightMoveDuration: 100
        // the speed of a qmltestrunner drag is not the real one (its events do not follow wait()): no flick
        flickDeceleration: 1e9
        onMovementStarted: root.events.push("moveStart")
        onMovementEnded: root.events.push("moveEnd")
        onDragStarted: root.events.push("dragStart")
        onDragEnded: root.events.push("dragEnd")
        delegate: Rectangle {
            width: 60; height: 40
            MouseArea { anchors.fill: parent; onClicked: root.events.push("click " + index); onCanceled: root.events.push("canceled " + index) }
        }
        path: Path { startX: 0; startY: 100; PathLine { x: 400; y: 100 } }
    }
    TestCase {
        name: "PathViewDrag"
        when: windowShown
        function test_drag() {
            let it = pv.itemAtIndex(0)
            console.log(pv.currentIndex, Math.round(it.x + it.width / 2))
            mousePress(root, 200, 100)
            for (let i = 1; i <= 10; i++) mouseMove(root, 200 - i * 12, 100, 20)
            console.log(pv.dragging, pv.moving, pv.currentIndex, pv.offset.toFixed(2))
            wait(200)
            mouseRelease(root, 80, 100)
            tryCompare(pv, "moving", false, 2000)
            console.log(pv.currentIndex, pv.offset.toFixed(2), root.events.join(", "))
            root.events = []
            mouseClick(root, 200, 100)
            console.log(root.events.join(", "), pv.currentIndex)
        }
    }
}
