import QtQuick

Item {
    Row {
        id: row
        spacing: 0

        Item { id: a; width: 20; height: 10 }
        Item { id: b; width: 30; height: 10 }
    }

    Timer {
        id: t
        interval: 1
        repeat: false
        onTriggered: {
            console.log("after", row.width, row.height)
            Qt.quit()
        }
    }

    Component.onCompleted: {
        console.log("before", row.width, row.height)
        row.spacing = 5
        b.visible = false
        t.start()
    }
}
