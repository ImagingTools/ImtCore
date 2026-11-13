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

        TextEdit {
            id: te
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.margins: 4
            wrapMode: TextEdit.WordWrap
        }
    }

    function st(tag) {
        console.log(tag, "x", te.x, "y", te.y, "w", te.width, "h==painted", te.height === te.paintedHeight, "content==painted+8", flick.contentHeight === te.paintedHeight + 8, "lines>1", te.lineCount > 1)
    }

    Component.onCompleted: {
        console.log("parent is content item", te.parent === flick.contentItem)
        st("empty")
        te.text = "a long line of words that has to wrap around the narrow editor and then some more words"
        st("long text")
        var wrapped = te.lineCount
        te.cursorPosition = te.length
        console.log("cursor below the first line", te.cursorRectangle.y > 0, "cursor inside", te.cursorRectangle.x < te.width)
        te.wrapMode = TextEdit.NoWrap
        st("no wrap")
        console.log("fewer lines without wrapping", te.lineCount < wrapped, te.lineCount)
        te.wrapMode = TextEdit.WordWrap
        te.text = ""
        st("cleared")
        Qt.quit()
    }
}
