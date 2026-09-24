import QtQuick

Item {
    id: root
    property alias border: background.border

    Rectangle {
        id: background
        width: 10
        height: 10
    }
}
