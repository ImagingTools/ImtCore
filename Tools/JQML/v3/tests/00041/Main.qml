import QtQuick

Item {
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
        model: 3
        delegate: Item {
            Component.onCompleted: {
                console.log("ChildItem.onCompleted")
            }
        }
    }
}
