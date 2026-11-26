import QtQuick
import QtQuick.Layouts

// a RowLayout with no width (the implicit one) whose text gets wider: the layout grows, the text is not elided
// (ButtonImpl of QmlAppTemplate: the bold label of the checked menu)
Item {
    width: 300; height: 100
    property bool bold: false
    property real w0
    property real r0
    RowLayout {
        id: row
        spacing: 6
        Rectangle { implicitWidth: 20; implicitHeight: 20 }
        Text { id: label; text: "Desktop components"; font.pixelSize: 14; font.bold: bold; elide: Text.ElideMiddle }
    }
    Timer { id: t1; interval: 50; onTriggered: {
        w0 = label.width; r0 = row.width
        bold = true
        t2.start()
    } }
    Timer { id: t2; interval: 50; onTriggered: {
        console.log(label.width === Math.ceil(label.implicitWidth), row.width === row.implicitWidth, label.truncated)
        label.text = "Desktop components and more"
        t3.start()
    } }
    Timer { id: t3; interval: 50; onTriggered: {
        console.log(label.width === Math.ceil(label.implicitWidth), row.width === row.implicitWidth, row.width > r0, label.truncated)
        Qt.quit()
    } }
    Component.onCompleted: t1.start()
}
