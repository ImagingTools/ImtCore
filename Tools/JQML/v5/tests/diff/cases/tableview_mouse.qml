import QtQuick
import QtTest
import Qt.labs.qmlmodels

// TableView and the pointer: a tap makes a cell current and clears the selection, with modifiers it does not; the
// edit triggers by a tap and a tap on a selected cell (QTest spaces its clicks: no double tap in qmltestrunner); pointerNavigationEnabled off; resizing a column
// and a row by dragging the grid line the pointer hovers (a press with no hover does not); a drag flicks an interactive view
// and does not tap; the wheel scrolls
Item {
    id: root
    width: 400
    height: 300

    property var log: []

    TableModel {
        id: tableModel
        TableModelColumn { display: "a"; edit: "a" }
        TableModelColumn { display: "b"; edit: "b" }
        TableModelColumn { display: "c"; edit: "c" }
        TableModelColumn { display: "d"; edit: "d" }
        TableModelColumn { display: "e"; edit: "e" }
        TableModelColumn { display: "f"; edit: "f" }
        rows: {
            let rows = []
            for (let i = 0; i < 40; i++)
                rows.push({ a: "a" + i, b: "b" + i, c: "c" + i, d: "d" + i, e: "e" + i, f: "f" + i })
            return rows
        }
    }

    TableView {
        id: tableView
        width: 250
        height: 160
        clip: true
        model: tableModel
        selectionModel: ItemSelectionModel {}
        editTriggers: TableView.NoEditTriggers
        delegate: Rectangle {
            id: cell
            required property bool current
            required property bool selected
            required property bool editing
            required property int row
            required property int column
            implicitWidth: 60
            implicitHeight: 30
            TableView.editDelegate: Rectangle {
                width: parent.width
                height: parent.height
                Component.onCompleted: root.log.push("editor " + cell.row + "," + cell.column)
                Component.onDestruction: root.log.push("editor gone")
            }
        }
        onLayoutChanged: root.log.push("layout")
    }

    function show(label) {
        let i = tableView.selectionModel.currentIndex
        let sel = tableView.selectionModel.selectedIndexes.map(i => i.row + "," + i.column)
        sel.sort()
        console.log(label, "| current", i.row + "," + i.column, "| selected", sel.join(" "),
                    "| view", tableView.contentX, tableView.contentY, "| sizes", tableView.columnWidth(0), tableView.columnWidth(1),
                    tableView.rowHeight(0), tableView.rowHeight(1), "| explicit", tableView.explicitColumnWidth(0), tableView.explicitRowHeight(0),
                    "| moving", tableView.moving, "| focus", tableView.activeFocus, "| log", root.log.filter(l => l !== "layout").join(", "))
        root.log = []
    }

    TestCase {
        name: "TableViewMouse"
        when: windowShown

        function init() {
            tableView.closeEditor()
            tableView.selectionModel.clear()
            tableView.interactive = false
            tableView.pointerNavigationEnabled = true
            tableView.editTriggers = TableView.NoEditTriggers
            tableView.resizableColumns = false
            tableView.resizableRows = false
            tableView.clearColumnWidths()
            tableView.clearRowHeights()
            tableView.contentX = 0
            tableView.contentY = 0
            tableView.forceLayout()
            root.forceActiveFocus()
            root.log = []
            wait(500)
        }

        function test_1_tap() {
            mouseClick(tableView, 70, 40)
            show("tap")
            tableView.selectionModel.select(tableView.index(0, 0), ItemSelectionModel.Select)
            mouseClick(tableView, 130, 70)
            show("tap clears")
            tableView.selectionModel.select(tableView.index(0, 0), ItemSelectionModel.Select)
            mouseClick(tableView, 190, 100, Qt.LeftButton, Qt.ControlModifier)
            show("control tap")
            mouseClick(tableView, 190, 100, Qt.RightButton)
            show("right tap")
            mousePress(tableView, 10, 130)
            show("press on a non-interactive view")
            mouseRelease(tableView, 10, 130)
        }

        function test_2_interactive() {
            tableView.interactive = true
            mousePress(tableView, 70, 40)
            show("press on an interactive view")
            mouseRelease(tableView, 70, 40)
            show("released")
            mouseDrag(tableView, 100, 120, 0, -60)
            show("dragged")
            tableView.contentY = 0
            mouseWheel(tableView, 100, 100, 0, -120)
            show("wheel")
        }

        function test_3_pointerNavigationOff() {
            tableView.pointerNavigationEnabled = false
            mouseClick(tableView, 70, 40)
            show("no pointer navigation")
        }

        function test_4_editTriggers() {
            tableView.editTriggers = TableView.SingleTapped
            mouseClick(tableView, 70, 40)
            show("single tapped")
            tableView.closeEditor()
            show("closed")
            tableView.editTriggers = TableView.DoubleTapped
            mouseClick(tableView, 130, 40)
            show("one tap")
            tableView.editTriggers = TableView.SelectedTapped
            mouseClick(tableView, 70, 70)
            show("not selected")
            tableView.selectionModel.select(tableView.index(2, 1), ItemSelectionModel.Select)
            mouseClick(tableView, 70, 70)
            show("selected tapped")
            tableView.closeEditor()
            show("closed again")
        }

        function test_5_resize() {
            tableView.resizableColumns = true
            tableView.resizableRows = true
            mousePress(tableView, 60, 15)
            mouseMove(tableView, 75, 15)
            mouseMove(tableView, 100, 15)
            mouseRelease(tableView, 100, 15)
            show("press with no hover")
            mouseMove(tableView, 60, 15)
            mousePress(tableView, 60, 15)
            mouseMove(tableView, 75, 15)
            mouseMove(tableView, 100, 15)
            mouseRelease(tableView, 100, 15)
            show("column resized")
            mouseMove(tableView, 15, 30)
            mousePress(tableView, 15, 30)
            mouseMove(tableView, 15, 45)
            mouseMove(tableView, 15, 70)
            mouseRelease(tableView, 15, 70)
            show("row resized")
            tableView.forceLayout()
            show("laid out")
            tableView.resizableColumns = false
            mouseMove(tableView, 85, 15)
            mousePress(tableView, 85, 15)
            mouseMove(tableView, 75, 15)
            mouseMove(tableView, 100, 15)
            mouseRelease(tableView, 100, 15)
            show("not resizable")
        }
    }
}
