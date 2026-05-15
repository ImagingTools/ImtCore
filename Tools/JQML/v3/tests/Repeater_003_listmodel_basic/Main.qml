import QtQuick
import QtQml.Models

Item {
    ListModel {
        id: lm
    }

    Repeater {
        id: rep
        model: lm
        delegate: Item {
            property string nameValue: model.name
            property int valueValue: model.value
        }
    }

    Component.onCompleted: {
        lm.append([{ name: "A", value: 1 }, { name: "B", value: 2 }])
        console.log("count", rep.count)
        console.log("items", rep.itemAt(0).nameValue, rep.itemAt(0).valueValue, rep.itemAt(1).nameValue, rep.itemAt(1).valueValue)
        Qt.quit()
    }
}
