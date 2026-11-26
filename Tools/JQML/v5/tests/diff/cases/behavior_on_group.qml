import QtQuick

// Behavior on a property of a group (anchors.rightMargin, border.width) and an animation on one
Item {
    width: 200; height: 100
    property bool wide: false
    Rectangle { id: r; anchors.fill: parent; anchors.rightMargin: wide ? 100 : 0; border.width: wide ? 5 : 1
        Behavior on anchors.rightMargin { NumberAnimation { duration: 100 } }
        Behavior on border.width { NumberAnimation { duration: 100 } }
        NumberAnimation on border.color { running: false }
    }
    Timer { id: t1; interval: 50; onTriggered: { console.log("half", r.width > 100 && r.width < 200, r.border.width > 1 && r.border.width < 5); t2.start() } }
    Timer { id: t2; interval: 200; onTriggered: { console.log("end", r.width, r.border.width); Qt.quit() } }
    Component.onCompleted: { console.log("start", r.width, r.border.width); wide = true; t1.start() }
}
