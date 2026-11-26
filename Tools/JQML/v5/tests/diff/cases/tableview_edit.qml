import QtQuick
import QtTest
import Qt.labs.qmlmodels

// TableView editing (QQuickTableView::edit, closeEditor and the key filter of the editor): Return / Enter open the
// editor with EditKeyPressed, any key with AnyKeyPressed; Return and Enter commit, Escape cancels, Tab and Backtab
// commit and move on; the delegate's editing, the editor's focus, TableView.commit; a tap elsewhere commits; edit()
// from code, on a cell out of view, with no editDelegate; the edit triggers none
Item {
    id: root
    width: 400
    height: 300

    property var log: []

    TableModel {
        id: tableModel
        TableModelColumn { display: "a"; edit: "a" }
        TableModelColumn { display: "b"; edit: "b" }
        TableModelColumn { display: "c" }
        rows: {
            let rows = []
            for (let i = 0; i < 30; i++)
                rows.push({ a: "a" + i, b: "b" + i, c: "c" + i })
            return rows
        }
    }

    Component {
        id: plainDelegate
        Rectangle {
            required property bool editing
            implicitWidth: 80
            implicitHeight: 30
        }
    }

    TableView {
        id: tableView
        width: 260
        height: 160
        clip: true
        model: tableModel
        selectionModel: ItemSelectionModel {}
        delegate: Rectangle {
            id: cell
            required property bool editing
            required property bool current
            required property int row
            required property int column
            required property var model
            implicitWidth: 80
            implicitHeight: 30
            onEditingChanged: root.log.push("editing " + row + "," + column + " " + editing)
            TableView.editDelegate: TextInput {
                id: input
                width: parent.width
                height: parent.height
                text: "edited " + cell.row
                Component.onCompleted: root.log.push("editor " + cell.row + "," + cell.column)
                Component.onDestruction: root.log.push("editor gone")
                onActiveFocusChanged: if (activeFocus) root.log.push("editor focus")
                TableView.onCommit: {
                    root.log.push("commit " + cell.row + "," + cell.column)
                    cell.model.edit = input.text
                }
            }
        }
    }

    function show(label) {
        let i = tableView.selectionModel.currentIndex
        console.log(label, "| current", i.row + "," + i.column, "| data", tableModel.rows[1].a, tableModel.rows[1].b, tableModel.rows[2].a,
                    "| table focus", tableView.activeFocus, "|", root.log.join(", "))
        root.log = []
    }

    TestCase {
        name: "TableViewEdit"
        when: windowShown

        function init() {
            tableView.closeEditor()
            tableView.editTriggers = TableView.EditKeyPressed
            tableView.delegate = tableView.delegate
            tableView.positionViewAtCell(Qt.point(0, 0), TableView.AlignLeft | TableView.AlignTop)
            tableView.selectionModel.setCurrentIndex(tableView.index(1, 0), ItemSelectionModel.NoUpdate)
            tableView.forceActiveFocus()
            root.log = []
        }

        function test_1_returnKey() {
            keyClick(Qt.Key_Return)
            show("return opens")
            keyClick(Qt.Key_Return)
            show("return commits")
            keyClick(Qt.Key_Down)
            show("down")
            keyClick(Qt.Key_Enter)
            show("enter opens")
            keyClick(Qt.Key_Enter)
            show("enter commits")
        }

        function test_2_escape() {
            keyClick(Qt.Key_Return)
            keyClick(Qt.Key_Escape)
            show("escape")
        }

        function test_3_tab() {
            keyClick(Qt.Key_Return)
            keyClick(Qt.Key_Tab)
            show("tab")
            keyClick(Qt.Key_Backtab)
            show("backtab")
            tableView.closeEditor()
            show("closed")
        }

        function test_4_triggers() {
            tableView.editTriggers = TableView.NoEditTriggers
            keyClick(Qt.Key_Return)
            show("no triggers")
            tableView.editTriggers = TableView.AnyKeyPressed
            keyClick(Qt.Key_X)
            show("any key")
            tableView.closeEditor()
            keyClick(Qt.Key_Down)
            show("arrow with any key")
        }

        function test_5_code() {
            tableView.edit(tableView.index(2, 1))
            show("edit from code")
            tableView.edit(tableView.index(2, 0))
            show("edit another")
            tableView.edit(tableView.index(25, 0))
            show("edit out of view")
            tableView.closeEditor()
            tableView.edit(tableView.index(1, 2))
            show("column with no edit role")
            tableView.closeEditor()
            show("closed")
        }

        function test_6_tapCommits() {
            tableView.edit(tableView.index(1, 1))
            mouseClick(tableView, 200, 130)
            show("tap elsewhere")
        }

        function test_7_noEditDelegate() {
            tableView.delegate = plainDelegate
            tableView.forceLayout()
            tableView.edit(tableView.index(1, 0))
            show("no edit delegate")
        }
    }
}
