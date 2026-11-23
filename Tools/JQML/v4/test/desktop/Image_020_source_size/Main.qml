import QtQuick

// Image.sourceSize is written by group properties (`sourceSize.width: 20`, 245 uses in our QML): the width and the height are kept, are read back,
// and with no picture loaded they do not change the size of the item.
Item {
    width: 40
    height: 40

    Image {
        id: a
        width: 50
        height: 30
        sourceSize.width: 20
        sourceSize.height: 10
    }
    Image {
        id: b
        sourceSize: Qt.size(16, 8)
    }
    Image { id: c }

    Component.onCompleted: {
        console.log("a", a.sourceSize.width, a.sourceSize.height, a.width, a.height)
        console.log("b", b.sourceSize.width, b.sourceSize.height)
        console.log("c", c.sourceSize.width, c.sourceSize.height)
        a.sourceSize.width = 64
        console.log("a changed", a.sourceSize.width, a.sourceSize.height)
        var n = 0
        a.sourceSizeChanged.connect(function () { n++ })
        a.sourceSize = Qt.size(5, 6)
        console.log("signal", n > 0, a.sourceSize.width, a.sourceSize.height)
        Qt.quit()
    }
}
