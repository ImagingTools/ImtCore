import QtQuick
import QtQuick.Layouts

// the sizes of the items of a layout are whole pixels, up (QGridLayoutEngine snapped to the pixel grid)
Item {
    width: 300; height: 100
    RowLayout { id: a; spacing: 6
        Rectangle { id: a1; implicitWidth: 30.3; implicitHeight: 10.2 }
        Rectangle { id: a2; implicitWidth: 50.6; implicitHeight: 10 } }
    RowLayout { id: b; width: 200; spacing: 6
        Rectangle { id: b1; implicitWidth: 30.3; implicitHeight: 10 }
        Rectangle { id: b2; implicitWidth: 50.6; implicitHeight: 10; Layout.fillWidth: true }
        Rectangle { id: b3; implicitWidth: 20.2; implicitHeight: 10; Layout.minimumWidth: 10.5 } }
    Component.onCompleted: { console.log(a.width, a.implicitWidth, a1.x, a1.width, a1.height, a2.x, a2.width, "|", b1.x, b1.width, b2.x, b2.width, b3.x, b3.width); Qt.quit() }
}
