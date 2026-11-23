import QtQuick
import QtQuick.Controls
import QtTest

// SplitView: its items and handles laid out (preferred, minimum, maximum, the filling item), a drag of a handle and its
// limits, a vertical one, saveState
Item {
    width: 600; height: 400
    SplitView { id: sv; width: 500; height: 200
        Rectangle { id: a; SplitView.preferredWidth: 100; SplitView.minimumWidth: 50 }
        Rectangle { id: b; SplitView.fillWidth: true }
        Rectangle { id: c; SplitView.preferredWidth: 120; SplitView.maximumWidth: 200 } }
    SplitView { id: vs; y: 220; width: 300; height: 150; orientation: Qt.Vertical
        Rectangle { id: d; implicitHeight: 40 } Rectangle { id: e } }
    function g(i) { return [i.x, i.y, i.width, i.height].join(",") }
    TestCase { name: "Split"; when: windowShown
        function test_split() {
            console.log("defaults", sv.orientation, sv.resizing, sv.count, sv.handle !== null, sv.focusPolicy)
            console.log("items", g(a), g(b), g(c))
            let h = null; for (let i = 0; i < sv.children.length; i++) { let ch = sv.children[i]; if (ch !== a && ch !== b && ch !== c && ch.width === 6) h = ch }
            console.log("handles", h ? g(h) : "none", h ? h.color : "-")
            mousePress(sv, 103, 100); mouseMove(sv, 133, 100); mouseMove(sv, 163, 100); console.log("dragging", sv.resizing, g(a), g(b)); mouseRelease(sv, 163, 100)
            console.log("after drag", g(a), g(b), a.SplitView.preferredWidth)
            mousePress(sv, a.width + 3, 100); mouseMove(sv, 10, 100); mouseMove(sv, 5, 100); mouseRelease(sv, 5, 100); console.log("min", g(a), g(b))
            sv.width = 700; wait(50); console.log("resized", g(a), g(b), g(c))
            console.log("vertical", g(d), g(e))
            let st = sv.saveState(); console.log("state", typeof st)
        }
    }
}
