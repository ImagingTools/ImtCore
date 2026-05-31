import QtQuick

BaseAlias {
    id: root

    property int midValue: 2
    property alias detailText: detail.text

    signal midSignal()

    Text {
        id: detail
        text: "mid"
    }

    function summary() {
        return baseLabel() + ":" + detailText + ":" + midValue
    }
}