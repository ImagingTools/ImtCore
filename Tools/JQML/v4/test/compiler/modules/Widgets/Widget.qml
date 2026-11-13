import QtQuick 2.15

Rectangle {
    id: root
    property alias label: text.text
    width: 160
    height: 40
    color: Theme.accent

    Text {
        id: text
        objectName: "widgetText"
        anchors.centerIn: parent
        color: "white"
    }
}
