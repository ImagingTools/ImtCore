import QtQuick

// mapToItem(null) / mapFromItem(null) use the document root origin (Qt window content item).
Item {
    id: root
    width: 200
    height: 100

    Item {
        id: child
        x: 30
        y: 40
        width: 10
        height: 10
    }

    Component.onCompleted: {
        var to = child.mapToItem(null, 5, 7)
        var from = child.mapFromItem(null, to.x, to.y)
        console.log("map", to.x, to.y, from.x, from.y)
        Qt.quit()
    }
}
