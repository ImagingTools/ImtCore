import QtQuick

Item {
    id: root

    property int baseCount: 2

    Repeater {
        id: repeater
        model: root.baseCount

        delegate: Text {
            property int itemIndex: index + 1
            text: "item" + itemIndex
        }
    }

    function countLabel() {
        return "count:" + root.baseCount
    }
}