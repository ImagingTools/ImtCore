import QtQuick 2.12
import imtcontrols 1.0

/*!
    \qmltype MenuSeparator
    \inqmlmodule imtcontrols

    Horizontal divider between groups of menu items.
*/
Item {
    id: sep
    objectName: "ImtControlsMenuSeparator"

    width: parent ? parent.width : 0
    height: (typeof Style !== "undefined" && Style.marginM) ? Style.marginM : 8
    implicitHeight: height

    /*! Stable type-tag so Menu can distinguish separators from items. */
    readonly property bool isMenuSeparator: true

    Rectangle {
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin:  (typeof Style !== "undefined" && Style.marginXS) ? Style.marginXS : 4
        anchors.rightMargin: (typeof Style !== "undefined" && Style.marginXS) ? Style.marginXS : 4
        height: 1
        color: (typeof Style !== "undefined" && Style.borderColor) ? Style.borderColor : "#cccccc"
    }
}
