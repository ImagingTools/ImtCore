import QtQuick

Item {
    Column {
        id: col
        spacing: 2

        Item { id: a; width: 10; height: 20 }
        Item { id: b; width: 50; height: 10 }
    }

    Timer {
        id: t
        interval: 1
        repeat: false
        onTriggered: {
            console.log("after", col.width, col.height)
            Qt.quit()
        }
    }

    Component.onCompleted: {
        console.log("before", col.width, col.height)
        b.width = 5
        a.visible = false
        t.start()
    }
}
