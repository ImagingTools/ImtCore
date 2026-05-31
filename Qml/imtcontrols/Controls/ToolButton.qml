import QtQuick 2.12
import Acf 1.0
import imtcontrols 2.0

/*!
    \qmltype ToolButton
    \inqmlmodule imtcontrols

    Tool bar button, API-compatible with Qt Quick Controls \c ToolButton.

    ToolButton is a \l Button variant intended for \l ToolBar: it has no
    permanent border or fill and only reveals a subtle background when
    hovered, pressed or \l checked. Use it for compact, icon- or text-only
    actions grouped in a tool bar.

    \qml
    ToolBar {
        Row {
            ToolButton { text: qsTr("Open") }
            ToolButton { text: qsTr("Save") }
        }
    }
    \endqml

    \sa Button, ToolBar
*/
AbstractButton {
    id: control
    objectName: "ImtControlsToolButton"

    /*! Render the button with the accent colour. */
    property bool highlighted: false

    padding: Style.spacingS
    leftPadding: Style.spacingM
    rightPadding: Style.spacingM

    implicitWidth:  Math.max(Style.controlHeightL, implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Style.controlHeightL

    background: Rectangle {
        radius: Style.radiusS
        color: !control.enabled ? "transparent"
               : control.pressed ? Style.buttonPressedColor
               : control.checked ? Style.buttonHoverColor
               : control.hovered ? Style.buttonHoverColor
               : control.highlighted ? Style.imaginToolsAccentColor
               : "transparent"
        Behavior on color { ColorAnimation { duration: 100 } }
    }

    contentItem: Text {
        text: control.text
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
        color: control.highlighted ? "#ffffff"
               : control.enabled ? Style.buttonTextColor : Style.inactiveTextColor
        font.family: Style.fontFamily
        font.pixelSize: Style.fontSizeM
    }
}
