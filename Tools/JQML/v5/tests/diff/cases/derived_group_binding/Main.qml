import QtQuick
import QtQuick.Layouts

Item {
    width: 200; height: 100
    property bool checked: true
    Base { id: b; font.bold: checked }
    RowLayout {
        id: row
        width: 200
        layoutDirection: Qt.RightToLeft
        spacing: 10
        Rectangle { id: r1; implicitWidth: 30; implicitHeight: 10 }
        Rectangle { id: r2; implicitWidth: 50; implicitHeight: 10 }
    }
    Row {
        id: plain
        layoutDirection: Qt.RightToLeft
        spacing: 5
        Rectangle { id: p1; width: 30; height: 10 }
        Rectangle { id: p2; width: 50; height: 10 }
    }
    Component.onCompleted: {
        console.log(b.font.bold, b.font.pixelSize)
        checked = false
        console.log(b.font.bold)
        checked = true
        console.log(b.font.bold, row.layoutDirection, r1.x, r2.x, plain.width, p1.x, p2.x)
        Qt.quit()
    }
}
