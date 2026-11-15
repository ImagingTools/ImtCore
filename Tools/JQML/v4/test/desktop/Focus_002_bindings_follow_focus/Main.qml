import QtQuick

Item {
    id: root
    width: 200
    height: 100

    FocusScope { id: a; width: 10; height: 10 }
    FocusScope { id: b; width: 10; height: 10 }

    property bool aActive: a.activeFocus
    property bool bActive: b.activeFocus
    property bool aFocus: a.focus
    property bool shown: bActive

    Rectangle {
        id: ring
        width: 10
        height: 10
        visible: b.activeFocus
    }

    function report(tag) {
        console.log(tag, "a", a.activeFocus, root.aActive, a.focus, root.aFocus, "b", b.activeFocus, root.bActive, "ring", ring.visible, "shown", root.shown)
    }

    Timer {
        id: step1
        interval: 50
        running: true
        onTriggered: {
            root.report("start")
            a.forceActiveFocus()
            root.report("a focused")
            b.forceActiveFocus()
            root.report("b focused")
            step2.start()
        }
    }

    Timer {
        id: step2
        interval: 50
        onTriggered: {
            root.report("later")
            a.forceActiveFocus()
            root.report("a again")
            Qt.quit()
        }
    }
}
