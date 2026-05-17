import QtQuick

Item {
    Repeater {
        id: rep
        model: ["A", "B", "C"]
        delegate: Item {
            property int idx: model.index
            property string textValue: modelData
        }
    }

    Component.onCompleted: {
        console.log("count", rep.count)
        console.log("items", rep.itemAt(0).idx, rep.itemAt(0).textValue, rep.itemAt(1).idx, rep.itemAt(1).textValue, rep.itemAt(2).idx, rep.itemAt(2).textValue)
        Qt.quit()
    }
}
