import QtQuick

Item {
    id: root

    Component.onCompleted: {
        let item = Qt.createQmlObject('import QtQuick; Item {
            id: created
            property rect area: Qt.rect(1, 2, 3, 4)
            property alias coordinate: created.area.x
        }', root, "DynamicAlias")
        item.coordinateChanged.connect(function(){ console.log("changed", item.coordinate) })
        item.coordinate = 9
        console.log("area", item.area.x, item.area.y, item.area.width, item.area.height)
        Qt.quit()
    }
}
