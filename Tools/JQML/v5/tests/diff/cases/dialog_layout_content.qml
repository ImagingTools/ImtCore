import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtTest

// A modal Dialog whose contentItem is a ColumnLayout, with standard buttons (the gallery's settings dialog): its size and
// the place of its footer; Escape closes it and does not reach a Shortcut of the window behind it
Item {
    width: 360; height: 520
    property var log: []
    Shortcut { sequences: ["Esc"]; onActivated: log.push("shortcut") }
    Dialog {
        id: d
        x: 20; y: 80
        modal: true
        focus: true
        title: "Settings"
        standardButtons: Dialog.Ok | Dialog.Cancel
        onRejected: log.push("rejected")
        contentItem: ColumnLayout {
            spacing: 20
            RowLayout { spacing: 10; Rectangle { implicitWidth: 40; implicitHeight: 20 } Rectangle { implicitWidth: 150; implicitHeight: 40; Layout.fillWidth: true } }
            Rectangle { implicitWidth: 100; implicitHeight: 40 }
            Rectangle { implicitWidth: 100; implicitHeight: 40 }
        }
    }
    TestCase { name: "DialogContent"; when: windowShown
        function test_a() {
            d.open(); wait(300)
            // (the title and the buttons depend on the font: their sizes are not compared)
            console.log("dialog", d.contentItem.height, d.footer.y - d.contentItem.y - d.contentItem.height, d.height - d.footer.y - d.footer.height, d.contentItem.y - d.header.height, d.footer.visible)
            keyClick(Qt.Key_Escape); wait(300)
            console.log("escape", d.visible, log.join(","))
            keyClick(Qt.Key_Escape)
            console.log("again", log.join(","))
        }
    }
}
