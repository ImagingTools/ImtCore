import QtQuick

Item {
    width: 300
    height: 200

    Flickable {
        id: flick
        width: 200
        height: 40
        contentWidth: width
        contentHeight: te.paintedHeight + 8

        // Anchors to the Flickable itself are refused (it is not the parent or a sibling): the editor keeps its own size.
        TextEdit {
            id: te
            anchors.left: flick.left
            anchors.right: flick.right
            anchors.top: flick.top
            anchors.margins: 4
            wrapMode: TextEdit.WordWrap
        }
    }

    Component.onCompleted: {
        console.log("parent is content item", te.parent === flick.contentItem)
        console.log("empty", te.x, te.y, te.width, te.height > 0)
        te.text = "some text"
        console.log("text", te.x, te.y, te.width > 0, te.width === te.implicitWidth)
        Qt.quit()
    }
}
