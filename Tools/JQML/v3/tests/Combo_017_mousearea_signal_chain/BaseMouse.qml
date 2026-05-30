import QtQuick

Item {
    id: root

    property int baseState: 1

    signal baseHit()

    MouseArea {
        id: area
        anchors.fill: parent
    }

    function baseLabel() {
        return "mouse:" + baseState
    }
}