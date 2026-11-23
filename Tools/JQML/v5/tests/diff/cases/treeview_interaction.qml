import QtQuick
import QtQuick.Controls
import QtTest
import Qt.labs.qmlmodels

// TreeView and the user (QQuickTreeView::keyPressEvent, QQuickTreeViewDelegate): Right expands and goes into a node,
// Left collapses and goes to the parent, Up / Down over the visible rows, shift selects rows; a click on the indicator of
// a TreeViewDelegate toggles; the model changing under expanded and collapsed nodes; a root index; the signals
Item {
    id: root
    width: 400
    height: 400

    property var log: []

    TreeModel {
        id: treeModel
        TableModelColumn { display: "name" }
        TableModelColumn { display: "size" }
        rows: [
            { name: "A", size: 1, rows: [
                { name: "A1", size: 2, rows: [ { name: "A1a", size: 3 }, { name: "A1b", size: 4 } ] },
                { name: "A2", size: 5 }
            ] },
            { name: "B", size: 6, rows: [ { name: "B1", size: 7 } ] },
            { name: "C", size: 8 }
        ]
    }

    TreeView {
        id: treeView
        width: 300
        height: 380
        model: treeModel
        selectionModel: ItemSelectionModel {}
        delegate: TreeViewDelegate {}
        onExpanded: (row, depth) => root.log.push("expanded " + row + " " + depth)
        onCollapsed: (row, recursively) => root.log.push("collapsed " + row + " " + recursively)
    }

    function rows() {
        let out = []
        for (let r = 0; r < treeView.rows; ++r) {
            let item = treeView.itemAtCell(Qt.point(0, r))
            out.push((item ? item.contentItem.text : "?") + (treeView.isExpanded(r) ? "-" : "") + treeView.depth(r))
        }
        return out.join(" ")
    }
    function show(label) {
        treeView.forceLayout()
        let i = treeView.selectionModel.currentIndex
        let sel = treeView.selectionModel.selectedIndexes.map(i => treeView.rowAtIndex(i) + "," + i.column)
        sel.sort()
        console.log(label, "|", rows(), "| current", treeView.currentRow, treeView.currentColumn,
                    "| selected", sel.join(" "), "|", root.log.join(", "))
        root.log = []
    }

    TestCase {
        name: "TreeViewInteraction"
        when: windowShown

        function init() {
            treeView.rootIndex = undefined
            treeView.collapseRecursively()
            treeView.selectionModel.clear()
            treeView.selectionModel.setCurrentIndex(treeView.index(0, 0), ItemSelectionModel.NoUpdate)
            treeView.forceActiveFocus()
            root.log = []
        }

        function test_1_keys() {
            show("start")
            keyClick(Qt.Key_Right)
            show("right expands")
            keyClick(Qt.Key_Right)
            show("right again")
            keyClick(Qt.Key_Down)
            show("down")
            keyClick(Qt.Key_Right)
            show("right on A1")
            keyClick(Qt.Key_Down)
            keyClick(Qt.Key_Down)
            show("down twice")
            keyClick(Qt.Key_Left)
            show("left on a leaf")
            keyClick(Qt.Key_Left)
            show("left again")
            keyClick(Qt.Key_Left)
            show("left at the top")
            keyClick(Qt.Key_Down)
            keyClick(Qt.Key_Down)
            show("down to C")
            keyClick(Qt.Key_Right)
            show("right on a leaf")
            keyClick(Qt.Key_Up)
            show("up")
        }

        function test_2_shiftSelect() {
            treeView.expandRecursively()
            treeView.selectionBehavior = TableView.SelectRows
            keyClick(Qt.Key_Down, Qt.ShiftModifier)
            keyClick(Qt.Key_Down, Qt.ShiftModifier)
            show("shift down")
            keyClick(Qt.Key_Left, Qt.ShiftModifier)
            show("shift left")
            treeView.selectionBehavior = TableView.SelectCells
        }

        function test_3_indicator() {
            let item = treeView.itemAtCell(Qt.point(0, 0))
            mouseClick(item.indicator)
            show("indicator click")
            item = treeView.itemAtCell(Qt.point(0, 1))
            mouseClick(item.indicator)
            show("indicator on A1")
            item = treeView.itemAtCell(Qt.point(0, 0))
            mouseClick(item.indicator)
            show("indicator collapses")
            mouseClick(treeView.itemAtCell(Qt.point(0, 1)), 150, 10)
            show("click on a row")
        }

        function test_4_modelChanges() {
            treeView.expand(0)
            show("expanded A")
            treeModel.appendRow(treeModel.index([0], 0), { name: "A3", size: 9 })
            show("appended to an expanded node")
            treeModel.appendRow(treeModel.index([1], 0), { name: "B2", size: 10 })
            show("appended to a collapsed node")
            treeView.expand(treeView.rowAtIndex(treeModel.index([1], 0)))
            show("expanded B")
            treeModel.insertRow(0, treeModel.index([0], 0), { name: "A0", size: 11 })
            show("inserted at the start")
            treeModel.removeRow(treeModel.index([0, 1], 0))
            show("removed A1")
            treeModel.setRow(treeModel.index([1], 0), { name: "Bee", size: 12 })
            show("set B")
            treeModel.removeRow(treeModel.index([0], 0))
            show("removed A")
            treeModel.clear()
            show("cleared")
            treeModel.rows = [
                { name: "A", size: 1, rows: [
                    { name: "A1", size: 2, rows: [ { name: "A1a", size: 3 }, { name: "A1b", size: 4 } ] },
                    { name: "A2", size: 5 }
                ] },
                { name: "B", size: 6, rows: [ { name: "B1", size: 7 } ] },
                { name: "C", size: 8 }
            ]
            show("rows again")
        }

        function test_5_rootIndex() {
            treeView.rootIndex = treeModel.index([0], 0)
            show("root A")
            treeView.expand(0)
            show("expanded under the root")
            treeView.rootIndex = treeModel.index([1], 0)
            show("root B")
            treeView.rootIndex = undefined
            show("no root")
        }

        function test_6_recursive() {
            treeView.expandRecursively(-1, 1)
            show("depth 1")
            treeView.collapseRecursively(0)
            show("collapsed A recursively")
            treeView.expandRecursively(0)
            show("A recursively")
            treeView.collapse(0)
            treeView.expand(0)
            show("collapse and expand keep the children")
            treeView.expandToIndex(treeModel.index([1, 0], 0))
            show("to B1")
            treeView.toggleExpanded(treeView.rowAtIndex(treeModel.index([1], 0)))
            show("toggled B")
        }
    }
}
