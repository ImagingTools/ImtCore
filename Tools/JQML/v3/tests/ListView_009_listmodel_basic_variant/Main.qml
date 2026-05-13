import QtQuick
import QtQml.Models

Item {
    id: root
    property int createdCount: 0

    ListModel {
        id: listModel
    }

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
        model: listModel

        delegate: Item {
            width: listView.width
            height: 50

            Component.onCompleted: {
                root.createdCount = root.createdCount + 1
                if (root.createdCount === 3)
                    doneTimer.start()
            }
        }
    }

    Component.onCompleted: {
        listModel.append([{}, {}, {}])
    }
}
