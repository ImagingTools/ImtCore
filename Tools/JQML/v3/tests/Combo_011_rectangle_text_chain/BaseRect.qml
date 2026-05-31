import QtQuick

Item {
    id: root

    property int baseWidth: 40
    property alias titleText: title.text

    signal basePing(string message)

    Rectangle {
        id: frame
        width: root.baseWidth
        height: 20
        color: "steelblue"
    }

    Text {
        id: title
        text: "base"
    }

    function describe() {
        return titleText + ":" + root.baseWidth
    }
}