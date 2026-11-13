import QtQuick 2.12

Item {
    id: baseRoot

    property string title: ""
    property int level: 0
    signal activated(int index)

    function refresh(force) {
        return force
    }
}
