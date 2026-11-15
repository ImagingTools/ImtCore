import QtQuick

// A view that keeps a list of the items of a Repeater (the commands of a toolbar) learns of them from itemAdded and itemRemoved.
Item {
    id: root
    width: 200
    height: 100

    property var items: []
    property var log: []

    ListModel {
        id: names
        ListElement { name: "a" }
        ListElement { name: "b" }
    }

    Repeater {
        id: repeater
        model: names
        delegate: Item {
            property string label: model.name
        }

        onItemAdded: function(index, item) {
            root.items.push(item)
            root.log.push("added " + index + " " + item.label)
        }

        onItemRemoved: function(index, item) {
            root.items.splice(root.items.indexOf(item), 1)
            root.log.push("removed " + index + " " + item.label)
        }
    }

    Component.onCompleted: {
        console.log("start", root.items.length, root.log.join(","))
        names.append({ name: "c" })
        console.log("append", root.items.length, root.log.join(","))
        names.remove(0)
        console.log("remove", root.items.length, root.log.join(","))
        Qt.quit()
    }
}
