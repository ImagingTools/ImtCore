import QtQuick

Item {
    id: deco
    width: 40
    height: 20

    property int base: 3
    property alias border: bg.border
    property alias radius: bg.radius
    property alias color: bg.color
    property alias backgroundItem: bg

    Rectangle {
        id: bg
        anchors.fill: parent
        color: "red"
        radius: deco.base
        border.width: deco.base
        border.color: "black"
    }
}
