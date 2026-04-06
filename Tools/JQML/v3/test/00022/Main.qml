import QtQuick

Item {
    id: root
    width: 500
    height: 400

    Rectangle {
        id: rect
        x: 100
        y: 50

        onXChanged: console.log("onXChanged (свойство):", x)
        onYChanged: console.log("onYChanged (свойство):", y)

        Component.onCompleted: {
            console.log("--- Начало теста ---")
            anchors.left = parent.left
            anchors.top = parent.top
            console.log("--- Конец теста ---")
        }
    }
}