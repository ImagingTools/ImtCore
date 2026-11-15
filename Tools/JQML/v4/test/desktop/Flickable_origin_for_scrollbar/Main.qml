import QtQuick

// A scroll bar places its thumb with `(contentY - originY) * k`: a Flickable has originX and originY (0 here), without them the
// thumb of every scroll bar of a Flickable stays at NaN, which is 0, and the bar does not follow the content.
Item {
    id: root
    width: 300
    height: 200

    Flickable {
        id: flick
        width: 300
        height: 200
        contentWidth: 600
        contentHeight: 1000

        Rectangle {
            width: 600
            height: 1000
            color: "lightblue"
        }
    }

    Rectangle {
        id: thumb
        width: 10
        height: 40
        y: (flick.contentY - flick.originY) * (flick.height - thumb.height) / (flick.contentHeight - flick.height)
    }

    Rectangle {
        id: thumbX
        width: 40
        height: 10
        y: 190
        x: (flick.contentX - flick.originX) * (flick.width - thumbX.width) / (flick.contentWidth - flick.width)
    }

    Component.onCompleted: {
        console.log("origin", flick.originX, flick.originY)
        flick.contentY = 400
        console.log("thumb", thumb.y)
        flick.contentY = 800
        console.log("thumb", thumb.y)
        flick.contentX = 100
        console.log("thumbX", thumbX.x)
        flick.contentX = 300
        console.log("thumbX", thumbX.x)
        Qt.quit()
    }
}
