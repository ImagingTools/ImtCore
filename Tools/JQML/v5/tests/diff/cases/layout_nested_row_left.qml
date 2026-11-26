import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

// a RowLayout aligned left in a ColumnLayout wider than it (the launcher of the Tesla dashboard): the row keeps the
// preferred widths of its buttons, the column gives it the width it has
Item {
    width: 1104; height: 445
    ColumnLayout {
        id: col
        spacing: 8
        anchors.fill: parent
        RowLayout {
            id: row
            Layout.leftMargin: 24
            Layout.alignment: Qt.AlignTop | Qt.AlignLeft
            spacing: 24
            Button { id: b1; implicitWidth: 128; implicitHeight: 128; Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter; text: "Front" }
            Button { id: b2; implicitWidth: 128; implicitHeight: 128; Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter; text: "Rear" }
            Button { id: b3; implicitWidth: 128; implicitHeight: 128; Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter; text: "Seat" }
        }
        Rectangle { id: line; Layout.alignment: Qt.AlignTop | Qt.AlignHCenter; width: parent.width - 48; height: 1 }
        RowLayout {
            id: row2
            Layout.leftMargin: 24
            Layout.alignment: Qt.AlignTop | Qt.AlignLeft
            spacing: 24
            Button { id: c1; implicitWidth: 128; implicitHeight: 128; text: "A" }
            Button { id: c2; implicitWidth: 128; implicitHeight: 128; text: "B" }
        }
    }
    Timer { running: true; interval: 50; onTriggered: {
        console.log("row", row.x, row.y, row.width, row.height, row.implicitWidth)
        console.log("buttons", b1.x, b1.width, b2.x, b2.width, b3.x, b3.width)
        console.log("line", line.x, line.y, line.width)
        console.log("row2", row2.x, row2.y, row2.width, c1.x, c1.width, c2.x, c2.width)
        Qt.quit()
    } }
}
