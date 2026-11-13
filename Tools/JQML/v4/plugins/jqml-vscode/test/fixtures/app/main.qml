import QtQuick 2.12
import mymod 1.0

Rectangle {
    id: root
    width: 100
    color: Theme.background

    Card {
        id: card
        title: "x"
        onActivated: root.width = index
    }

    MouseArea {
        id: area
        anchors.fill: parent
        onClicked: card.refresh(true)
    }
}
