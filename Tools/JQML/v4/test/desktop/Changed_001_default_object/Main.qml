import QtQuick

// `property Item items: Item {}` assigns the default during construction after onItemsChanged
// was registered. Qt does not emit itemsChanged for that default; firing it would clear model.
Item {
    id: root
    property int hits: 0

    Item {
        id: holder
        property Item items: Item {
            objectName: "defaultItems"
        }
        onItemsChanged: {
            root.hits += 1
            view.model = 0
        }
    }

    ListView {
        id: view
        width: 80
        height: 80
        model: 5
        delegate: Item {
            width: 10
            height: 10
        }
    }

    Component.onCompleted: {
        console.log("hits", root.hits)
        console.log("count", view.count)
        console.log("name", holder.items.objectName)
        Qt.quit()
    }
}
