import QtQuick

BaseColumn {
    id: root

    property alias footerText: footer.text

    Flow {
        id: flow
        spacing: 2

        Text {
            id: footer
            text: "mid"
        }
    }

    function details() {
        return columnInfo() + ":" + footerText + ":" + 2
    }
}