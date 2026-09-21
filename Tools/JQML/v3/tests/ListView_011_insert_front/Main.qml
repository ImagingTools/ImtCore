import QtQuick
import QtQml.Models

Item {
    id: root
    width: 240
    height: 240

    ListModel {
        id: popupModel
    }

    ListView {
        id: view
        width: 200
        height: 200
        spacing: 4
        interactive: false
        model: popupModel
        delegate: Item {
            width: 80
            height: 20
            property string textValue: model.text
        }
    }

    Timer {
        id: doneTimer
        interval: 20
        repeat: false
        onTriggered: {
            console.log("count", view.count)
            console.log("texts", popupModel.get(0).text, popupModel.get(1).text, popupModel.get(2).text)
            Qt.quit()
        }
    }

    Component.onCompleted: {
        popupModel.insert(0, { "text": "first" })
        popupModel.insert(0, { "text": "second" })
        popupModel.insert(0, { "text": "third" })
        doneTimer.start()
    }
}
