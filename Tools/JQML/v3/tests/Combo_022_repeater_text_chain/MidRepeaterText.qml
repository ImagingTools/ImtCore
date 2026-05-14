import QtQuick

BaseRepeaterText {
    id: root

    property int bonusCount: 3

    function summary() {
        return countLabel() + ":" + bonusCount
    }
}