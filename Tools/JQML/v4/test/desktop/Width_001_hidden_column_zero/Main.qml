import QtQuick

// A cell that is `width: visible ? implicitWidth : 0` and is hidden has the width 0, and what is anchored to its right edge starts at
// its left edge (a hidden column of a table). The same when the width is written by a script.
Item {
    id: root
    width: 400
    height: 300

    Item {
        id: cell
        visible: false
        implicitWidth: 50
        width: visible ? implicitWidth : 0
        height: 20

        Rectangle {
            width: 50
            height: 20
            color: "red"
        }
    }

    Rectangle {
        id: next
        anchors.left: cell.right
        width: 10
        height: 10
        color: "blue"
    }

    Item {
        id: written
        height: 20
        y: 100

        Rectangle {
            width: 40
            height: 20
            color: "green"
        }
    }

    Item {
        id: after
        anchors.left: written.right
        y: 100
        width: 10
        height: 10
    }

    Component.onCompleted: {
        written.width = 0
        console.log("cell", cell.width, "next", next.x)
        console.log("written", written.width, "after", after.x)
        Qt.quit()
    }
}
