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
            property int scoreValue: model.score
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
        lm.append([{ name: "A", score: 10 }, { name: "B", score: 20 }])
        doneTimer.start()
    }
}
