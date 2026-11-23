// TableView following its model: rows inserted, removed and moved in a TableModel and a ListModel, data changed in place
// (the roles and the required properties of the delegates), a reset, another model and another delegate, reuseItems off
import QtQuick
import Qt.labs.qmlmodels
Item {
    width: 400; height: 400
    property int made: 0
    TableModel {
        id: tm
        TableModelColumn { display: "name" }
        TableModelColumn { display: "age" }
        rows: [ { name: "a", age: 1 }, { name: "b", age: 2 }, { name: "c", age: 3 } ]
    }
    ListModel {
        id: lm
        ListElement { name: "x" } ListElement { name: "y" }
    }
    Component {
        id: required
        Text {
            required property var display
            required property int row
            required property int column
            text: display
            Component.onCompleted: made++
        }
    }
    Component {
        id: context
        Text {
            text: "[" + model.display + "]" + row + "," + column
            Component.onCompleted: made++
        }
    }
    Component {
        id: listDelegate
        Text {
            text: name + "@" + index
            Component.onCompleted: made++
        }
    }
    TableView {
        id: tv
        width: 300; height: 300
        model: tm
        delegate: required
    }
    function cells() {
        var out = []
        for (var r = tv.topRow; r <= tv.bottomRow; ++r) {
            var line = []
            for (var c = tv.leftColumn; c <= tv.rightColumn; ++c) {
                var item = tv.itemAtCell(Qt.point(c, r))
                line.push(item ? item.text : "-")
            }
            out.push(line.join(" "))
        }
        return out.join(" | ") + " (rows " + tv.rows + " columns " + tv.columns + " made " + made + ")"
    }
    Component.onCompleted: {
        console.log("initial " + cells())
        tm.insertRow(1, { name: "n", age: 9 })
        tv.forceLayout()
        console.log("insert " + cells())
        tm.removeRow(0)
        tv.forceLayout()
        console.log("remove " + cells())
        tm.moveRow(0, 2)
        tv.forceLayout()
        console.log("move " + cells())
        tm.setData(tm.index(1, 1), 77, "display")
        console.log("setData " + cells())
        tm.setRow(0, { name: "z", age: 0 })
        console.log("setRow " + cells())
        tm.rows = [ { name: "p", age: 5 } ]
        tv.forceLayout()
        console.log("reset " + cells())
        tv.delegate = context
        tv.forceLayout()
        console.log("delegate " + cells())
        tv.model = lm
        tv.delegate = listDelegate
        tv.forceLayout()
        console.log("list " + cells())
        lm.append({ name: "w" })
        tv.forceLayout()
        console.log("append " + cells())
        lm.setProperty(0, "name", "X")
        console.log("setProperty " + cells())
        lm.move(0, 2, 1)
        tv.forceLayout()
        console.log("list move " + cells())
        lm.remove(1)
        tv.forceLayout()
        console.log("list remove " + cells())
        tv.reuseItems = false
        lm.clear()
        tv.forceLayout()
        console.log("cleared " + cells() + " contentHeight " + tv.contentHeight)
        tv.model = 3
        tv.delegate = context
        tv.forceLayout()
        console.log("number " + cells())
        Qt.quit()
    }
}
