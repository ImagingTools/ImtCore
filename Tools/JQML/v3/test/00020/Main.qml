import QtQuick

Rectangle {
    id: rect
    width: 150
    height: 80

    onWidthChanged: console.log("Событие onWidthChanged:", width)
    onHeightChanged: console.log("Событие onHeightChanged:", height)

    Component.onCompleted: {
        width = 250
        height = 120
        console.log("Финальные размеры:", width, height)
    }
}