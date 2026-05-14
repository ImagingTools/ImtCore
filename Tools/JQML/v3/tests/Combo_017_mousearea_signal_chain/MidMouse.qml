import QtQuick

BaseMouse {
    id: root

    property int midState: 2

    signal midHit()

    function summary() {
        return baseLabel() + ":" + midState
    }
}