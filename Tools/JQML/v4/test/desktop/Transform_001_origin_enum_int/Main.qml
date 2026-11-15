import QtQuick

Item {
    id: root
    width: 100
    height: 100

    // Item.TransformOrigin is an int enum: it can be stored in a `property int` (imtcontrols' Popup does this).
    property int origin: Item.Center

    Rectangle {
        id: box
        width: 20
        height: 20
        transformOrigin: root.origin
    }

    Component.onCompleted: {
        console.log(Item.TopLeft, Item.Center, Item.BottomRight)
        console.log(box.transformOrigin === Item.Center, box.transformOrigin)
        box.transformOrigin = Item.TopLeft
        console.log(box.transformOrigin)
        root.origin = Item.BottomRight
        console.log(box.transformOrigin)
        Qt.quit()
    }
}
