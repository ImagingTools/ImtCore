import QtQuick

Item {
    id: root

    property int baseCount: 3

    Repeater {
        id: repeater
        model: root.baseCount

        delegate: Item {
            property int labelIndex: index + 1
        }
    }

    function modelCount() {
        return root.baseCount
    }
}