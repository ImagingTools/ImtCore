import QtQuick
import QtQuick.Controls
import QtTest

// Tumbler: the path view of a wrapping one and the ListView of another, the place, size and displacement of the
// delegates, wrap from count and visibleItemCount, a new current index (animated), Up / Down, positionViewAtIndex, a new
// model, a drag (the offset follows past the drag distance; after the release the view rests on an item), the Basic
// delegate (opacity by displacement)
Item {
    width: 400; height: 400
    property var log: []
    Tumbler { id: t; model: 10; onCurrentIndexChanged: log.push("ci " + currentIndex)
        delegate: Text { required property var modelData; required property int index; property real d: Tumbler.displacement; property var tum: Tumbler.tumbler; text: modelData } }
    Tumbler { id: t2; x: 100; model: 3
        delegate: Text { required property var modelData; required property int index; property real d: Tumbler.displacement; text: modelData } }
    Tumbler { id: t3; x: 200; model: ["a", "b", "c", "d", "e", "f"]; currentIndex: 2; wrap: false; onMovingChanged: log.push("moving " + moving)
        delegate: Text { required property var modelData; required property int index; property real d: Tumbler.displacement; text: modelData } }
    Tumbler { id: t4; x: 300; model: 7; currentIndex: 1 }
    function dump(tt) {
        let v = tt.contentItem.children[0], list = v.contentItem ? v.contentItem.children : v.children
        let arr = []
        for (let i = 0; i < list.length; i++) {
            let p = list[i].mapToItem(tt, 0, 0)
            if (list[i].d !== undefined && p.y >= -list[i].height - 0.5 && p.y < tt.height - 0.5) arr.push(list[i])
        }
        arr.sort((a, b) => a.y - b.y)
        let s = []
        for (let i = 0; i < arr.length; i++) { let c = arr[i]; let p = c.mapToItem(tt, 0, 0); s.push(c.text + "@" + p.y.toFixed(1) + "/" + c.width + "x" + c.height.toFixed(1) + " d" + c.d.toFixed(2)) }
        return s.join(" ")
    }
    function rests(tt) {
        let v = tt.contentItem.children[0]
        let p = tt.currentItem.mapToItem(tt, 0, 0)
        return Math.abs(p.y - (tt.availableHeight / tt.visibleItemCount) * Math.floor(tt.visibleItemCount / 2)) < 0.01
    }
    TestCase { name: "Tumbler"; when: windowShown
        function test_a_layout() {
            wait(1500)
            console.log("t", t.count, t.currentIndex, t.wrap, t.visibleItemCount, t.moving, t.currentItem.text, t.currentItem.tum === t, t.implicitWidth, t.implicitHeight, t.contentItem.width, t.contentItem.height)
            console.log(dump(t))
            console.log("t2", t2.wrap, dump(t2))
            console.log("t3", t3.wrap, t3.currentItem.text, dump(t3))
            console.log("fd", t.flickDeceleration, t2.flickDeceleration, t3.flickDeceleration, t.focusPolicy)
            console.log("t4", t4.wrap, t4.currentItem.text, t4.currentItem.opacity, t4.currentItem.width, t4.currentItem.height, t4.currentItem.horizontalAlignment, t4.currentItem.verticalAlignment)
        }
        function test_b_index() {
            log = []
            t.currentIndex = 3; console.log("set", t.currentIndex, log.join(",")); wait(1200); console.log("set 3", t.currentItem.text, dump(t)); log = []
            t.forceActiveFocus(); keyClick(Qt.Key_Down); console.log("down", t.currentIndex); wait(1200); console.log(dump(t), log.join(",")); log = []
            keyClick(Qt.Key_Up); keyClick(Qt.Key_Up); wait(1200); console.log("up up", t.currentIndex, dump(t), log.join(",")); log = []
            t.currentIndex = 0; wait(1200); keyClick(Qt.Key_Up); wait(1200); console.log("up wrap", t.currentIndex, dump(t), log.join(",")); log = []
            t3.forceActiveFocus(); keyClick(Qt.Key_Down); wait(1200); console.log("t3 down", t3.currentIndex, dump(t3))
            t3.currentIndex = 5; wait(1200); keyClick(Qt.Key_Down); wait(1200); console.log("t3 end", t3.currentIndex, dump(t3))
            t.currentIndex = 20; console.log("clamped", t.currentIndex); t.currentIndex = -3; console.log("clamped", t.currentIndex); wait(1200)
        }
        function test_c_position() {
            t.visibleItemCount = 3; wait(50); console.log("vic3", t.wrap, dump(t))
            t.visibleItemCount = 5; wait(50)
            t.positionViewAtIndex(6, Tumbler.Center); wait(50); console.log("pos6", t.currentIndex, dump(t))
            t.positionViewAtIndex(1, Tumbler.Beginning); wait(50); console.log("pos1 beginning", t.currentIndex)
            t.positionViewAtIndex(1, Tumbler.End); wait(50); console.log("pos1 end", t.currentIndex)
            t.positionViewAtIndex(5, Tumbler.SnapPosition); wait(50); console.log("pos5 snap", t.currentIndex)
            t.currentIndex = 0; wait(1200)
        }
        function test_d_drag() {
            let v = t.contentItem.children[0]
            log = []
            mousePress(t, 30, 100); wait(20)
            for (let y = 98; y >= 60; y -= 2) { mouseMove(t, 30, y); wait(20); if (y % 10 == 0) console.log("y", y, v.offset.toFixed(3), t.currentIndex, t.moving) }
            wait(300); mouseRelease(t, 30, 60); wait(3000)
            console.log("released", t.moving, Math.abs(v.offset - Math.round(v.offset)) < 1e-6, rests(t), log[0])
            log = []
            let lv = t3.contentItem.children[0]
            t3.currentIndex = 2; wait(1200)
            mousePress(t3, 30, 100); wait(20)
            for (let y = 98; y >= 40; y -= 2) { mouseMove(t3, 30, y); wait(20); if (y % 20 == 0) console.log("lv y", y, lv.contentY.toFixed(3), t3.currentIndex, t3.moving) }
            wait(300); mouseRelease(t3, 30, 40); wait(3000)
            console.log("lv released", t3.moving, rests(t3), log.join(","))
        }
        function test_e_wrap() {
            t3.currentIndex = 5; wait(1200)
            t3.wrap = true; wait(50); console.log("t3 wrap", t3.wrap, t3.currentIndex, dump(t3))
            t3.wrap = undefined; console.log("reset", t3.wrap)
            t.model = 4; wait(50); console.log("model 4", t.count, t.currentIndex, t.wrap, dump(t))
            t.model = 12; wait(50); console.log("model 12", t.count, t.currentIndex, t.wrap, dump(t))
        }
    }
}
