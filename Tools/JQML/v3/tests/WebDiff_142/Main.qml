import QtQuick

// An eliding Text reports its whole text as implicitWidth; only the painted width is cut
Item {
    width: 400
    height: 100

    Text {
        id: elided
        width: 30
        text: "A rather long text"
        elide: Text.ElideRight
        font.pixelSize: 14
    }

    Text {
        id: natural
        text: "A rather long text"
        font.pixelSize: 14
    }

    Component.onCompleted: {
        console.log("same implicit width:", elided.implicitWidth === natural.implicitWidth)
        console.log("wider than its width:", elided.implicitWidth > elided.width)
        console.log("width:", elided.width)
        Qt.quit();
    }
}
