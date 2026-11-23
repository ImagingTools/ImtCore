import QtQuick
import QtTest
import Qt.labs.qmlmodels

// TableView positioning (QQuickTableView::positionViewAtCell and the rest): every alignment on both axes with and
// without an offset, Contain from either side, the margins and the spacing, animated (the end position), cells at the
// ends of the table; the edges that are loaded and the signals of the top / left cells
Item {
    id: root
    width: 500
    height: 400

    property int leftChanges: 0
    property int topChanges: 0

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
        TableModelColumn { display: "i" }
        TableModelColumn { display: "j" }
        TableModelColumn { display: "k" }
        TableModelColumn { display: "l" }
        rows: {
            let rows = []
            for (let i = 0; i < 60; i++)
                rows.push({ a: i, b: i, c: i, d: i, e: i, f: i, g: i, h: i, i: i, j: i, k: i, l: i })
            return rows
        }
    }

    TableView {
        id: tableView
        width: 230
        height: 170
        clip: true
        model: tableModel
        columnSpacing: 3
        rowSpacing: 2
        leftMargin: 7
        rightMargin: 11
        topMargin: 5
        bottomMargin: 13
        columnWidthProvider: function(column) { return 40 + (column % 3) * 15 }
        rowHeightProvider: function(row) { return 20 + (row % 4) * 6 }
        delegate: Rectangle { implicitWidth: 40; implicitHeight: 20 }
        onLeftColumnChanged: root.leftChanges++
        onTopRowChanged: root.topChanges++
    }

    function show(label) {
        console.log(label, "| content", Math.round(tableView.contentX * 100) / 100, Math.round(tableView.contentY * 100) / 100,
                    "| edges", tableView.leftColumn, tableView.rightColumn, tableView.topRow, tableView.bottomRow,
                    "| size", Math.round(tableView.contentWidth), Math.round(tableView.contentHeight),
                    "| origin", tableView.originX, tableView.originY,
                    "| loaded", tableView.isColumnLoaded(tableView.leftColumn), tableView.isRowLoaded(tableView.bottomRow + 1),
                    "| signals", leftChanges, topChanges)
        leftChanges = 0
        topChanges = 0
    }

    TestCase {
        name: "TableViewPosition"
        when: windowShown

        function init() {
            tableView.animate = false
            tableView.positionViewAtCell(Qt.point(0, 0), TableView.AlignLeft | TableView.AlignTop)
            leftChanges = 0
            topChanges = 0
        }

        function test_1_alignments() {
            show("start")
            let alignments = [
                ["left top", TableView.AlignLeft | TableView.AlignTop],
                ["right bottom", TableView.AlignRight | TableView.AlignBottom],
                ["center", TableView.AlignHCenter | TableView.AlignVCenter],
                ["center flag", TableView.AlignCenter],
                ["left vcenter", TableView.AlignLeft | TableView.AlignVCenter],
                ["hcenter bottom", TableView.AlignHCenter | TableView.AlignBottom],
                ["contain", TableView.Contain],
            ]
            for (let a of alignments) {
                tableView.positionViewAtCell(Qt.point(6, 25), a[1])
                show(a[0])
            }
            for (let a of alignments) {
                tableView.positionViewAtCell(Qt.point(6, 25), a[1], Qt.point(-9, 4))
                show(a[0] + " offset")
            }
        }

        function test_2_contain() {
            tableView.positionViewAtCell(Qt.point(2, 3), TableView.Contain)
            show("contain visible")
            tableView.positionViewAtCell(Qt.point(9, 40), TableView.Contain)
            show("contain below right")
            tableView.positionViewAtCell(Qt.point(3, 30), TableView.Contain)
            show("contain above left")
            tableView.positionViewAtCell(Qt.point(3, 30), TableView.Contain, Qt.point(5, 5))
            show("contain offset")
        }

        function test_3_rowsColumns() {
            tableView.positionViewAtRow(30, TableView.AlignBottom)
            show("row bottom")
            tableView.positionViewAtColumn(8, TableView.AlignHCenter)
            show("column center")
            tableView.positionViewAtRow(30, TableView.AlignTop, -5)
            show("row top offset")
            tableView.positionViewAtColumn(8, TableView.AlignRight, 12)
            show("column right offset")
            tableView.positionViewAtIndex(tableView.index(45, 10), TableView.AlignLeft | TableView.AlignBottom)
            show("index")
        }

        function test_4_ends() {
            tableView.positionViewAtCell(Qt.point(11, 59), TableView.AlignRight | TableView.AlignBottom)
            show("last cell")
            tableView.positionViewAtCell(Qt.point(11, 59), TableView.AlignLeft | TableView.AlignTop)
            show("last cell left top")
            tableView.positionViewAtCell(Qt.point(0, 0), TableView.AlignRight | TableView.AlignBottom)
            show("first cell right bottom")
            tableView.positionViewAtRow(59, TableView.AlignTop)
            tableView.positionViewAtColumn(0, TableView.AlignLeft)
            show("row 59 top")
        }

        function test_5_animated() {
            tableView.animate = true
            tableView.positionViewAtCell(Qt.point(7, 33), TableView.AlignHCenter | TableView.AlignVCenter)
            wait(1500)
            show("animated")
            tableView.positionViewAtCell(Qt.point(1, 2), TableView.AlignLeft | TableView.AlignTop)
            wait(1500)
            show("animated back")
        }

        function test_6_flick() {
            tableView.contentY = 600
            tableView.contentX = 150
            tableView.forceLayout()
            show("moved")
            tableView.contentY = -40
            tableView.contentX = -30
            tableView.forceLayout()
            show("before the start")
        }
    }
}
