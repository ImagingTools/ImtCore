import QtQuick
import QtQml.Models

Item {
    ListModel {
        id: lm
    }

    ListView {
        id: view
        width: 200
        height: 200
        model: lm
        delegate: Item {
            width: model.size > 0 ? 80 : 0
            height: model.size > 0 ? 20 : 0
            property string nameValue: model.name
        }
    }

    Timer {
        id: doneTimer
        interval: 1
        repeat: false
        onTriggered: {
            // ListView should count all items including zero-size ones
            console.log("count", view.count)
            Qt.quit()
        }
    }

    Component.onCompleted: {
        lm.append([
            { name: "A", size: 1 },
            { name: "B", size: 0 },
            { name: "C", size: 1 }
        ])
        doneTimer.start()
    }
}
