import QtQuick

BaseTimerState {
    id: root

    property int midTicks: 1

    function summary() {
        return baseTicks + ":" + midTicks
    }
}