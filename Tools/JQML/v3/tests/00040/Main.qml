import QtQuick

Item {
    width: 500
    height: 500
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
        width: parent.width
        height: parent.height
        id: listView
        model: 3
        delegate: Item {
            width: listView.width
            height: 50
            Component.onCompleted: {
                console.log("ChildItem.onCompleted")
            }
        }
    }
}
