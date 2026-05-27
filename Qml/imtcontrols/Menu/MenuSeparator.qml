import QtQuick 2.12
import Acf 1.0
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
    height: Style.marginM
    implicitHeight: height

    /*! Stable type-tag so Menu can distinguish separators from items. */
    readonly property bool isMenuSeparator: true

    Rectangle {
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin:  Style.marginXS
        anchors.rightMargin: Style.marginXS
        height: Style.buttonBorderWidth
        color: Style.borderColor
    }
}
