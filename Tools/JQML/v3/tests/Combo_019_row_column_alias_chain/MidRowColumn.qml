import QtQuick

BaseRowColumn {
    id: root

    property alias detailText: detail.text

    Text {
        id: detail
        text: "mid"
    }

    function summary() {
        return baseLabel() + ":" + detailText
    }
}