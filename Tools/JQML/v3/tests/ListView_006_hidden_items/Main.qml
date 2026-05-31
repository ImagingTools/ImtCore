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
            width: 80
            height: model.visible ? 20 : 0
            visible: model.visible
            property string nameValue: model.name
        }
    }

    Timer {
        id: doneTimer
        interval: 1
        repeat: false
        onTriggered: {
            // ListView should count all items including invisible ones
            console.log("count", view.count)
            Qt.quit()
        }
    }

    Component.onCompleted: {
        lm.append([
            { name: "A", visible: true },
            { name: "B", visible: false },
            { name: "C", visible: true }
        ])
        doneTimer.start()
    }
}
