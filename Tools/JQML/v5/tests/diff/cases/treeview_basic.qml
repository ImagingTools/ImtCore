// TreeView over a TreeModel: the visible rows and their depth, expanded and hasChildren; expand, collapse, recursively,
// to an index; the tree's indexes from index() and cellAtIndex(); a selection of rows; the model changing under it
import QtQuick
import Qt.labs.qmlmodels
Item {
    width: 400; height: 400
    TreeModel {
        id: tree
        TableModelColumn { display: "name" }
        TableModelColumn { display: "size" }
        rows: [
            { name: "A", size: 1, rows: [
                { name: "A1", size: 11, rows: [ { name: "A1a", size: 111 }, { name: "A1b", size: 112 } ] },
                { name: "A2", size: 12 } ] },
            { name: "B", size: 2 },
            { name: "C", size: 3, rows: [ { name: "C1", size: 31 } ] }
        ]
    }
    ItemSelectionModel { id: sm; model: tree }
    TreeView {
        id: tv
        width: 300; height: 300
        model: tree
        selectionModel: sm
        onExpanded: (row, depth) => console.log("expanded " + row + " " + depth)
        onCollapsed: (row, recursively) => console.log("collapsed " + row + " " + recursively)
        delegate: Item {
            required property TreeView treeView
            required property bool isTreeNode
            required property bool expanded
            required property bool hasChildren
            required property int depth
            required property int row
            required property int column
            required property var display
            required property bool selected
            required property bool current
            property string text: (isTreeNode ? "  ".repeat(depth) + (hasChildren ? (expanded ? "-" : "+") : " ") : "") + display + (selected ? "*" : "") + (current ? "^" : "")
            implicitWidth: 80; implicitHeight: 20
        }
    }
    function rows() {
        var out = []
        for (var r = tv.topRow; r <= tv.bottomRow; ++r) {
            var line = []
            for (var c = tv.leftColumn; c <= tv.rightColumn; ++c) {
                var item = tv.itemAtCell(Qt.point(c, r))
                line.push(item ? item.text : "-")
            }
            out.push(line.join(":"))
        }
        return out.join(" | ") + " (rows " + tv.rows + ")"
    }
    Component.onCompleted: {
        console.log("start " + rows())
        tv.expand(0)
        tv.forceLayout()
        console.log("expand 0 " + rows() + " depth(1) " + tv.depth(1) + " isExpanded(0) " + tv.isExpanded(0))
        tv.expand(1)
        tv.forceLayout()
        console.log("expand 1 " + rows())
        tv.collapse(0)
        tv.forceLayout()
        console.log("collapse 0 " + rows())
        tv.expand(0)
        tv.forceLayout()
        console.log("expand 0 again (A1 stays expanded) " + rows())
        tv.collapseRecursively()
        tv.forceLayout()
        console.log("collapse all " + rows())
        tv.expandRecursively()
        tv.forceLayout()
        console.log("expand all " + rows())
        tv.collapseRecursively()
        tv.expandToIndex(tree.index([0, 0, 1], 0))
        tv.forceLayout()
        console.log("to A1b " + rows())
        var i = tv.index(3, 1)
        console.log("index(3,1) " + tree.data(i, "display") + " row " + i.row + " parent valid " + i.parent.valid + " cell of A2 " + tv.cellAtIndex(tree.index([0, 1], 0)) + " rowAtIndex(C) " + tv.rowAtIndex(tree.index([2], 0)))
        sm.select(tv.index(1, 0), ItemSelectionModel.Select | ItemSelectionModel.Rows)
        sm.setCurrentIndex(tv.index(4, 0), ItemSelectionModel.NoUpdate)
        console.log("selected " + rows())
        tree.appendRow(tree.index([0, 0], 0), { name: "A1c", size: 113 })
        tv.forceLayout()
        console.log("appended " + rows())
        tree.removeRow(tree.index([0, 0, 0], 0))
        tv.forceLayout()
        console.log("removed " + rows())
        tree.setRow(tree.index([1], 0), { name: "Bee", size: 22 })
        console.log("setRow " + rows())
        tv.toggleExpanded(0)
        tv.forceLayout()
        console.log("toggled " + rows())
        tv.rootIndex = tree.index([0], 0)
        tv.forceLayout()
        console.log("root A " + rows())
        tv.rootIndex = undefined
        tv.forceLayout()
        console.log("root reset " + rows())
        Qt.quit()
    }
}
