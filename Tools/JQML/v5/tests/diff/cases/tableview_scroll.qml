// TableView over 100 rows x 10 columns, the rows of different heights: the edges it loads as the content moves (a short
// move loads and unloads edges, a far one rebuilds from a new top-left cell), the origin and the estimated content size,
// positionViewAt without animation, hidden and explicit column widths, spacing, the reuse of delegates
import QtQuick
import Qt.labs.qmlmodels
Item {
    width: 400; height: 300
    property int made: 0
    property int pooled: 0
    property int reused: 0
    TableModel {
        id: tm
        TableModelColumn { display: "c0" } TableModelColumn { display: "c1" } TableModelColumn { display: "c2" }
        TableModelColumn { display: "c3" } TableModelColumn { display: "c4" } TableModelColumn { display: "c5" }
        TableModelColumn { display: "c6" } TableModelColumn { display: "c7" } TableModelColumn { display: "c8" }
        TableModelColumn { display: "c9" }
        rows: {
            var list = []
            for (var r = 0; r < 100; ++r) {
                var row = {}
                for (var c = 0; c < 10; ++c) row["c" + c] = r * 10 + c
                list.push(row)
            }
            return list
        }
    }
    TableView {
        id: tv
        width: 300; height: 200
        model: tm
        animate: false
        delegate: Rectangle {
            required property int row
            required property int column
            implicitWidth: 60 + (column % 3) * 10
            implicitHeight: 20 + (row % 3) * 5
            Component.onCompleted: made++
            TableView.onPooled: pooled++
            TableView.onReused: reused++
        }
    }
    function state(tag) {
        var cell = tv.itemAtCell(Qt.point(tv.leftColumn, tv.topRow))
        console.log(tag + ": rows " + tv.topRow + "-" + tv.bottomRow + " columns " + tv.leftColumn + "-" + tv.rightColumn
            + " content " + Math.round(tv.contentX) + "," + Math.round(tv.contentY) + " size " + Math.round(tv.contentWidth) + "x" + Math.round(tv.contentHeight)
            + " origin " + Math.round(tv.originX) + "," + Math.round(tv.originY)
            + " first " + (cell ? Math.round(cell.x) + "," + Math.round(cell.y) + " " + cell.width + "x" + cell.height : "none")
            + " made " + made + " pooled " + pooled + " reused " + reused)
    }
    Component.onCompleted: {
        state("initial")
        tv.contentY = 60
        state("y 60")
        tv.contentY = 400
        state("y 400")
        tv.contentY = 1500
        state("y 1500 (far)")
        tv.contentX = 150
        state("x 150")
        tv.contentX = 0; tv.contentY = 0
        state("back")
        tv.positionViewAtRow(50, TableView.AlignTop)
        state("row 50 top")
        tv.positionViewAtCell(Qt.point(8, 90), TableView.AlignRight | TableView.AlignBottom)
        state("cell 8,90 right bottom")
        tv.positionViewAtRow(88, TableView.Contain)
        state("row 88 contain")
        tv.positionViewAtRow(20, TableView.AlignVCenter)
        state("row 20 center")
        tv.positionViewAtColumn(0, TableView.AlignLeft)
        tv.positionViewAtRow(0, TableView.AlignTop)
        state("home")
        console.log("widths " + tv.columnWidth(0) + " " + tv.columnWidth(1) + " " + tv.columnWidth(2) + " implicit " + tv.implicitColumnWidth(1) + " loaded " + tv.isColumnLoaded(1) + " " + tv.isColumnLoaded(9))
        tv.columnWidthProvider = function(column) { return column === 1 ? 0 : -1 }
        tv.forceLayout()
        state("column 1 hidden")
        console.log("widths " + tv.columnWidth(0) + " " + tv.columnWidth(1) + " " + tv.columnWidth(2) + " loaded " + tv.isColumnLoaded(1))
        tv.columnWidthProvider = undefined
        tv.setColumnWidth(2, 120)
        tv.forceLayout()
        state("column 2 at 120")
        console.log("explicit " + tv.explicitColumnWidth(2) + " " + tv.explicitColumnWidth(3) + " width " + tv.columnWidth(2))
        tv.clearColumnWidths()
        tv.rowSpacing = 4
        tv.columnSpacing = 2
        tv.forceLayout()
        state("spacing")
        console.log("cell at 100,30 " + tv.cellAtPosition(100, 30) + " spacing " + tv.cellAtPosition(64, 10) + " with " + tv.cellAtPosition(64, 10, true))
        tv.width = 500
        tv.forceLayout()
        state("wider")
        tv.setRowHeight(0, 50)
        tv.forceLayout()
        state("row 0 at 50")
        Qt.quit()
    }
}
