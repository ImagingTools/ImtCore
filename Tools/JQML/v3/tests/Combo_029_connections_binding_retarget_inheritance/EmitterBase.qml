import QtQuick

Item {
    id: root

    property int value: 0
    signal tick(string label)

    function fire(label, v) {
        value = v
        tick(label)
    }
}
