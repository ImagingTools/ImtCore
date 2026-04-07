import QtQuick

Item {
        width: parent.width
        height: parent.height
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

        ListView {
            id: listView
            width: parent.width
            height: parent.height
            model: listModel
            delegate: Item {
                Component.onCompleted: {
                    console.log("ChildItem.onCompleted")
                }
            }
        }
    }