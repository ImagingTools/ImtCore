import QtQuick
import QtQuick.Controls

// SplitView narrower than its items: the items keep their implicit size, the fill item takes what is left down to its
// minimum, the content goes past the end (the gallery's SplitView page)
Item {
    width: 360; height: 200
    SplitView {
        id: sv
        width: 360; height: 200
        Rectangle { id: a; implicitWidth: 200; implicitHeight: 100; SplitView.maximumWidth: 400 }
        Rectangle { id: b; SplitView.minimumWidth: 50; SplitView.fillWidth: true }
        Rectangle { id: c; implicitWidth: 200; implicitHeight: 100 }
    }
    Component.onCompleted: { console.log(a.x, a.width, b.x, b.width, c.x, c.width, a.height); Qt.quit() }
}
