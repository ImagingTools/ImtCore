import QtQuick

Item {
    id: root

    property int baseValue: 1

    signal bump(int step)

    function doBump(step) {
        baseValue += step
        bump(step)
    }
}
