import QtQuick
import QtQuick.Controls
import Qt.labs.qmlmodels

// HeaderView with a model of its own: a JS array, a number, a ListModel; the default textRole and an explicit one;
// the text of each section and the look of the default delegate
Item {
    id: root
    width: 400
    height: 300

    TableModel {
        id: tableModel
        TableModelColumn { display: "a" }
        TableModelColumn { display: "b" }
        TableModelColumn { display: "c" }
        rows: [ { a: 1, b: 2, c: 3 }, { a: 4, b: 5, c: 6 } ]
    }
    ListModel {
        id: listModel
        ListElement { name: "First"; display: "D1" }
        ListElement { name: "Second"; display: "D2" }
        ListElement { name: "Third"; display: "D3" }
    }

    TableView {
        id: tableView
        y: 40
        x: 60
        width: 300
        height: 200
        model: tableModel
        delegate: Rectangle { implicitWidth: 80; implicitHeight: 30 }
    }

    HorizontalHeaderView { id: arrayHeader; syncView: tableView; model: ["Alpha", "Beta", "Gamma"] }
    HorizontalHeaderView { id: arrayRoleHeader; syncView: tableView; textRole: "modelData"; model: ["Alpha", "Beta", "Gamma"] }
    HorizontalHeaderView { id: numberHeader; syncView: tableView; model: 3 }
    HorizontalHeaderView { id: listHeader; syncView: tableView; model: listModel }
    HorizontalHeaderView { id: listRoleHeader; syncView: tableView; model: listModel; textRole: "name" }
    HorizontalHeaderView { id: syncHeader; syncView: tableView }
    VerticalHeaderView { id: verticalArray; syncView: tableView; model: ["R1", "R2"] }
    VerticalHeaderView { id: verticalSync; syncView: tableView }

    // the text of each section and the look of the Basic delegate (the sizes of a text depend on the fonts)
    function texts(view) {
        let out = []
        for (let c = 0; c < 3; c++) {
            for (let r = 0; r < 3; r++) {
                let item = view.itemAtCell(Qt.point(c, r))
                if (!item) continue
                out.push(c + "," + r + "=" + item.contentItem.text)
            }
        }
        return out.join(" | ")
    }
    function show(name, view) {
        let item = view.itemAtCell(Qt.point(0, 0))
        console.log(name, "rows", view.rows, "columns", view.columns, "textRole <" + view.textRole + ">",
                    "look", item.background.color, item.background.border.color, item.background.border.width,
                    item.contentItem.color, item.padding, item.contentItem.horizontalAlignment, item.contentItem.verticalAlignment,
                    ":", texts(view))
    }

    Component.onCompleted: {
        for (let v of [arrayHeader, arrayRoleHeader, numberHeader, listHeader, listRoleHeader, syncHeader, verticalArray, verticalSync])
            v.forceLayout()
        show("array", arrayHeader)
        show("arrayRole", arrayRoleHeader)
        show("number", numberHeader)
        show("list", listHeader)
        show("listRole", listRoleHeader)
        show("sync", syncHeader)
        show("verticalArray", verticalArray)
        show("verticalSync", verticalSync)
        Qt.quit()
    }
}
