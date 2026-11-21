import QtQuick
import QtQuick.Controls
import QtTest

// ScrollBar and ScrollIndicator: attached to a Flickable (its place, size and position, active while it moves, the fade
// of the handle), a press on the track and on the handle, a drag, snapping, increase / decrease, the wheel over the bar,
// minimumSize and positions beyond the ends, an indicator; and on a ListView and a TableView
Item {
    id: root
    width: 600
    height: 400
    Flickable {
        id: f; x: 10; y: 10; width: 200; height: 150; contentWidth: 500; contentHeight: 600; clip: true
        ScrollBar.vertical: ScrollBar { id: vb }
        ScrollBar.horizontal: ScrollBar { id: hb }
    }
    ScrollBar { id: free; x: 300; y: 0; height: 200; size: 0.3; position: 0.2 }
    ScrollBar { id: snap; x: 330; height: 200; size: 0.25; stepSize: 0.25; snapMode: ScrollBar.SnapAlways }
    ScrollBar { id: minimum; x: 360; height: 200; size: 0.01; minimumSize: 0.2; position: 0.5 }
    Flickable {
        id: fi; x: 400; y: 10; width: 100; height: 100; contentHeight: 400
        ScrollIndicator.vertical: ScrollIndicator { id: vi }
    }
    ListView {
        id: list; x: 10; y: 200; width: 150; height: 150; model: 100; clip: true
        delegate: Rectangle { width: 150; height: 30 }
        ScrollBar.vertical: ScrollBar { id: lb }
    }
    function g(s) {
        return [s.parent === f || s.parent === fi || s.parent === list, s.x, s.y, s.width, s.height, s.orientation, s.size, s.position,
                s.visualSize, s.visualPosition, s.active, s.minimumSize, s.visible, s.contentItem.x, s.contentItem.y,
                s.contentItem.width, s.contentItem.height, s.contentItem.opacity, s.contentItem.color, s.contentItem.radius].join(",")
    }
    TestCase {
        name: "ScrollBars"; when: windowShown
        function test_1_attached() {
            console.log("vertical", g(vb)); console.log("horizontal", g(hb)); console.log("free", g(free))
            console.log("area", f.visibleArea.yPosition, f.visibleArea.heightRatio, f.visibleArea.xPosition, f.visibleArea.widthRatio)
            f.contentY = 150
            console.log("scrolled", g(vb), f.visibleArea.yPosition)
            f.contentX = 100
            console.log("scrolled x", g(hb))
            f.width = 300; f.height = 200
            console.log("resized", g(vb), g(hb))
            f.width = 200; f.height = 150; f.contentX = 0
        }
        function test_2_fade() {
            f.flick(0, -800); wait(50)
            console.log("flicking", vb.active, vb.contentItem.opacity)
            f.cancelFlick(); wait(300); console.log("after 300", vb.active, vb.contentItem.opacity)
            wait(600); console.log("after 900", vb.active, vb.contentItem.opacity)
        }
        function test_3_mouse() {
            f.contentY = 0
            mouseClick(vb, 5, 140); console.log("click track", vb.position, f.contentY, vb.pressed)
            mousePress(vb, 5, 40); console.log("press", vb.position, vb.pressed, vb.active, vb.contentItem.color)
            mouseMove(vb, 5, 70); console.log("drag", vb.position, f.contentY)
            mouseRelease(vb, 5, 70); console.log("release", vb.position, vb.pressed)
            vb.increase(); console.log("increase", vb.position, f.contentY)
            vb.decrease(); vb.decrease(); console.log("decrease", vb.position, f.contentY)
            mousePress(snap, 5, 90); mouseMove(snap, 5, 100); console.log("snap", snap.position); mouseRelease(snap, 5, 100)
            console.log("minimum", g(minimum))
            free.position = 0.95; console.log("over", free.visualPosition, free.visualSize)
            free.position = -0.1; console.log("under", free.visualPosition, free.visualSize)
            free.interactive = false; console.log("not interactive", free.implicitWidth, free.contentItem.implicitWidth)
            free.policy = ScrollBar.AlwaysOff; console.log("always off", free.visible)
            free.policy = ScrollBar.AlwaysOn; console.log("always on", free.visible, free.contentItem.opacity)
            free.orientation = Qt.Horizontal; console.log("horizontal", free.horizontal, free.vertical, free.minimumSize)
        }
        function test_4_wheel() {
            f.contentY = 100
            mouseWheel(vb, 5, 40, 0, -120); wait(400); console.log("wheel on bar", vb.position, f.contentY)
        }
        function test_5_views() {
            console.log("indicator", g(vi), vi.implicitWidth)
            fi.contentY = 100; console.log("indicator scrolled", g(vi))
            console.log("list", g(lb))
            list.contentY = 900; console.log("list scrolled", g(lb), list.visibleArea.yPosition, list.visibleArea.heightRatio)
            lb.position = 0.5; console.log("list bar moved", list.contentY)
        }
    }
}
