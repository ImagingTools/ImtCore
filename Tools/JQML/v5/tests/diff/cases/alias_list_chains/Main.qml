import QtQuick

Item {
    NamedBox {
        id: named
        forwarded: [Item { objectName: "first" }, Item { objectName: "second" }]
    }

    DefaultBox {
        id: implicitChildren
        Item { objectName: "first" }
        Item { objectName: "second" }
    }

    TypedBox {
        id: typed
        Item { objectName: "first" }
        Item { objectName: "second" }
    }

    Component.onCompleted: {
        console.log("named list", named.slot.children.map(function(entry){ return entry.objectName }).join(","))
        console.log("default list", implicitChildren.slot.children.map(function(entry){ return entry.objectName }).join(","))
        console.log("typed list", typed.entries.map(function(entry){ return entry.objectName }).join(","), typed.children.length)
        Qt.quit()
    }
}
