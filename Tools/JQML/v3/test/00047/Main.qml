import QtQuick

Item {
    id: root
    width: 500
    height: 500
    Loader {
        property real itemWidth: item.width
        property real itemHeight: item.height
        onItemWidthChanged: console.log('Loader.onItemWidthChanged', itemWidth)
        onItemHeightChanged: console.log('Loader.onItemHeightChanged', itemHeight)
        onWidthChanged: console.log('Loader.onWidthChanged', width)
        onHeightChanged: console.log('Loader.onHeightChanged', height)
        sourceComponent: Rectangle {
            onWidthChanged: console.log('item.onWidthChanged', width)
            onHeightChanged: console.log('item.onHeightChanged', height)
        }
        onLoaded: {
            item.width = root.width
            item.height = root.height
        }
    }
}