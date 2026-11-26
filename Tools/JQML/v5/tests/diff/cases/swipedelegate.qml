import QtQuick
import QtQuick.Controls
import QtTest

// SwipeDelegate: the left / right items made on demand, the position during a drag past the drag distance, the release
// opening past 0.5 and closing below, complete / opened / closed / completed, canceled instead of clicked after a drag,
// a click without a drag, open() / close(), behind, the attached SwipeDelegate.pressed and clicked() of an item in
// right, the place of the content and the background
Item {
    width: 400; height: 400
    property var log: []
    SwipeDelegate { id: s; width: 300; text: "Item"
        swipe.left: Rectangle { width: 100; height: s.height; color: "green" }
        swipe.right: Rectangle { id: r; width: 80; height: s.height; anchors.right: parent.right; color: "red"
            SwipeDelegate.onClicked: log.push("right clicked")
            property bool p: SwipeDelegate.pressed
            onPChanged: log.push("right pressed " + p) }
        onClicked: log.push("clicked")
        onCanceled: log.push("canceled")
        onPressedChanged: log.push("pressed " + pressed)
        swipe.onCompleted: log.push("completed")
        swipe.onOpened: log.push("opened " + swipe.position)
        swipe.onClosed: log.push("closed " + swipe.position)
    }
    SwipeDelegate { id: b; y: 100; width: 300; text: "Behind"
        swipe.behind: Rectangle { width: b.width; height: b.height; color: "blue" } }
    function flush(n) { console.log(n + ": " + log.join(", ")); log = [] }
    function where(c) { return [c.swipe.position.toFixed(3), c.swipe.complete, c.contentItem.x.toFixed(1), c.background.x.toFixed(1), c.swipe.leftItem ? c.swipe.leftItem.visible : null, c.swipe.rightItem ? c.swipe.rightItem.visible : null].join(" ") }
    TestCase { name: "Swipe"; when: windowShown
        function test_a_api() {
            console.log("api", s.swipe.position, s.swipe.complete, s.swipe.enabled, s.swipe.leftItem, s.swipe.rightItem, s.swipe.transition !== null, SwipeDelegate.Left, SwipeDelegate.Right, s.padding, s.spacing, s.icon.width, s.implicitHeight > 0)
            mouseClick(s, 150, 20); flush("click")
        }
        function test_b_drag() {
            mousePress(s, 150, 20); wait(20)
            for (let x = 148; x >= 90; x -= 4) { mouseMove(s, x, 20); wait(20); if (x % 20 == 10) console.log("x", x, where(s)) }
            wait(200); mouseRelease(s, 90, 20); flush("small release"); wait(800); console.log("after", where(s)); flush("events")
            mousePress(s, 150, 20); wait(20)
            for (let x = 148; x >= 80; x -= 4) { mouseMove(s, x, 20); wait(20) }
            console.log("dragged", where(s), s.swipe.rightItem.x, s.swipe.rightItem.z)
            wait(200); mouseRelease(s, 80, 20); wait(800); console.log("open", where(s)); flush("events")
            // (Qt handles the events of an item of an open swipe twice: its log is not compared, only what it does)
            mouseClick(s, 260, 20); wait(100); console.log("click right item", log.indexOf("right clicked") >= 0, log.indexOf("clicked") < 0, s.pressed, where(s)); log = []
            mousePress(s, 100, 20); wait(20)
            for (let x = 104; x <= 160; x += 4) { mouseMove(s, x, 20); wait(20) }
            wait(200); mouseRelease(s, 160, 20); wait(800); console.log("dragged back", where(s)); flush("events")
        }
        function test_c_methods() {
            s.swipe.open(SwipeDelegate.Left); wait(800); console.log("open left", where(s)); flush("events")
            s.swipe.close(); wait(800); console.log("closed", where(s)); flush("events")
            s.swipe.open(SwipeDelegate.Right); wait(800); console.log("open right", where(s)); flush("events")
            s.width = 250; console.log("width", where(s))
            s.swipe.close(); wait(800); flush("closed")
            mousePress(b, 50, 20); for (let x = 54; x <= 230; x += 4) { mouseMove(b, x, 20); wait(20) }
            console.log("behind", b.swipe.position.toFixed(3), b.swipe.behindItem !== null, b.swipe.behindItem.x, b.contentItem.x.toFixed(1))
            wait(200); mouseRelease(b, 230, 20); wait(800); console.log("behind released", b.swipe.position, b.swipe.complete)
        }
    }
}
