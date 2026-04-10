import QtQuick

Item {
    width: 500
    height: 500

    Loader {
        width: parent.width
        height: parent.height

        onWidthChanged: console.log('Loader.onWidthChanged', width)
        onHeightChanged: console.log('Loader.onHeightChanged', height)

        sourceComponent: Rectangle {
            onWidthChanged: console.log('item.onWidthChanged', width)
            onHeightChanged: console.log('item.onHeightChanged', height)
        }
    }
}