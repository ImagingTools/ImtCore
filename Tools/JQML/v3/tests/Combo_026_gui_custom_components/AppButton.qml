import QtQuick

Rectangle {
    id: root

    width: 96
    height: 30
    color: "#3b82f6"

    property alias text: caption.text

    signal clicked()

    function dump(tag) {
        console.log(tag, "button", width, height, text)
    }

    Text {
        id: caption
        anchors.centerIn: parent
        text: "Button"
    }

    MouseArea {
        anchors.fill: parent
        onClicked: root.clicked()
    }
}