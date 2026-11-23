import QtQuick
import QtQml.Models
import Qt.labs.qmlmodels

// The delegates of a TableView over their lives (QQmlTableInstanceModel): made, pooled and reused as the view
// scrolls (TableView.pooled / reused: the counts, the order of the pool is QHash's in Qt), destroyed with reuseItems off; a delegate
// writing its row back through model.<role>, through a required property (delegateModelAccess) and modelData of an
// array; the delegates of a new model
Item {
    width: 400
    height: 300

    property int made: 0
    property int destroyed: 0
    property int pooled: 0
    property int reused: 0

    TableModel {
        id: tableModel
        TableModelColumn { display: "name"; edit: "name" }
        TableModelColumn { display: "value" }
        rows: {
            let rows = []
            for (let i = 0; i < 50; i++)
                rows.push({ name: "n" + i, value: i })
            return rows
        }
    }

    ListModel {
        id: listModel
        ListElement { name: "x"; value: 1 }
        ListElement { name: "y"; value: 2 }
        ListElement { name: "z"; value: 3 }
    }

    Component {
        id: modelWriter
        Rectangle {
            id: w
            required property int row
            required property var model
            implicitWidth: 100
            implicitHeight: 30
            function write(value) { w.model.display = value }
        }
    }

    Component {
        id: requiredWriter
        Rectangle {
            id: r
            required property int row
            required property var display
            implicitWidth: 100
            implicitHeight: 30
            function write(value) { r.display = value }
        }
    }

    Component {
        id: arrayDelegate
        Rectangle {
            required property var modelData
            required property int index
            implicitWidth: 100
            implicitHeight: 30
        }
    }

    TableView {
        id: tableView
        width: 220
        height: 100
        clip: true
        model: tableModel
        delegate: Rectangle {
            id: cell
            required property int row
            required property int column
            required property var display
            property string history: ""
            implicitWidth: 100
            implicitHeight: 30
            Component.onCompleted: made++
            Component.onDestruction: destroyed++
            TableView.onPooled: { pooled++; history += "p" + row }
            TableView.onReused: { reused++; history += "r" + row }
        }
    }

    function counts(label) {
        let rows = []
        for (let r = tableView.topRow; r <= tableView.bottomRow; ++r) {
            let item = tableView.itemAtCell(Qt.point(0, r))
            rows.push(item.row + ":" + item.display)
        }
        console.log(label, "| made", made, "destroyed", destroyed, "pooled", pooled, "reused", reused,
                    "| rows", tableView.topRow, tableView.bottomRow, "|", rows.join(" "))
    }

    Component.onCompleted: {
        counts("start")
        tableView.contentY = 35
        tableView.forceLayout()
        counts("a row down")
        tableView.contentY = 200
        tableView.forceLayout()
        counts("further")
        tableView.contentY = 0
        tableView.forceLayout()
        counts("back")
        tableView.contentY = 1000
        tableView.forceLayout()
        counts("far")
        tableView.contentY = 0
        tableView.forceLayout()

        tableView.reuseItems = false
        tableView.contentY = 120
        tableView.forceLayout()
        counts("no reuse")
        tableView.reuseItems = true
        tableView.contentY = 0
        tableView.forceLayout()

        tableView.delegate = modelWriter
        tableView.forceLayout()
        tableView.itemAtCell(Qt.point(0, 1)).write("written")
        console.log("model.display write", tableModel.rows[1].name, tableView.itemAtCell(Qt.point(0, 1)).model.display)

        console.log("access", tableView.delegateModelAccess, DelegateModel.Qt5ReadWrite, DelegateModel.ReadOnly, DelegateModel.ReadWrite)
        tableView.delegate = requiredWriter
        tableView.forceLayout()
        tableView.itemAtCell(Qt.point(0, 2)).write("required default")
        console.log("required default", tableModel.rows[2].name, tableView.itemAtCell(Qt.point(0, 2)).display)
        tableView.delegateModelAccess = DelegateModel.ReadWrite
        tableView.itemAtCell(Qt.point(0, 3)).write("required readwrite")
        console.log("required readwrite", tableModel.rows[3].name, tableView.itemAtCell(Qt.point(0, 3)).display)
        tableView.delegateModelAccess = DelegateModel.ReadOnly
        tableView.delegate = modelWriter
        tableView.forceLayout()
        let threw = false
        try { tableView.itemAtCell(Qt.point(0, 4)).write("readonly") } catch (e) { threw = true }
        console.log("readonly", tableModel.rows[4].name, threw)
        tableView.delegateModelAccess = DelegateModel.Qt5ReadWrite

        tableView.model = ["p", "q", "r"]
        tableView.delegate = arrayDelegate
        tableView.forceLayout()
        let items = []
        for (let r = 0; r < tableView.rows; ++r) {
            let item = tableView.itemAtCell(Qt.point(0, r))
            items.push(item.index + ":" + item.modelData)
        }
        console.log("array", tableView.rows, tableView.columns, items.join(" "))

        tableView.model = listModel
        tableView.forceLayout()
        let list = []
        for (let r = 0; r < tableView.rows; ++r)
            list.push(typeof tableView.itemAtCell(Qt.point(0, r)).modelData + " " + tableView.itemAtCell(Qt.point(0, r)).modelData.name)
        console.log("list model", tableView.rows, tableView.columns, list.join(" "))
        Qt.quit()
    }
}
