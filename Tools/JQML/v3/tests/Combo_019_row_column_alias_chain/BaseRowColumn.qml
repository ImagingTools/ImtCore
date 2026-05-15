import QtQuick

Item {
    id: root

    property int baseCount: 2
    property alias headerText: header.text

    Row {
        id: row

        Column {
            id: column

            Text {
                id: header
                text: "base"
            }
        }
    }

    function baseLabel() {
        return headerText + ":" + baseCount
    }
}