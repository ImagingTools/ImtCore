import QtQuick

Rectangle {
    id: rect
    x: 50
    y: 30
    width: 200
    height: 100
    color: "lightblue"

    Component.onCompleted: {
        console.log("x:", x)
        console.log("y:", y)
        console.log("width:", width)
        console.log("height:", height)
    }
}