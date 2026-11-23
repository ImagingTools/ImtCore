import QtQuick
import QtTest
import Qt.labs.qmlmodels

// TableView and the keys (QQuickTableView::keyPressEvent): the arrows, Page Up / Down, Home / End, Tab / Backtab from
// the current cell; shift selects from it by cells, rows or columns; the selection modes; hidden rows and columns are
// skipped; the view follows the current cell; keyNavigationEnabled off; no current cell yet
Item {
    id: root
    width: 400
    height: 300

    TableModel {
        id: tableModel
        TableModelColumn { display: "a" }
        TableModelColumn { display: "b" }
        TableModelColumn { display: "c" }
        TableModelColumn { display: "d" }
        TableModelColumn { display: "e" }
        TableModelColumn { display: "f" }
        TableModelColumn { display: "g" }
        TableModelColumn { display: "h" }
        rows: {
            let rows = []
            for (let i = 0; i < 40; i++)
                rows.push({ a: i, b: i, c: i, d: i, e: i, f: i, g: i, h: i })
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
        columnWidthProvider: function(column) { return hiddenColumn === column ? 0 : 60 }
        rowHeightProvider: function(row) { return hiddenRow === row ? 0 : 30 }
        property int hiddenColumn: -1
        property int hiddenRow: -1
        delegate: Rectangle {
            required property bool current
            required property bool selected
            implicitWidth: 60
            implicitHeight: 30
        }
    }

    function cells() {
        let list = tableView.selectionModel.selectedIndexes.map(i => i.row + "," + i.column)
        list.sort((a, b) => {
            let pa = a.split(",").map(Number), pb = b.split(",").map(Number)
            return pa[0] - pb[0] || pa[1] - pb[1]
        })
        return list.length > 12 ? list.length + " cells " + list[0] + " .. " + list[list.length - 1] : list.join(" ")
    }
    function show(label) {
        tableView.forceLayout()
        let i = tableView.selectionModel.currentIndex
        console.log(label, "| current", i.row + "," + i.column, tableView.currentRow, tableView.currentColumn,
                    "| view", tableView.contentX, tableView.contentY, tableView.leftColumn, tableView.topRow,
                    "|", cells())
    }

    TestCase {
        name: "TableViewKeys"
        when: windowShown

        function init() {
            tableView.selectionModel.clear()
            tableView.animate = false
            tableView.selectionBehavior = TableView.SelectCells
            tableView.selectionMode = TableView.ExtendedSelection
            tableView.keyNavigationEnabled = true
            tableView.hiddenColumn = -1
            tableView.hiddenRow = -1
            tableView.forceLayout()
            tableView.positionViewAtCell(Qt.point(0, 0), TableView.AlignLeft | TableView.AlignTop)
            tableView.forceLayout()
            tableView.forceActiveFocus()
        }

        function test_1_noCurrent() {
            show("start")
            keyClick(Qt.Key_Down)
            show("down without current")
            keyClick(Qt.Key_Down)
            show("down")
            keyClick(Qt.Key_Right)
            show("right")
            keyClick(Qt.Key_Up)
            keyClick(Qt.Key_Up)
            show("up at top")
            keyClick(Qt.Key_Left)
            keyClick(Qt.Key_Left)
            show("left at start")
        }

        function test_2_follow() {
            tableView.selectionModel.setCurrentIndex(tableView.index(0, 0), ItemSelectionModel.NoUpdate)
            for (let i = 0; i < 7; i++)
                keyClick(Qt.Key_Down)
            show("down 7")
            for (let i = 0; i < 5; i++)
                keyClick(Qt.Key_Right)
            show("right 5")
            keyClick(Qt.Key_PageDown)
            show("page down")
            keyClick(Qt.Key_PageDown)
            show("page down 2")
            keyClick(Qt.Key_PageUp)
            show("page up")
            keyClick(Qt.Key_End)
            show("end")
            keyClick(Qt.Key_Home)
            show("home")
            keyClick(Qt.Key_End, Qt.ControlModifier)
            show("control end")
            keyClick(Qt.Key_Home, Qt.ControlModifier)
            show("control home")
        }

        function test_3_tab() {
            tableView.selectionModel.setCurrentIndex(tableView.index(2, 6), ItemSelectionModel.NoUpdate)
            keyClick(Qt.Key_Tab)
            show("tab")
            keyClick(Qt.Key_Tab)
            show("tab wraps")
            keyClick(Qt.Key_Backtab)
            show("backtab")
            keyClick(Qt.Key_Backtab)
            show("backtab 2")
            tableView.selectionModel.setCurrentIndex(tableView.index(0, 0), ItemSelectionModel.NoUpdate)
            keyClick(Qt.Key_Backtab)
            tableView.forceLayout()
            show("backtab at start")
        }

        function test_4_shiftCells() {
            tableView.selectionModel.setCurrentIndex(tableView.index(1, 1), ItemSelectionModel.NoUpdate)
            keyClick(Qt.Key_Right, Qt.ShiftModifier)
            show("shift right")
            keyClick(Qt.Key_Down, Qt.ShiftModifier)
            keyClick(Qt.Key_Down, Qt.ShiftModifier)
            show("shift down 2")
            keyClick(Qt.Key_Left, Qt.ShiftModifier)
            keyClick(Qt.Key_Left, Qt.ShiftModifier)
            show("shift left 2")
            keyClick(Qt.Key_Up, Qt.ShiftModifier)
            keyClick(Qt.Key_Up, Qt.ShiftModifier)
            keyClick(Qt.Key_Up, Qt.ShiftModifier)
            show("shift up 3")
            keyClick(Qt.Key_Down)
            show("down clears")
        }

        function test_5_shiftRowsColumns() {
            tableView.selectionBehavior = TableView.SelectRows
            tableView.selectionModel.setCurrentIndex(tableView.index(2, 2), ItemSelectionModel.NoUpdate)
            keyClick(Qt.Key_Down, Qt.ShiftModifier)
            show("rows shift down")
            keyClick(Qt.Key_Right, Qt.ShiftModifier)
            show("rows shift right")
            keyClick(Qt.Key_Right)
            tableView.selectionBehavior = TableView.SelectColumns
            keyClick(Qt.Key_Right, Qt.ShiftModifier)
            show("columns shift right")
            keyClick(Qt.Key_Down, Qt.ShiftModifier)
            show("columns shift down")
            tableView.selectionBehavior = TableView.SelectionDisabled
            keyClick(Qt.Key_Right, Qt.ShiftModifier)
            show("disabled shift right")
        }

        function test_6_modes() {
            tableView.selectionMode = TableView.SingleSelection
            tableView.selectionModel.setCurrentIndex(tableView.index(1, 1), ItemSelectionModel.NoUpdate)
            keyClick(Qt.Key_Right, Qt.ShiftModifier)
            show("single shift right")
            keyClick(Qt.Key_Down, Qt.ShiftModifier)
            show("single shift down")
            tableView.selectionMode = TableView.ContiguousSelection
            keyClick(Qt.Key_Down)
            keyClick(Qt.Key_Right, Qt.ShiftModifier)
            show("contiguous shift right")
        }

        function test_7_hidden() {
            tableView.hiddenColumn = 2
            tableView.hiddenRow = 3
            tableView.forceLayout()
            tableView.selectionModel.setCurrentIndex(tableView.index(2, 1), ItemSelectionModel.NoUpdate)
            keyClick(Qt.Key_Right)
            show("right over a hidden column")
            keyClick(Qt.Key_Down)
            show("down over a hidden row")
            keyClick(Qt.Key_Up)
            keyClick(Qt.Key_Left)
            show("back")
            tableView.selectionModel.setCurrentIndex(tableView.index(2, 1), ItemSelectionModel.NoUpdate)
            keyClick(Qt.Key_Right, Qt.ShiftModifier)
            keyClick(Qt.Key_Down, Qt.ShiftModifier)
            show("shift over hidden")
        }

        function test_8_disabled() {
            tableView.keyNavigationEnabled = false
            tableView.selectionModel.setCurrentIndex(tableView.index(1, 1), ItemSelectionModel.NoUpdate)
            keyClick(Qt.Key_Down)
            keyClick(Qt.Key_Right)
            show("navigation disabled")
        }
    }
}
