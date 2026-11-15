import QtQuick
import QtQml.Models

// Qt: every object of the created tree is complete before the first Component.onCompleted runs;
// the handlers keep their order, and an object created inside a handler completes at once
Item {
    id: root
    width: 240
    height: 240

    property var log: []

    function add(name) {
        let list = root.log
        list.push(name)
        root.log = list
    }

    Component.onCompleted: {
        root.add("root")
        console.log("views", view.count, view.itemAtIndex(0) !== null, view.itemAtIndex(0) ? view.itemAtIndex(0).y : -1, view.itemAtIndex(1) ? view.itemAtIndex(1).y : -1)
        console.log("repeater", repeater.count, column.children.length, column.height)
        console.log("loader", loader.status === Loader.Ready, loader.item ? loader.item.objectName : "none")
        let created = dynamicComponent.createObject(root)
        console.log("created", created.objectName, created.completedInside)
        doneTimer.start()
    }

    Item {
        id: a
        Component.onCompleted: root.add("a")

        Item {
            Component.onCompleted: root.add("a1")
        }
        Item {
            Component.onCompleted: root.add("a2")
        }
    }

    ListModel {
        id: listModel

        ListElement { name: "first" }
        ListElement { name: "second" }
        ListElement { name: "third" }
    }

    ListView {
        id: view
        width: 100
        height: 100
        model: listModel
        delegate: Item {
            width: 100
            height: 20
        }
        Component.onCompleted: root.add("view")
    }

    Column {
        id: column
        x: 120

        Repeater {
            id: repeater
            model: 3
            delegate: Item {
                width: 20
                height: 15
            }
        }
    }

    Loader {
        id: loader
        sourceComponent: Item {
            objectName: "loaded"
            Component.onCompleted: root.add("loaded")
        }
    }

    Component {
        id: dynamicComponent

        Item {
            objectName: "dynamic"
            property bool completedInside: false
            Component.onCompleted: {
                completedInside = true
                root.add("dynamic")
            }
        }
    }

    QtObject {
        Component.onCompleted: root.add("q")
    }

    Timer {
        id: doneTimer
        interval: 10
        onTriggered: {
            console.log("order", root.log.join(" "))
            Qt.quit()
        }
    }
}
