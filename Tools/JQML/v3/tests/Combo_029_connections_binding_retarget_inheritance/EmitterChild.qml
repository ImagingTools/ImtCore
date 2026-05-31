import QtQuick

EmitterBase {
    id: root

    property int offset: 1

    function fireWithOffset(label, v) {
        fire(label, v + offset)
    }
}
