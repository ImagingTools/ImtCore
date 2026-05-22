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
            height: 20
            property string nameValue: model.name
        }
    }

    Timer {
        id: doneTimer
        interval: 1
        repeat: false
        onTriggered: {
            console.log("count", view.count)
            Qt.quit()
        }
    }

    Component.onCompleted: {
        lm.append([{ name: "A" }, { name: "B" }, { name: "C" }])
        lm.insert(1, { name: "X" })
        lm.remove(2, 1)
        doneTimer.start()
    }
}
