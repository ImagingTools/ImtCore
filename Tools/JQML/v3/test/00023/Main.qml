import QtQuick

Item {
    width: 600
    height: 400

    Rectangle {
        id: rect
        width: 100
        height: 60
        color: "green"
        anchors.centerIn: parent

        Component.onCompleted: {
            console.log("x после centerIn:", x)
            console.log("y после centerIn:", y)
            console.log("Ожидаемый x:", (parent.width - width) / 2)
            console.log("Ожидаемый y:", (parent.height - height) / 2)
            console.log("Совпадение?", Math.abs(x - (parent.width - width) / 2) < 1 && Math.abs(y - (parent.height - height) / 2) < 1)
        }
    }
}