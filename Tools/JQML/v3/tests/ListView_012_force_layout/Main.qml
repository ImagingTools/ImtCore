import QtQuick
import QtQml.Models

Item {
    id: root
    width: 240
    height: 240

    ListModel {
        id: listModel
    }

    ListView {
        id: view
        width: 200
        height: 100
        spacing: 4
        interactive: false
        model: listModel
        delegate: Item {
            width: 80
            height: 20
            property string textValue: model.text
        }
    }

    function itemText(index) {
        let item = view.itemAtIndex(index)
        return item ? item.textValue : "none"
    }

    function itemY(index) {
        let item = view.itemAtIndex(index)
        return item ? item.y : -1
    }

    // Qt: forceLayout() applies the pending model changes at once, before the next frame
    Timer {
        id: startTimer
        interval: 20
        onTriggered: {
            for (let i = 0; i < 10; i++) {
                listModel.append({ "text": "item" + i })
            }
            view.forceLayout()
            console.log("append", view.count, root.itemText(0), root.itemY(0), root.itemText(1), root.itemY(1))

            listModel.insert(0, { "text": "front" })
            view.forceLayout()
            console.log("insert", view.count, root.itemText(0), root.itemText(1), root.itemY(1))

            listModel.remove(0)
            view.forceLayout()
            console.log("remove", view.count, root.itemText(0), root.itemY(0))

            view.positionViewAtIndex(9, ListView.End)
            console.log("position", view.contentY)
            Qt.quit()
        }
    }

    Component.onCompleted: startTimer.start()
}
