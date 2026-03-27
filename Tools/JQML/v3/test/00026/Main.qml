import QtQuick

Item {
    width: 400; height: 300

    Rectangle {
        id: rect
        anchors.left: parent.left; anchors.right: parent.right; anchors.top: parent.top; anchors.bottom: parent.bottom;
        anchors.margins: 20

        Component.onCompleted: {
            console.log("x с учетом margins:", x)
            console.log("y с учетом margins:", y)
            console.log("width с учетом margins:", width)
            console.log("height с учетом margins:", height)
            console.log("Ожидаемая ширина:", parent.width - anchors.margins * 2)
            console.log("Ожидаемая высота:", parent.height - anchors.margins * 2)
        }
    }
}