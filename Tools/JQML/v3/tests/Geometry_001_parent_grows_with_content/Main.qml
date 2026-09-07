import QtQuick

// Mirrors Table: parent grows with contentHeight, list is stretched by
// anchors, and a 10px bar steals the opposite axis when it becomes visible.
// Adding rows must not leave width at 990 with a stuck scrollbar.
Item {
    id: root
    width: 1000
    height: headerHeight + list.contentHeight

    property int headerHeight: 40
    property int rowCount: 0

    Rectangle {
        id: vbar
        width: 10
        anchors.right: parent.right
        anchors.top: list.top
        anchors.bottom: list.bottom
        visible: list.contentHeight > list.height
        color: "red"
    }

    Rectangle {
        id: hbar
        height: 10
        anchors.left: list.left
        anchors.right: list.right
        anchors.top: list.bottom
        anchors.topMargin: 1
        visible: list.contentWidth - 3 > list.width
        color: "red"
    }

    ListView {
        id: list
        anchors.left: parent.left
        anchors.right: vbar.visible ? vbar.left : parent.right
        anchors.top: parent.top
        anchors.topMargin: root.headerHeight
        anchors.bottom: parent.bottom
        anchors.bottomMargin: hbar.visible ? hbar.height + hbar.anchors.topMargin : 0
        model: root.rowCount
        spacing: 0
        clip: true
        delegate: Rectangle {
            width: 1000
            height: 30
            color: "blue"
        }
    }

    Timer {
        interval: 20
        running: true
        repeat: false
        onTriggered: {
            root.rowCount = 4
        }
    }

    Timer {
        interval: 80
        running: true
        repeat: false
        onTriggered: {
            console.log("fits", list.contentHeight <= list.height && Math.round(list.width) === 1000)
            console.log("width", Math.round(list.width))
            console.log("vbar", vbar.visible)
            console.log("hbar", hbar.visible)
            Qt.quit()
        }
    }
}
