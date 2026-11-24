import QtQuick

// a size value written to the sourceSize group (a binding, an assignment, a binding to a size of another image)
Item {
    property int w: 48
    Image { id: a; sourceSize: Qt.size(w, 24); onSourceSizeChanged: console.log("a changed", sourceSize.width, sourceSize.height) }
    Image { id: b; sourceSize.width: 10 }
    Image { id: c; sourceSize: a.sourceSize }
    Component.onCompleted: {
        console.log(a.sourceSize.width, a.sourceSize.height, b.sourceSize.width, b.sourceSize.height, c.sourceSize.width, c.sourceSize.height)
        w = 64
        console.log(a.sourceSize.width, a.sourceSize.height, c.sourceSize.width)
        b.sourceSize = Qt.size(5, 6)
        console.log(b.sourceSize.width, b.sourceSize.height)
        Qt.quit()
    }
}
