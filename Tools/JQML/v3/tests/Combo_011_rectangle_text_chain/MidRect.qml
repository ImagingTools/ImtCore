import QtQuick

BaseRect {
    id: root

    property int extraWidth: 12
    property alias detailText: detail.text

    Text {
        id: detail
        text: "mid"
    }

    function merged() {
        return describe() + ":" + detailText + ":" + root.extraWidth
    }
}