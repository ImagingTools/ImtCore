import QtQuick

Item {
    width: 400
    height: 300

    Rectangle {
        anchors.fill: parent
        anchors.margins: 20

        Component.onCompleted: {
            console.log("rect", x, y, width, height)
            console.log("expected", parent.width - anchors.margins * 2, parent.height - anchors.margins * 2)
            Qt.quit()
        }
    }
}
