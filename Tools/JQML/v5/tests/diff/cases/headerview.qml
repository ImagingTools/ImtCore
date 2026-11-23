// HorizontalHeaderView / VerticalHeaderView synced to a TableView: the header data of its model (section + 1 by default),
// the columns it loads and its contentX following the table, a header of a list model (transposed), textRole
import QtQuick
import QtQuick.Controls
import Qt.labs.qmlmodels
Item {
    width: 500; height: 400
    TableModel {
        id: tm
        TableModelColumn { display: "a" } TableModelColumn { display: "b" } TableModelColumn { display: "c" }
        TableModelColumn { display: "d" } TableModelColumn { display: "e" } TableModelColumn { display: "f" }
        rows: [ { a: 1, b: 2, c: 3, d: 4, e: 5, f: 6 }, { a: 7, b: 8, c: 9, d: 10, e: 11, f: 12 } ]
    }
    ListModel { id: lm; ListElement { name: "x" } ListElement { name: "y" } ListElement { name: "z" } }
    HorizontalHeaderView { id: hh; x: 40; width: 200; syncView: tv; clip: true
        delegate: Text {
            required property var model
            required property var headerView
            required property int index
            text: model[headerView.textRole]
        } }
    VerticalHeaderView { id: vh; y: 40; height: 150; syncView: tv; clip: true
        delegate: Text {
            required property var model
            required property var headerView
            required property int index
            text: model[headerView.textRole]
        } }
    TableView {
        id: tv
        x: 40; y: 40; width: 200; height: 150
        model: tm
        animate: false
        delegate: Rectangle { implicitWidth: 60; implicitHeight: 30 }
    }
    HorizontalHeaderView {
        id: listHeader
        y: 300; width: 300; height: 40
        model: lm
        textRole: "name"
        delegate: Text {
            required property var model
            required property var headerView
            required property int index
            text: model[headerView.textRole]
        }
    }
    function texts(view) {
        var out = []
        for (var r = view.topRow; r <= view.bottomRow; ++r)
            for (var c = view.leftColumn; c <= view.rightColumn; ++c) {
                var item = view.itemAtCell(Qt.point(c, r))
                out.push(item ? item.text : "-")
            }
        return out.join(",") + " [" + view.rows + "x" + view.columns + "]"
    }
    Component.onCompleted: {
        console.log("horizontal " + texts(hh) + " columns " + hh.leftColumn + "-" + hh.rightColumn + " width0 " + hh.columnWidth(0))
        console.log("vertical " + texts(vh) + " rows " + vh.topRow + "-" + vh.bottomRow + " height0 " + vh.rowHeight(0))
        console.log("list header " + texts(listHeader) + " textRole " + listHeader.textRole + " hh textRole " + hh.textRole)
        tv.contentX = 130
        console.log("scrolled " + texts(hh) + " contentX " + hh.contentX + " columns " + hh.leftColumn + "-" + hh.rightColumn)
        tv.setColumnWidth(3, 100)
        tv.forceLayout()
        console.log("width3 " + hh.columnWidth(3) + " " + tv.columnWidth(3))
        tv.moveColumn(0, 1)
        tv.forceLayout()
        tv.contentX = 0
        console.log("moved " + texts(hh))
        lm.append({ name: "w" })
        listHeader.forceLayout()
        console.log("list appended " + texts(listHeader))
        Qt.quit()
    }
}
