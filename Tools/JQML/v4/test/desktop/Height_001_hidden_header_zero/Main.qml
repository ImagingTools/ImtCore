import QtQuick

// A header that is `height: visible ? implicitHeight : 0` and is hidden has the height 0, and what is anchored to its bottom starts at
// its top (SimpleCollectionTableHeader of IotPlatform: the column titles of an empty table were 50 px too low). The same when the height
// is written by a script.
Item {
    id: root
    width: 400
    height: 300

    Item {
        id: header
        visible: false
        implicitHeight: 50
        height: visible ? implicitHeight : 0
        width: 100

        Rectangle {
            width: 100
            height: 50
            color: "red"
        }
    }

    Rectangle {
        id: below
        anchors.top: header.bottom
        width: 100
        height: 10
        color: "blue"
    }

    Item {
        id: written
        width: 100
        y: 100
        Rectangle {
            width: 100
            height: 40
            color: "green"
        }
    }

    Item {
        id: after
        anchors.top: written.bottom
        width: 100
        height: 10
    }

    Component.onCompleted: {
        written.height = 0
        console.log("header", header.height, "below", below.y)
        console.log("written", written.height, "after", after.y)
        Qt.quit()
    }
}
