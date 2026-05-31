import QtQuick

Item {
    id: root

    property int hitCount: 0

    MouseArea {
        id: area
        anchors.fill: parent
    }

    function areaState() {
        return "mouse:" + hitCount
    }
}
