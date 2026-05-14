import QtQuick

Rectangle {
    id: root

    width: 180
    height: 96
    color: "#111827"
    visible: false

    property alias title: titleText.text

    function dump(tag) {
        console.log(tag, "popup", x, y, width, height, visible, title)
    }

    Text {
        id: titleText
        x: 8
        y: 6
        text: "Popup"
    }

    Loader {
        id: bodyLoader
        x: 8
        y: 26
        width: 164
        height: 62
        sourceComponent: Rectangle {
            color: "#374151"
            width: 164
            height: 62

            Text {
                anchors.centerIn: parent
                text: "Popup Body"
            }
        }
    }
}