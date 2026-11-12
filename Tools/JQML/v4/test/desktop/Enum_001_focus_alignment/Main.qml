import QtQuick

Item {
    id: root

    // These used to stay as the names "WheelFocus" and "AlignHCenter", which an int property refuses.
    property int focusPolicy: Qt.WheelFocus
    property int horizontalAlignment: Text.AlignHCenter
    property int alignment: Qt.AlignCenter

    Component.onCompleted: {
        console.log("focus", Qt.NoFocus, Qt.TabFocus, Qt.ClickFocus, Qt.StrongFocus, Qt.WheelFocus)
        console.log("policy", focusPolicy)
        console.log("align", Qt.AlignLeft, Qt.AlignRight, Qt.AlignHCenter, Qt.AlignJustify, Qt.AlignTop, Qt.AlignBottom, Qt.AlignVCenter, Qt.AlignCenter)
        console.log("text-align", Text.AlignHCenter, Text.AlignVCenter, horizontalAlignment, alignment)
        Qt.quit()
    }
}
