import QtQuick
import QtQml.Models

Item {
    id: root
    width: 400
    height: 100

    property int selectedIndex: 1

    ListModel {
        id: tabModel
    }

    ListView {
        id: view
        width: 300
        height: 40
        spacing: 4
        interactive: false
        orientation: ListView.Horizontal
        model: tabModel
        delegate: Item {
            width: model.index === root.selectedIndex ? 60 : 40
            height: 40
            property string nameValue: model.name
        }
    }

    function closeTab(index){
        tabModel.remove(index)
        root.selectedIndex = 0
    }

    Timer {
        id: doneTimer
        interval: 20
        repeat: false
        onTriggered: {
            console.log("count", view.count, view.contentItem.children.length)
            for (let i = 0; i < view.count; i++){
                let item = view.itemAtIndex(i)
                console.log("item", i, item.nameValue, item.x, item.width)
            }
            Qt.quit()
        }
    }

    Timer {
        id: closeTimer
        interval: 20
        repeat: false
        onTriggered: {
            root.closeTab(1)
            doneTimer.start()
        }
    }

    Component.onCompleted: {
        tabModel.append({ "name": "A" })
        tabModel.append({ "name": "B" })
        tabModel.append({ "name": "C" })
        closeTimer.start()
    }
}
