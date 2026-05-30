import QtQuick

Item {
    id: root

    property alias title: label.text
    property int value: 10

    Text {
        id: label
        text: "child-default"
    }
}
