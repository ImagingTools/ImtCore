import QtQuick

BaseMouseArea {
    id: root

    property string tag: "mouse"

    function inspect() {
        return tag + ":" + areaState()
    }
}
