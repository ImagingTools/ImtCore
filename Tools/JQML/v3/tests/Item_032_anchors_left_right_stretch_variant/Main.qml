import QtQuick

Item {
    width: 500
    height: 300

    Rectangle {
        width: 100
        anchors.left: parent.left
        anchors.right: parent.right

        Component.onCompleted: {
            console.log("rect", x, width)
            console.log("parent", parent.width)
            Qt.quit()
        }
    }
}
