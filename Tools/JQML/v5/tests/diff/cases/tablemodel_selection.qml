// TableModel (Qt.labs.qmlmodels) and ItemSelectionModel (QtQml.Models) without a view: the rows, the roles, the signals of
// the model; selections by every flag, and a selection that follows its rows while they are inserted, moved and removed
import QtQuick
import Qt.labs.qmlmodels
Item {
    TableModel {
        id: tm
        TableModelColumn { display: "name" }
        TableModelColumn { display: "age"; edit: "age" }
        TableModelColumn { display: function(index) { return "f" + index.row } }
        rows: [ { name: "a", age: 1 }, { name: "b", age: 2 }, { name: "c", age: 3 }, { name: "d", age: 4 } ]
        onRowsInserted: (parent, first, last) => console.log("rowsInserted " + first + " " + last)
        onRowsRemoved: (parent, first, last) => console.log("rowsRemoved " + first + " " + last)
        onRowsMoved: (sp, ss, se, dp, dr) => console.log("rowsMoved " + ss + " " + se + " " + dr)
        onDataChanged: (tl, br, roles) => console.log("dataChanged " + tl.row + "," + tl.column + " " + br.row + "," + br.column + " roles " + JSON.stringify(roles))
        onModelReset: console.log("modelReset")
        onRowCountChanged: console.log("rowCount -> " + rowCount)
    }
    ItemSelectionModel {
        id: sm
        model: tm
        onSelectionChanged: (selected, deselected) => console.log("selectionChanged +" + ranges(selected) + " -" + ranges(deselected))
        onCurrentChanged: (current, previous) => console.log("currentChanged " + cell(current) + " from " + cell(previous))
    }
    function cell(i) { return i.valid ? i.row + ":" + i.column : "invalid" }
    function ranges(sel) {
        var out = []
        for (var i = 0; i < sel.length; ++i) out.push("[" + sel[i].top + "," + sel[i].left + "-" + sel[i].bottom + "," + sel[i].right + "]")
        return out.join("")
    }
    function selected() {
        var out = []
        for (var r = 0; r < tm.rowCount; ++r) {
            var line = ""
            for (var c = 0; c < tm.columnCount; ++c) line += sm.isSelected(tm.index(r, c)) ? "x" : "."
            out.push(line)
        }
        return out.join("|") + " has " + sm.hasSelection + " rows " + JSON.stringify(sm.selectedRows(0).map(i => i.row))
    }
    Component.onCompleted: {
        console.log("counts " + tm.rowCount + " " + tm.columnCount)
        console.log("data " + tm.data(tm.index(1, 0), "display") + " " + tm.data(tm.index(1, 1), "display") + " " + tm.data(tm.index(2, 2), "display") + " " + tm.data(tm.index(1, 1), "edit"))
        console.log("index valid " + tm.index(3, 2).valid + " " + tm.index(4, 0).valid + " " + tm.index(0, 3).valid)
        console.log("getRow " + JSON.stringify(tm.getRow(2)))
        tm.appendRow({ name: "e", age: 5 })
        tm.insertRow(1, { name: "x", age: 9 })
        console.log("names " + JSON.stringify(tm.rows.map(r => r.name)))
        tm.moveRow(0, 3, 2)
        console.log("names " + JSON.stringify(tm.rows.map(r => r.name)))
        tm.moveRow(4, 1)
        console.log("names " + JSON.stringify(tm.rows.map(r => r.name)))
        tm.removeRow(1, 2)
        console.log("names " + JSON.stringify(tm.rows.map(r => r.name)))
        console.log("setData " + tm.setData(tm.index(0, 1), 42, "edit") + " " + tm.data(tm.index(0, 1), "display"))
        tm.setRow(1, { name: "y", age: 7 })
        console.log("names " + JSON.stringify(tm.rows.map(r => r.name)))

        sm.select(tm.index(0, 0), ItemSelectionModel.Select)
        console.log(selected())
        sm.select(tm.index(1, 1), ItemSelectionModel.Select | ItemSelectionModel.Rows)
        console.log(selected())
        sm.select(tm.index(1, 2), ItemSelectionModel.Toggle)
        console.log(selected())
        sm.select(tm.index(2, 1), ItemSelectionModel.ClearAndSelect | ItemSelectionModel.Columns)
        console.log(selected())
        sm.select(tm.index(0, 1), ItemSelectionModel.Deselect)
        console.log(selected())
        sm.setCurrentIndex(tm.index(2, 0), ItemSelectionModel.SelectCurrent)
        console.log(selected())
        sm.setCurrentIndex(tm.index(3, 0), ItemSelectionModel.SelectCurrent)
        console.log(selected() + " current " + cell(sm.currentIndex))
        sm.clearSelection()
        console.log(selected())
        sm.select(tm.index(1, 0), ItemSelectionModel.Select | ItemSelectionModel.Rows)
        sm.select(tm.index(3, 0), ItemSelectionModel.Select | ItemSelectionModel.Rows)
        console.log(selected())
        tm.insertRow(0, { name: "z", age: 0 })
        console.log("after insert " + selected())
        tm.moveRow(0, 4)
        console.log("after move " + selected())
        tm.removeRow(1)
        console.log("after remove " + selected())
        tm.rows = [ { name: "p", age: 1 } ]
        console.log("after reset " + selected() + " current " + cell(sm.currentIndex))
        tm.clear()
        console.log("cleared " + tm.rowCount)
        Qt.quit()
    }
}
