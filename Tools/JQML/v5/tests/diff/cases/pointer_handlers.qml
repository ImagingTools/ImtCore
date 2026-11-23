import QtQuick
import QtTest

// HoverHandler (hovered without hoverEnabled, margin, accepted devices, blocking) and DragHandler (threshold, axes,
// target, the MouseArea below canceled when it takes the grab)
Item {
    id: root
    width: 300; height: 200
    Rectangle {
        id: card
        x: 10; y: 10; width: 100; height: 50
        HoverHandler { id: hh; margin: 5; onHoveredChanged: console.log("card hovered", hovered, point.position.x, point.position.y) }
        HoverHandler { id: touchOnly; acceptedDevices: PointerDevice.TouchScreen }
    }
    Rectangle {
        id: top
        x: 60; y: 10; width: 30; height: 30
        HoverHandler { id: blocker; blocking: true; onHoveredChanged: console.log("top hovered", hovered) }
    }
    Rectangle {
        id: box
        x: 150; y: 50; width: 40; height: 40
        MouseArea {
            anchors.fill: parent
            onPressed: console.log("area pressed")
            onCanceled: console.log("area canceled")
            onReleased: console.log("area released")
        }
        DragHandler {
            id: dh
            xAxis.minimum: 120; xAxis.maximum: 200
            yAxis.enabled: false
            onActiveChanged: console.log("drag active", active, box.x, box.y)
        }
    }
    TestCase {
        name: "PointerHandlers"
        when: windowShown
        function test_hover() {
            mouseMove(root, 250, 150)
            mouseMove(root, 112, 30)
            console.log("near", hh.hovered, touchOnly.hovered)
            mouseMove(root, 40, 30)
            mouseMove(root, 70, 20)
            console.log("over top", hh.hovered, blocker.hovered)
            mouseMove(root, 250, 150)
            console.log("away", hh.hovered, blocker.hovered, hh.active)
        }
        function test_drag() {
            mousePress(root, 170, 70)
            mouseMove(root, 175, 70)
            console.log("small", dh.active, box.x)
            mouseMove(root, 185, 72)
            mouseMove(root, 230, 90)
            console.log("moved", dh.active, box.x, box.y, dh.activeTranslation.x, dh.persistentTranslation.x, dh.xAxis.activeValue)
            mouseMove(root, 60, 90)
            console.log("limited", box.x, dh.centroid.position.x)
            mouseRelease(root, 60, 90)
            console.log("released", dh.active, box.x, dh.activeTranslation.x, dh.persistentTranslation.x)
        }
    }
}
