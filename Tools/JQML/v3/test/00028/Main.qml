import QtQuick

Item {
    width: 500
    height: 300

    Rectangle {
        id: rect
        width: 100
        anchors.left: parent.left
        anchors.right: parent.right

        Component.onCompleted: {
            console.log("width (по anchors):", width)
            console.log("x (по anchors):", x)
            console.log("parent.width:", parent.width)
            console.log("Проверка: x + width == parent.width?", Math.abs(x + width - parent.width) < 1)
        }
    }
}