import QtQuick
import QtQuick.Layouts

// GridLayout auto placement with an item that sets only Layout.column, or only Layout.row (the gallery's MonthGrid page)
Item {
    width: 300; height: 300
    GridLayout {
        id: g
        columns: 2
        Rectangle { id: a; Layout.column: 1; implicitWidth: 100; implicitHeight: 20 }
        Rectangle { id: b; implicitWidth: 30; implicitHeight: 100 }
        Rectangle { id: c; implicitWidth: 100; implicitHeight: 100 }
    }
    GridLayout {
        y: 150
        columns: 3
        Rectangle { id: d; implicitWidth: 20; implicitHeight: 20 }
        Rectangle { id: e; Layout.row: 1; implicitWidth: 20; implicitHeight: 20 }
        Rectangle { id: f; implicitWidth: 20; implicitHeight: 20 }
    }
    Component.onCompleted: {
        console.log(a.x, a.y, b.x, b.y, c.x, c.y)
        console.log(d.x, d.y, e.x, e.y, f.x, f.y)
        Qt.quit()
    }
}
