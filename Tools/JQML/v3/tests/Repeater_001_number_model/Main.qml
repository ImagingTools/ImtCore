import QtQuick

Item {
    Repeater {
        id: rep
        model: 3
        delegate: Item {
            property int idx: model.index
        }
    }

    Component.onCompleted: {
        console.log("count", rep.count)
        console.log("items", rep.itemAt(0).idx, rep.itemAt(1).idx, rep.itemAt(2).idx)
        Qt.quit()
    }
}
