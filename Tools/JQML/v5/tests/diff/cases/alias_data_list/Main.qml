import QtQuick

Item {
    Card {
        id: card
        media: Rectangle {
            objectName: "mediaRect"
            anchors.left: parent.left
            anchors.right: parent.right
            height: 10
            Component.onCompleted: console.log("media parent", parent && parent.objectName, width)
        }
        Rectangle { objectName: "body"; anchors.fill: parent; Component.onCompleted: console.log("body parent", parent.objectName, width, height) }
    }
    Component.onCompleted: {
        console.log(card.media.length, card.media[0].objectName, card.content.length, card.content[0].objectName)
        Qt.quit()
    }
}
