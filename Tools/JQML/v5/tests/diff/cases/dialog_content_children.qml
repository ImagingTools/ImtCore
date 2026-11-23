import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

// A Dialog whose contentItem is a ColumnLayout: the children of the layout are its own (no item of the dialog goes in)
Item {
    width: 360; height: 520
    Dialog {
        id: d
        title: "T"
        standardButtons: Dialog.Ok
        contentItem: ColumnLayout {
            id: col
            Rectangle { implicitWidth: 10; implicitHeight: 10 }
            Rectangle { implicitWidth: 10; implicitHeight: 10; Layout.fillHeight: true }
        }
    }
    Component.onCompleted: { d.open(); later.start() }
    Timer { id: later; interval: 300; onTriggered: { console.log(col.children.length, col.implicitHeight, d.contentItem === col); Qt.quit() } }
}
