import QtQuick

Item {
    id: root

    property int baseCount: 3

    signal assembled()

    Repeater {
        id: repeater
        model: root.baseCount

        delegate: Item {
            property int value: index + 1
            width: 10
            height: 10
        }
    }

    function totalCount() {
        return baseCount
    }

    Component.onCompleted: {
        assembled()
    }
}
