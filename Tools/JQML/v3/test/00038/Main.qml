import QtQuick

Item {
   Component.onCompleted: {
        console.log("Item.onCompleted")
    }

    ListModel {
        id: listModel

        Component.onCompleted: {
            append({})
            append({})
            append({})
            console.log("ListModel.onCompleted")
        }
    }

    Repeater {
        id: repeater
        model: listModel
        delegate: Item {
            Component.onCompleted: {
                console.log("ChildItem.onCompleted")
            }
        }
    }
}