import QtQuick

Item {
        width: parent.width
        height: parent.height
       Component.onCompleted: {
            console.log("Item.onCompleted")
            Qt.quit()
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
                width: listView.width
                height: 50
                Component.onCompleted: {
                    console.log("ChildItem.onCompleted")
                }
            }
        }
    }
