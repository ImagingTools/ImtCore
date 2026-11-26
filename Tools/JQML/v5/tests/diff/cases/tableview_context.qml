// TableView delegates of a number, an array, a ListModel and a TableModel: the order the cells are made in, row, column,
// index (column * rows + row), modelData, the roles, the required properties; rows, columns and the content size
import QtQuick
import Qt.labs.qmlmodels
Item {
    width: 400; height: 300
    ListModel { id: lm; ListElement { name: "a"; age: 1 } ListElement { name: "b"; age: 2 } }
    TableModel {
        id: tm
        TableModelColumn { display: "name" }
        TableModelColumn { display: "age"; edit: "age" }
        rows: [ { name: "x", age: 10 }, { name: "y", age: 20 } ]
    }
    property var log: []
    Component {
        id: plain
        Rectangle {
            implicitWidth: 40; implicitHeight: 20
            Component.onCompleted: console.log("plain r" + row + " c" + column + " index=" + index + " modelData=" + (typeof modelData === "undefined" ? "undef" : modelData) + " model.index=" + model.index + " model.row=" + model.row + " model.column=" + model.column)
        }
    }
    Component {
        id: roles
        Rectangle {
            implicitWidth: 40; implicitHeight: 20
            Component.onCompleted: console.log("lm r" + row + " c" + column + " index=" + index + " name=" + name + " model.name=" + model.name + " age=" + age + " modelData=" + (typeof modelData))
        }
    }
    Component {
        id: tmd
        Rectangle {
            implicitWidth: 40; implicitHeight: 20
            Component.onCompleted: console.log("tm r" + row + " c" + column + " index=" + index + " display=" + display + " model.display=" + model.display + " edit=" + (typeof edit === "undefined" ? "undef" : edit) + " modelData=" + (typeof modelData))
        }
    }
    Component {
        id: req
        Rectangle {
            required property int row
            required property int column
            required property var display
            required property bool current
            required property bool selected
            required property bool editing
            implicitWidth: 40; implicitHeight: 20
            Component.onCompleted: console.log("req r" + row + " c" + column + " display=" + display + " current=" + current + " selected=" + selected + " editing=" + editing)
        }
    }
    TableView { id: t1; width: 400; height: 100; model: 3; delegate: plain }
    TableView { id: t2; y: 100; width: 400; height: 100; model: ["p", "q"]; delegate: plain }
    TableView { id: t3; y: 200; width: 400; height: 100; model: lm; delegate: roles }
    TableView { id: t4; y: 300; width: 400; height: 100; model: tm; delegate: tmd }
    TableView { id: t5; y: 400; width: 400; height: 100; model: tm; delegate: req }
    Timer { interval: 50; running: true; onTriggered: {
        console.log("t1 rows " + t1.rows + " columns " + t1.columns + " contentWidth " + t1.contentWidth + " contentHeight " + t1.contentHeight)
        console.log("t3 rows " + t3.rows + " columns " + t3.columns)
        console.log("t4 rows " + t4.rows + " columns " + t4.columns + " index(1,1) row " + t4.index(1,1).row + " col " + t4.index(1,1).column + " valid " + t4.index(1,1).valid + " model.data " + tm.data(t4.index(1,1), "display"))
        console.log("t4 itemAtCell(1,0) " + (t4.itemAtCell(Qt.point(1, 0)) !== null) + " cellAtPosition(45,5) " + t4.cellAtPosition(45, 5) + " columnWidth(0) " + t4.columnWidth(0) + " rowHeight(0) " + t4.rowHeight(0))
        console.log("tm rowCount " + tm.rowCount + " columnCount " + tm.columnCount + " getRow " + JSON.stringify(tm.getRow(1)))
        Qt.quit() } }
}
