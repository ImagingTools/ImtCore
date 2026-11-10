import QtQuick

// A binding that reads some dependencies and then throws (a null target) is given up after
// repeated failures. Dropping its subscriptions while the evaluation is still being tracked
// must not break the frame; the property keeps its last good value, as in Qt.
Item {
    id: root

    property var target: probe
    property real value: 1
    property real derived: root.value + root.target.scaleCoeff

    Item {
        id: probe
        property real scaleCoeff: 2
    }

    Timer {
        interval: 1
        running: true
        onTriggered: {
            console.log("before", root.derived)
            root.target = null
            for (var i = 0; i < 40; i++) root.value = 10 + i
            console.log("after", root.derived)
            root.target = probe
            console.log("alive", root.value)
            Qt.quit()
        }
    }
}
