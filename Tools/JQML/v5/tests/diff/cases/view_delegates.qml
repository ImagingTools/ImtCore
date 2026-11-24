// TableViewDelegate and TreeViewDelegate (QtQuick.Controls): the required properties a view gives them, the text of the
// display role, highlighted, the tree's indicator; the editor writes the EditRole back when it commits
import QtQuick
import QtQuick.Controls
import Qt.labs.qmlmodels
Item {
    width: 600; height: 500
    TableModel {
        id: tm
        TableModelColumn { display: "name"; edit: "name" }
        TableModelColumn { display: "age" }
        rows: [ { name: "a", age: 1 }, { name: "b", age: 2 } ]
    }
    TreeModel {
        id: tree
        TableModelColumn { display: "name" }
        rows: [ { name: "A", rows: [ { name: "A1" } ] }, { name: "B" } ]
    }
    ItemSelectionModel { id: sm; model: tm }
    ItemSelectionModel { id: treeSm; model: tree }
    TableView {
        id: tv
        width: 300; height: 200
        model: tm
        selectionModel: sm
        alternatingRows: true
        delegate: TableViewDelegate {}
    }
    TreeView {
        id: trv
        y: 250; width: 300; height: 200
        model: tree
        selectionModel: treeSm
        delegate: TreeViewDelegate {}
    }
    function cells(view) {
        var out = []
        for (var r = view.topRow; r <= view.bottomRow; ++r)
            for (var c = view.leftColumn; c <= view.rightColumn; ++c) {
                var d = view.itemAtCell(Qt.point(c, r))
                out.push(d.contentItem.text + "(" + d.row + "," + d.column + (d.current ? " current" : "") + (d.selected ? " selected" : "")
                    + (d.highlighted ? " hl" : "") + (d.editing ? " editing" : "")
                    + (d.depth !== undefined ? " depth " + d.depth + " node " + d.isTreeNode + " children " + d.hasChildren + " expanded " + d.expanded + " indicator " + d.indicator.visible : "") + ")")
            }
        return out.join(" ")
    }
    Component.onCompleted: {
        console.log("table " + cells(tv))
        console.log("tree " + cells(trv))
        sm.select(tm.index(1, 1), ItemSelectionModel.Select)
        sm.setCurrentIndex(tm.index(0, 1), ItemSelectionModel.NoUpdate)
        console.log("table selected " + cells(tv))
        var looks = []
        for (var r = 0; r < 2; ++r)
            for (var c = 0; c < 2; ++c) {
                var d = tv.itemAtCell(Qt.point(c, r))
                looks.push(d.background.color + " " + d.background.border.width + " " + d.background.border.color + " " + d.contentItem.color
                    + " " + d.leftPadding + "/" + d.topPadding + " " + (d.implicitHeight === d.contentItem.implicitHeight))
            }
        console.log("looks " + looks.join(" | "))
        trv.expand(0)
        trv.forceLayout()
        treeSm.setCurrentIndex(tree.index([0, 0], 0), ItemSelectionModel.NoUpdate)
        console.log("tree expanded " + cells(trv))
        editTimer.start()
    }
    Timer {
        id: editTimer
        interval: 10
        onTriggered: {
            tv.edit(tm.index(1, 0))
            console.log("editing " + cells(tv))
            tv.edit(tm.index(0, 0))
            console.log("committed " + JSON.stringify(tm.rows.map(r => r.name)) + " " + cells(tv))
            tv.closeEditor()
            Qt.quit()
        }
    }
}
