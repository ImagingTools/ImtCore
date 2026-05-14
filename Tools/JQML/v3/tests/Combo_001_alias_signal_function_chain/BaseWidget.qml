import QtQuick

Item {
    id: root

    property int baseValue: 2
    property alias labelText: label.text

    signal pinged(string text)

    function makeMessage(prefix) {
        return prefix + ":" + labelText
    }

    Text {
        id: label
        text: "base"
    }

    Component.onCompleted: {
        console.log("base", baseValue, labelText)
    }
}
