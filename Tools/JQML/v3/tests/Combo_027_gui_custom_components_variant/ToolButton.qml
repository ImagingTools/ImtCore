import QtQuick

Rectangle {
    id: root

    width: 84
    height: 26
    color: "#2563eb"

    property alias text: caption.text

    signal triggered()

    function dump(tag) {
        console.log(tag, "toolButton", width, height, text)
    }

    Text {
        id: caption
        anchors.centerIn: parent
        text: "Tool"
    }

    MouseArea {
        anchors.fill: parent
        onClicked: root.triggered()
    }
}