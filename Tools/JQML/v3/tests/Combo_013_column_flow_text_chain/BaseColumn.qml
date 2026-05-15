import QtQuick

Item {
    id: root

    property int baseSpacing: 3
    property alias headerText: header.text

    Column {
        id: column
        spacing: root.baseSpacing

        Text {
            id: header
            text: "base"
        }
    }

    function columnInfo() {
        return headerText + ":" + root.baseSpacing
    }
}