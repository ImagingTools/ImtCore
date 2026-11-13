import QtQuick

// onChildrenChanged comes when a child is added or removed, not on every pass: an item with a non-visual child (a Timer) gave a new
// `children` list on every read, so the handler ran again and again, and a handler that restarts a timer kept the page painting.
Item {
    id: root
    width: 100
    height: 100
    property int changes: 0

    Timer {
        id: timer
        interval: 0
        onTriggered: console.log("timer")
    }

    Rectangle {
        id: first
        width: 10
        height: 10
    }

    Component {
        id: extra
        Rectangle {
            width: 10
            height: 10
        }
    }

    onChildrenChanged: {
        root.changes = root.changes + 1
        timer.restart()
    }

    property int settled: 0
    property var added: null

    Timer {
        id: step
        interval: 50
        repeat: true
        running: true
        property int n: 0
        onTriggered: {
            n = n + 1
            if (n === 3) {
                root.settled = root.changes
            }
            if (n === 6) {
                console.log("no change while nothing changes", root.changes === root.settled)
                root.settled = root.changes
                root.added = extra.createObject(root)
            }
            if (n === 9) {
                console.log("one more for an added child", root.changes - root.settled)
                root.settled = root.changes
                root.added.destroy()
            }
            if (n === 12) {
                console.log("one more for a removed child", root.changes - root.settled)
                Qt.quit()
            }
        }
    }
}
