import QtQuick

// ButtonDecorator cancels spacing with `width: -margin` when the label is empty; that value must
// round-trip through the width getter so an icon-only control stays content-sized and centered.
Item {
    id: root
    width: 32
    height: 32

    Item {
        id: content
        anchors.centerIn: parent
        width: icon.width + label.width + spacing
        height: icon.height

        property real spacing: 10

        Item {
            id: icon
            width: 16
            height: 16
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
        }

        Item {
            id: label
            width: -content.spacing
            height: 16
            anchors.left: icon.right
            anchors.leftMargin: content.spacing
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    Component.onCompleted: {
        console.log("labelWidth", label.width)
        console.log("contentWidth", content.width)
        console.log("contentX", content.x)
        Qt.quit()
    }
}
