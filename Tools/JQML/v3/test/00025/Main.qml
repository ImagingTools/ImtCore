import QtQuick

Item {
    width: 400; height: 300

    Rectangle {
        id: rect
        color: "teal"
        anchors.left: parent.left; anchors.right: parent.right // Ширина определяется якорями

        Component.onCompleted: {
            console.log("Ширина rect по anchors:", rect.width)
            console.log("Ширина родителя:", parent.width)
            console.log("Совпадение?", Math.abs(rect.width - parent.width) < 1)
        }
    }
}