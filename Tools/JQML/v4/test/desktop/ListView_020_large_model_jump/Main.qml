import QtQuick

// 100000 uniform rows: the content length and the first visible row must not be found by
// walking every row, and a jump to the far end must still create the right delegates.
Item {
    width: 200
    height: 200

    ListView {
        id: list
        anchors.fill: parent
        model: 100000
        spacing: 4
        clip: true
        reuseItems: true
        delegate: Rectangle {
            width: list.width
            height: 64
            property int row: index
        }
    }

    function check(index) {
        var it = list.itemAtIndex(index)
        var next = list.itemAtIndex(index + 1)
        console.log(index, it ? it.row : -1, it && next ? next.y - it.y : -1)
    }

    Timer {
        interval: 1
        running: true
        onTriggered: {
            console.log("contentHeight", list.contentHeight)
            list.contentY = 50000 * 68
            list.forceLayout()
            check(50000)
            list.contentY = list.contentHeight - list.height
            list.forceLayout()
            console.log("last", list.itemAtIndex(99999) ? list.itemAtIndex(99999).row : -1)
            list.contentY = 0
            list.forceLayout()
            check(0)
            Qt.quit()
        }
    }
}
