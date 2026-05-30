import QtQuick

Item {
    id: root

    property int baseCount: 3
    property alias headerText: header.text

    ListView {
        id: view
        model: root.baseCount

        delegate: Item {
            property int itemIndex: index + 1
        }
    }

    Text {
        id: header
        text: "base"
    }

    function label() {
        return headerText + ":" + baseCount
    }
}