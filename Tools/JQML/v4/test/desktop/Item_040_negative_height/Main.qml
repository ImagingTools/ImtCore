import QtQuick

// Same as Item_039 for height: an explicit negative height must round-trip so spacing
// cancellation in a vertical stack still yields the content size.
Item {
    id: root
    width: 32
    height: 32

    Item {
        id: content
        anchors.centerIn: parent
        width: icon.width
        height: icon.height + label.height + spacing

        property real spacing: 10

        Item {
            id: icon
            width: 16
            height: 16
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Item {
            id: label
            width: 16
            height: -content.spacing
            anchors.top: icon.bottom
            anchors.topMargin: content.spacing
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }

    Component.onCompleted: {
        console.log("labelHeight", label.height)
        console.log("contentHeight", content.height)
        console.log("contentY", content.y)
        Qt.quit()
    }
}
