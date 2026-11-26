import QtQuick
import QtQuick.Controls
import Qt.labs.qmlmodels
import QtTest

// ScrollBar on a TableView (and its header): the size and position of the bars from the estimated content size, a drag of
// the handle to the middle and to the end of a large table (the rows it shows), the keys moving the current cell and the
// bar following, the horizontal bar with columns of different widths
Item {
    id: root
    width: 500
    height: 400

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
            for (let i = 0; i < 1000; i++) rows.push({ a: i, b: i, c: i, d: i, e: i, f: i })
            return rows
        }
    }
    HorizontalHeaderView {
        id: header
        x: tableView.x
        width: tableView.width
        syncView: tableView
        clip: true
    }
    TableView {
        id: tableView
        y: header.height
        width: 400
        height: 300
        clip: true
        model: tableModel
        columnWidthProvider: function(column) { return 60 + column * 20 }
        selectionModel: ItemSelectionModel {}
        delegate: Rectangle { implicitHeight: 30; Text { text: display } }
        ScrollBar.vertical: ScrollBar { id: vbar }
        ScrollBar.horizontal: ScrollBar { id: hbar }
    }
    function bar(b) {
        return [b.parent === tableView, b.x, b.y, b.width, b.height, b.size.toFixed(4), b.position.toFixed(4), b.visualSize.toFixed(4),
                b.contentItem.y.toFixed(2), b.contentItem.height.toFixed(2), b.contentItem.x.toFixed(2), b.contentItem.width.toFixed(2)].join(",")
    }
    function view() {
        return [tableView.contentY.toFixed(1), tableView.contentHeight.toFixed(1), tableView.topRow, tableView.bottomRow,
                tableView.contentX.toFixed(1), tableView.contentWidth.toFixed(1), tableView.leftColumn, tableView.rightColumn, header.contentX.toFixed(1)].join(",")
    }
    TestCase {
        name: "TableViewScrollBar"; when: windowShown
        function test_1_bars() {
            tableView.forceLayout()
            console.log("start", bar(vbar), bar(hbar), "|", view())
            tableView.contentY = 3000; tableView.forceLayout()
            console.log("contentY 3000", bar(vbar), "|", view())
            tableView.contentX = 100; tableView.forceLayout()
            console.log("contentX 100", bar(hbar), "|", view())
        }
        function test_2_drag() {
            tableView.contentY = 0; tableView.contentX = 0; tableView.forceLayout()
            let handle = vbar.contentItem
            mousePress(vbar, 5, handle.y + 2)
            mouseMove(vbar, 5, 150)
            tableView.forceLayout()
            console.log("dragged to the middle", bar(vbar), "|", view())
            mouseMove(vbar, 5, 299)
            mouseRelease(vbar, 5, 299)
            tableView.forceLayout()
            console.log("dragged to the end", bar(vbar), "|", view())
            mouseClick(vbar, 5, 10)
            tableView.forceLayout()
            console.log("click at the top", bar(vbar), "|", view())
        }
        function test_3_keys() {
            tableView.contentY = 0; tableView.forceLayout()
            tableView.forceActiveFocus()
            tableView.selectionModel.setCurrentIndex(tableModel.index(0, 0), ItemSelectionModel.NoUpdate)
            tableView.animate = false
            for (let i = 0; i < 3; i++) keyClick(Qt.Key_PageDown)
            tableView.forceLayout()
            console.log("page down x3", tableView.currentRow, bar(vbar), "|", view())
            keyClick(Qt.Key_End, Qt.ControlModifier)
            tableView.forceLayout()
            console.log("end", tableView.currentRow, bar(vbar), "|", view())
        }
        function test_4_resize() {
            tableView.width = 300; tableView.height = 200; tableView.forceLayout()
            console.log("resized", bar(vbar), bar(hbar), "|", view())
        }
    }
}
