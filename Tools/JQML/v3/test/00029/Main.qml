import QtQuick

Item {
    width: 400
    height: 300

    Rectangle {
        id: rect
        height: 80
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        Component.onCompleted: {
            console.log("height (по anchors):", height)
            console.log("y (по anchors):", y)
            console.log("parent.height:", parent.height)
            console.log("Проверка: y + height == parent.height?", Math.abs(y + height - parent.height) < 1)
        }
    }
}