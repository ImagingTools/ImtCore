import QtQuick

BaseModelRepeater {
    id: root

    property int extraCount: 2

    function summary() {
        return modelCount() + ":" + root.extraCount
    }
}