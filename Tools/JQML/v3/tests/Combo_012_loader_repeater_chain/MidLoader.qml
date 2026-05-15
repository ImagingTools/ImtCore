import QtQuick

BaseLoader {
    id: root

    property int bonusCount: 2

    Repeater {
        id: repeater
        model: root.bonusCount

        delegate: Item {
            property int number: index + 1
        }
    }

    function summary() {
        return loadedSize() + ":" + root.bonusCount
    }
}