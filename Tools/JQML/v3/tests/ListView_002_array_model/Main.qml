import QtQuick

Item {
    ListView {
        id: view
        width: 200
        height: 200
        model: ["A", "B", "C"]
        delegate: Item {
            width: 80
            height: 20
            property int idx: model.index
            property string value: modelData
        }
    }

    Component.onCompleted: {
        console.log("count", view.count)
        Qt.quit()
    }
}
