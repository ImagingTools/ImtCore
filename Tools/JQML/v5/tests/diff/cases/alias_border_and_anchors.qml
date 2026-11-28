import QtQuick
Item {
    id: root
    property alias borderWidth: rectangle.border.width
    property alias bottomMargin: rectangle.anchors.bottomMargin
    property bool observe: false
    onBorderWidthChanged: { if (observe) console.log("border", borderWidth) }
    onBottomMarginChanged: { if (observe) console.log("margin", bottomMargin) }
    Rectangle { id: rectangle; border.width: 2 }
    Component.onCompleted: {
        observe = true
        rectangle.border.color = "red"
        rectangle.border.width = 3
        rectangle.anchors.topMargin = 4
        rectangle.anchors.bottomMargin = 5
        console.log("done")
        Qt.quit()
    }
}
