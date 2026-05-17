import QtQuick

Item {
    id: root

    property alias label: title.text

    signal leafStep()

    Text {
        id: title
        text: "leaf-default"
    }

    onLeafStep: {
        console.log("slot-leaf", label)
    }
}