import QtQuick

Item {
    ListView {
        id: view
        width: 200
        height: 200
        model: 3
        delegate: Item {
            width: 80
            height: 20
            property int idx: model.index
        }
    }

    Component.onCompleted: {
        console.log("count", view.count)
        Qt.quit()
    }
}
