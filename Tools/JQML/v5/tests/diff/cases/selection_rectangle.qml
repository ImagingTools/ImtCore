import QtQuick
import QtQuick.Controls
import QtTest
import Qt.labs.qmlmodels

// SelectionRectangle on a TableView: a drag selects, a tap clears, shift and control extend and add, a handle drags a
// corner, a press and hold selects on an interactive view, the Drag mode selects instead of flicking
Item {
    id: root
    width: 400
    height: 300

    property Item topLeft: null
    property Item bottomRight: null
    property int activeChanges: 0
    property int draggingChanges: 0

    TableModel {
        id: tableModel
        TableModelColumn { display: "a" }
        TableModelColumn { display: "b" }
        TableModelColumn { display: "c" }
        TableModelColumn { display: "d" }
        TableModelColumn { display: "e" }
        TableModelColumn { display: "f" }
        rows: {
            let rows = []
            for (let i = 0; i < 30; i++)
                rows.push({ a: "a" + i, b: "b" + i, c: "c" + i, d: "d" + i, e: "e" + i, f: "f" + i })
            return rows
        }
    }

    TableView {
        id: tableView
        width: 300
        height: 200
        clip: true
        interactive: false
        model: tableModel
        selectionModel: ItemSelectionModel {}
        delegate: Rectangle {
            required property bool selected
            implicitWidth: 60
            implicitHeight: 30
            color: selected ? "lightblue" : "white"
        }
    }

    SelectionRectangle {
        id: selectionRectangle
        target: tableView
        topLeftHandle: Rectangle {
            id: tl
            width: 16
            height: 16
            color: "red"
            visible: SelectionRectangle.control.active
            property bool isTopLeft: true
            Component.onCompleted: root.topLeft = tl
        }
        bottomRightHandle: Rectangle {
            id: br
            width: 16
            height: 16
            color: "blue"
            visible: SelectionRectangle.control.active
            property bool dragging: SelectionRectangle.dragging
            Component.onCompleted: root.bottomRight = br
        }
        onActiveChanged: root.activeChanges++
        onDraggingChanged: root.draggingChanges++
    }

    SelectionRectangle {
        id: defaults
    }

    TableView {
        id: basicView
        x: 0
        y: 210
        width: 300
        height: 90
        interactive: false
        model: tableModel
        selectionModel: ItemSelectionModel {}
        delegate: Rectangle {
            implicitWidth: 60
            implicitHeight: 30
        }
    }

    SelectionRectangle {
        id: basicSelection
        target: basicView
    }

    function cells() {
        let list = tableView.selectionModel.selectedIndexes.map(i => i.row + "," + i.column)
        list.sort()
        return list.join(" ")
    }
    function current() {
        let i = tableView.selectionModel.currentIndex
        return i.row + "," + i.column
    }
    function show(label) {
        console.log(label, "|", cells(), "| current", current(), "| active", selectionRectangle.active,
                    "dragging", selectionRectangle.dragging, "| changes", activeChanges, draggingChanges)
    }
    function handles(label) {
        console.log(label, "handles", topLeft ? topLeft.x + "," + topLeft.y + " " + topLeft.visible + " " + topLeft.z + " " + (topLeft.parent === tableView.contentItem) : "none",
                    bottomRight ? bottomRight.x + "," + bottomRight.y + " " + bottomRight.visible + " " + bottomRight.dragging : "none")
    }

    TestCase {
        name: "SelectionRectangle"
        when: windowShown

        function test_1_defaults() {
            console.log("defaults", defaults.selectionMode, defaults.active, defaults.dragging, defaults.target,
                        defaults.topLeftHandle !== null, defaults.bottomRightHandle !== null)
            console.log("enums", SelectionRectangle.Drag, SelectionRectangle.PressAndHold, SelectionRectangle.Auto)
            console.log("target", selectionRectangle.target === tableView, root.topLeft, root.bottomRight)
        }

        function test_2_drag() {
            mousePress(tableView, 30, 15)
            show("pressed")
            mouseMove(tableView, 45, 30)
            show("moved past threshold")
            mouseMove(tableView, 150, 75)
            show("moved")
            handles("dragging")
            mouseRelease(tableView, 150, 75)
            show("released")
            handles("released")
        }

        function test_3_tap() {
            mouseClick(tableView, 200, 120)
            show("tapped")
            handles("tapped")
        }

        function test_4_shift() {
            mouseClick(tableView, 70, 45)
            show("current")
            mouseClick(tableView, 190, 100, Qt.LeftButton, Qt.ShiftModifier)
            show("shift click")
            handles("shift")
            mouseClick(tableView, 250, 160, Qt.LeftButton, Qt.ShiftModifier)
            show("shift click again")
        }

        function test_5_control() {
            mouseClick(tableView, 10, 10)
            show("cleared")
            mouseClick(tableView, 70, 45, Qt.LeftButton, Qt.ControlModifier)
            show("control click")
            mouseClick(tableView, 190, 105, Qt.LeftButton, Qt.ControlModifier)
            show("control click 2")
            mouseClick(tableView, 70, 45, Qt.LeftButton, Qt.ControlModifier)
            show("control click same")
        }

        function test_6_handle() {
            mouseClick(tableView, 10, 10)
            mousePress(tableView, 70, 45)
            mouseMove(tableView, 85, 60)
            mouseMove(tableView, 130, 75)
            mouseRelease(tableView, 130, 75)
            show("selected")
            handles("before handle")
            let h = root.bottomRight
            mousePress(h, 8, 8)
            mouseMove(h, 20, 20)
            show("handle threshold")
            handles("handle threshold")
            mouseMove(h, 75, 40)
            show("handle moved")
            handles("handle moved")
            mouseRelease(h, 75, 40)
            show("handle released")
            handles("handle released")
            let t = root.topLeft
            mousePress(t, 8, 8)
            mouseMove(t, -10, -10)
            mouseMove(t, -60, -30)
            mouseRelease(t, -60, -30)
            show("top left dragged")
            handles("top left dragged")
        }

        function test_7_pressAndHold() {
            mouseClick(tableView, 10, 10)
            tableView.interactive = true
            show("interactive")
            mousePress(tableView, 130, 75)
            wait(1200)
            show("held")
            handles("held")
            mouseRelease(tableView, 130, 75)
            show("held released")
            mouseDrag(tableView, 200, 150, -100, -60)
            show("flicked", tableView.contentY)
            console.log("contentY", tableView.contentY)
        }

        function test_8_dragMode() {
            mouseClick(tableView, 10, 10)
            tableView.contentY = 0
            selectionRectangle.selectionMode = SelectionRectangle.Drag
            mousePress(tableView, 30, 15)
            mouseMove(tableView, 45, 30)
            mouseMove(tableView, 100, 140)
            mouseRelease(tableView, 100, 140)
            show("drag mode")
            console.log("contentY", tableView.contentY)
        }

        function test_a_autoScroll() {
            tableView.cancelFlick()
            tableView.contentX = 0
            tableView.contentY = 0
            selectionRectangle.selectionMode = SelectionRectangle.Auto
            tableView.interactive = false
            mouseClick(tableView, 10, 10)
            mousePress(tableView, 30, 15)
            mouseMove(tableView, 45, 30)
            mouseMove(tableView, 100, 230)
            wait(300)
            console.log("scrolled down", tableView.contentY > 0, selectionRectangle.dragging)
            let rows = tableView.selectionModel.selectedIndexes.map(i => i.row)
            console.log("the end follows the pointer", Math.max(...rows) === tableView.selectionModel.currentIndex.row, Math.max(...rows) >= 6)
            mouseRelease(tableView, 100, 230)
            console.log("after release", selectionRectangle.dragging, selectionRectangle.active)
            mouseClick(tableView, 10, 10)
            tableView.contentY = 0
        }

        function test_b_basicHandles() {
            mousePress(basicView, 30, 15)
            mouseMove(basicView, 45, 30)
            mouseMove(basicView, 130, 45)
            let handles = basicView.contentItem.children.filter(c => c.radius === 14)
            for (let h of handles)
                console.log("basic handle", h.x, h.y, h.width, h.height, h.z, h.visible, h.color, h.border.width, h.border.color)
            mouseRelease(basicView, 130, 45)
            mouseClick(basicView, 250, 75)
            for (let h of handles)
                console.log("basic handle after tap", h.visible)
        }

        function test_9_disabled() {
            selectionRectangle.enabled = false
            show("disabled")
            mousePress(tableView, 30, 15)
            mouseMove(tableView, 45, 30)
            mouseMove(tableView, 100, 140)
            mouseRelease(tableView, 100, 140)
            show("drag while disabled")
            selectionRectangle.enabled = true
        }
    }
}
