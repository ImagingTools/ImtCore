import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

// A wrapping Label with Layout.fillWidth in a GridLayout and a ColumnLayout narrower than its text: the layout gives it its
// own width (the minimum of an item is 0), the text wraps; the other cells of the grid
Item {
    width: 360; height: 400
    GridLayout {
        id: grid
        width: 360; height: 200
        Label { id: l1; wrapMode: Label.Wrap; Layout.fillWidth: true; Layout.columnSpan: 2
            text: "TableView provides a scrollable grid that displays data from a model in rows and columns, allowing users to view and interact" }
        Rectangle { id: a; Layout.column: 1; Layout.row: 1; Layout.fillWidth: true; implicitHeight: 20 }
        Rectangle { id: b; Layout.column: 0; Layout.row: 2; implicitWidth: 30; Layout.fillHeight: true }
        Rectangle { id: c; Layout.column: 1; Layout.row: 2; Layout.fillWidth: true; Layout.fillHeight: true }
    }
    ColumnLayout {
        y: 220; width: 200
        Label { id: l2; wrapMode: Label.Wrap; Layout.fillWidth: true; text: "A long line of text in a column layout that has to wrap" }
    }
    Component.onCompleted: {
        console.log("grid", l1.width, l1.lineCount > 1, a.x, a.width, b.x, b.width, c.x, c.width, c.height > 50)
        console.log("column", l2.width, l2.lineCount > 1)
        Qt.quit()
    }
}
