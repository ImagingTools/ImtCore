import QtQuick

Item {
    width: 500
    height: 500

    Text {
        anchors.left: parent.left
        anchors.right: parent.right
        text: "Hello"

        Component.onCompleted: {
            console.log("text width", width)
            Qt.quit()
        }
    }
}
