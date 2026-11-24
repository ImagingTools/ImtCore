import QtQuick
import Qt.labs.qmlmodels

// TableModel.setData with EditRole: a column without an edit role refuses it; a column whose edit and display roles are
// the same property changes both, the roles dataChanged gives and a delegate showing display (the editor of
// TableViewDelegate commits with EditRole)
Item {
    width: 300
    height: 200
    TableModel { id: a; TableModelColumn { display: "name" } rows: [ { name: "x" } ] }
    TableModel { id: b; TableModelColumn { display: "name"; edit: "name" } rows: [ { name: "x" } ] }
    TableView {
        id: view
        anchors.fill: parent
        model: b
        delegate: Text { required property var display; text: display; property var edited: model.edit }
    }
    Connections {
        target: b
        function onDataChanged(topLeft, bottomRight, roles) { console.log("dataChanged", topLeft.row, topLeft.column, "roles", JSON.stringify(roles)) }
    }
    Component.onCompleted: {
        view.forceLayout()
        console.log("display only:", a.setData(a.index(0, 0), "y", Qt.EditRole), a.data(a.index(0, 0), "display"))
        console.log("with edit:", b.setData(b.index(0, 0), "y", Qt.EditRole), b.data(b.index(0, 0), "display"), b.data(b.index(0, 0), "edit"))
        let item = view.itemAtCell(Qt.point(0, 0))
        console.log("delegate:", item.text, item.edited)
        console.log("display:", b.setData(b.index(0, 0), "z", Qt.DisplayRole))
        console.log("delegate:", item.text, item.edited)
        Qt.quit()
    }
}
