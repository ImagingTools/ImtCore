import QtQuick

Item {
    id: root

    property int baseValue: 1
    property alias titleText: title.text

    signal baseSignal()

    Text {
        id: title
        text: "base"
    }

    function baseLabel() {
        return titleText + ":" + baseValue
    }
}