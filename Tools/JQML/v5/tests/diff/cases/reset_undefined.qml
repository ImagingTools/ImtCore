import QtQuick
import QtQuick.Controls

// undefined resets a property: anchors.leftMargin follows anchors.margins again; width follows implicitWidth; a layout
// in a layout; Popup.transformOrigin by the enums of Popup
Item {
    width: 300; height: 200
    property bool wide: true
    Item { id: box; anchors.fill: parent; anchors.margins: 5; anchors.leftMargin: wide ? 40 : undefined }
    Rectangle { id: r; implicitWidth: 33; width: wide ? 100 : undefined; height: 10 }
    Popup { id: p; transformOrigin: Popup.TopRight }
    Component.onCompleted: {
        console.log(box.x, box.width, r.width, p.transformOrigin, Popup.BottomLeft)
        wide = false
        console.log(box.x, box.width, box.anchors.leftMargin, r.width)
        wide = true; r.implicitWidth = 50; console.log("again", box.x, r.width)
        wide = false; console.log("reset", r.width)
        Qt.quit()
    }
}
