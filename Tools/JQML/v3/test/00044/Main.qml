import QtQuick

Item {
    id: root
    width: 500
    height: 500

    Loader {
        onWidthChanged: console.log('Loader.onWidthChanged', width)
        onHeightChanged: console.log('Loader.onHeightChanged', height)

        sourceComponent: Rectangle {
            width: root.width
            height: root.height

            onWidthChanged: console.log('item.onWidthChanged', width)
            onHeightChanged: console.log('item.onHeightChanged', height)
        }
    }
}