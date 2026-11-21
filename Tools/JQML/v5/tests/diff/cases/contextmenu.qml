import QtQuick
import QtQuick.Controls
import QtTest

// ContextMenu: a right click opens the menu of the item under it or of the nearest item above with one, at the point
// (relative to that item); requested(position); a MouseArea that takes the right button; the menu made on first use
Item {
    width: 400; height: 400
    property var log: []
    Rectangle { id: a; width: 200; height: 200; color: "gray"
        ContextMenu.menu: Menu { id: am; MenuItem { text: "Cut" } MenuItem { text: "Copy" } onOpened: log.push("a opened " + x + "," + y + " " + (parent === a)) }
        ContextMenu.onRequested: (position) => log.push("a requested " + position.x + "," + position.y)
        Rectangle { id: inner; x: 50; y: 50; width: 50; height: 50; color: "red" }
        MouseArea { id: ma; x: 120; y: 120; width: 50; height: 50; acceptedButtons: Qt.RightButton; onPressed: log.push("ma pressed") }
    }
    Rectangle { id: b; x: 220; width: 150; height: 150; color: "blue"
        ContextMenu.onRequested: (position) => log.push("b requested " + position.x + "," + position.y) }
    Rectangle { id: c; y: 220; width: 100; height: 100 }
    TestCase { name: "Ctx"; when: windowShown
        function flush(n) { console.log(n + ": " + log.join(", ")); log = [] }
        function test_a() {
            console.log("menu", a.ContextMenu.menu !== null, c.ContextMenu.menu)
            mouseClick(a, 10, 20, Qt.RightButton); wait(50); flush("right on a"); console.log("visible", am.visible); am.close(); wait(50)
            mouseClick(inner, 5, 5, Qt.RightButton); wait(50); flush("right on inner"); am.close(); wait(50)
            mouseClick(ma, 5, 5, Qt.RightButton); wait(50); flush("right on mouse area"); am.close(); wait(50)
            mouseClick(a, 10, 20, Qt.LeftButton); wait(50); flush("left on a")
            mouseClick(b, 30, 40, Qt.RightButton); wait(50); flush("right on b")
            mouseClick(c, 30, 40, Qt.RightButton); wait(50); flush("right on c")
        }
    }
}
