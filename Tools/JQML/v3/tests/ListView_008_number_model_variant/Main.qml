import QtQuick

Item {
    id: root
    property int createdCount: 0

    Timer {
        id: doneTimer
        interval: 1
        repeat: false
        onTriggered: {
            console.log("created", root.createdCount)
            console.log("count", listView.count)
            Qt.quit()
        }
    }

    ListView {
        id: listView
        width: 200
        height: 200
        model: 3

        delegate: Item {
            width: 80
            height: 20

            Component.onCompleted: {
                root.createdCount = root.createdCount + 1
                if (root.createdCount === 3)
                    doneTimer.start()
            }
        }
    }
}
