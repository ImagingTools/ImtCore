// TableView with a selection model (the required selected / current of the delegates, currentRow / currentColumn),
// indexes and cells, moved columns and rows, a table synced to another, a DelegateChooser, and an editor
import QtQuick
import Qt.labs.qmlmodels
Item {
    width: 600; height: 400
    TableModel {
        id: tm
        TableModelColumn { display: "name"; edit: "name" }
        TableModelColumn { display: "age" }
        TableModelColumn { display: "kind" }
        rows: [ { name: "a", age: 1, kind: "x" }, { name: "b", age: 2, kind: "y" }, { name: "c", age: 3, kind: "x" } ]
    }
    ItemSelectionModel { id: sm; model: tm }
    TableView {
        id: tv
        width: 300; height: 150
        model: tm
        selectionModel: sm
        onCurrentRowChanged: console.log("currentRow " + currentRow)
        onCurrentColumnChanged: console.log("currentColumn " + currentColumn)
        onColumnMoved: (logical, oldVisual, newVisual) => console.log("columnMoved " + logical + " " + oldVisual + " -> " + newVisual)
        onRowMoved: (logical, oldVisual, newVisual) => console.log("rowMoved " + logical + " " + oldVisual + " -> " + newVisual)
        delegate: Rectangle {
            id: cell
            required property var display
            required property bool selected
            required property bool current
            required property bool editing
            required property int row
            required property int column
            property string text: display + (selected ? "*" : "") + (current ? "^" : "") + (editing ? "e" : "")
            implicitWidth: 50; implicitHeight: 20
            TableView.editDelegate: TextInput {
                required property var display
                text: display
                Component.onCompleted: console.log("editor for " + display)
                TableView.onCommit: console.log("commit " + text)
            }
        }
    }
    TableView {
        id: header
        y: 200
        width: 300; height: 30
        syncView: tv
        syncDirection: Qt.Horizontal
        model: tm
        delegate: DelegateChooser {
            role: "kind"
            DelegateChoice { roleValue: "y"; Text { required property int column; text: "Y" + column } }
            DelegateChoice { column: 2; Text { required property int column; text: "C2" } }
            DelegateChoice { Text { required property var display; text: "d" + display } }
        }
    }
    function cells(view) {
        var out = []
        for (var r = view.topRow; r <= view.bottomRow; ++r) {
            var line = []
            for (var c = view.leftColumn; c <= view.rightColumn; ++c) {
                var item = view.itemAtCell(Qt.point(c, r))
                line.push(item ? item.text : "-")
            }
            out.push(line.join(" "))
        }
        return out.join(" | ")
    }
    Component.onCompleted: {
        console.log("start " + cells(tv) + " current " + tv.currentRow + "," + tv.currentColumn)
        console.log("header " + cells(header) + " contentX " + header.contentX + " columns " + header.leftColumn + "-" + header.rightColumn)
        sm.setCurrentIndex(tv.index(1, 2), ItemSelectionModel.NoUpdate)
        console.log("current " + cells(tv))
        sm.select(tv.index(0, 0), ItemSelectionModel.Select | ItemSelectionModel.Rows)
        console.log("select row 0 " + cells(tv))
        sm.select(tm.index(2, 1), ItemSelectionModel.Toggle)
        console.log("toggle " + cells(tv))
        sm.clear()
        console.log("clear " + cells(tv) + " current " + tv.currentRow + "," + tv.currentColumn)
        var i = tv.index(2, 1)
        console.log("index " + i.row + "," + i.column + " cell " + tv.cellAtIndex(i) + " row " + tv.rowAtIndex(i) + " column " + tv.columnAtIndex(i) + " item " + (tv.itemAtIndex(i) ? tv.itemAtIndex(i).text : null))
        tv.moveColumn(0, 2)
        tv.forceLayout()
        console.log("moved column " + cells(tv) + " | header " + cells(header))
        console.log("index after move " + tv.index(0, 0).column + " cell of model column 0 " + tv.cellAtIndex(tm.index(0, 0)))
        tv.moveRow(2, 0)
        tv.forceLayout()
        console.log("moved row " + cells(tv))
        tv.clearColumnReordering()
        tv.clearRowReordering()
        tv.forceLayout()
        console.log("cleared " + cells(tv))
        tv.contentX = 40
        console.log("synced contentX " + header.contentX + " header columns " + header.leftColumn + "-" + header.rightColumn)
        tv.contentX = 0
        editTimer.start()
    }
    // an editor needs the window the item is shown in (Qt dereferences window()->focusObject())
    Timer {
        id: editTimer
        interval: 10
        onTriggered: {
            tv.edit(tv.index(1, 0))
            console.log("editing " + cells(tv))
            tv.closeEditor()
            console.log("closed " + cells(tv))
            tv.edit(tv.index(1, 1))
            console.log("not editable " + cells(tv))
            tv.edit(tv.index(2, 0))
            console.log("editing again " + cells(tv))
            tv.edit(tv.index(0, 0))
            console.log("switched " + cells(tv))
            tv.closeEditor()
            Qt.quit()
        }
    }
}
