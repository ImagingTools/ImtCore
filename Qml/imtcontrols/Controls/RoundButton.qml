import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

/*!
    \qmltype RoundButton
    \inqmlmodule imtcontrols

    Circular push button, API-compatible with Qt Quick Controls
    \c RoundButton.

    RoundButton behaves like a regular button but its background is fully
    rounded (a circle when width equals height). It is commonly used for
    floating action buttons or compact icon buttons.

    \qml
    RoundButton {
        text: "+"
        onClicked: model.append({})
    }
    \endqml

    \sa AbstractButton, ToolButton
*/
AbstractButton {
    id: control
    objectName: "ImtControlsRoundButton"

    /*! Corner radius. Defaults to a fully circular button. */
    property real radius: Math.min(width, height) / 2

    /*! Highlight the button using the accent colour. */
    property bool highlighted: false

    padding: Style.spacingS
    implicitWidth:  Math.max(Style.controlHeightL, implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Style.controlHeightL

    background: Rectangle {
        radius: control.radius
        color: !control.enabled ? Style.buttonInactiveColor
               : control.pressed ? Style.buttonPressedColor
               : control.hovered ? Style.buttonHoverColor
               : control.highlighted ? Style.imaginToolsAccentColor
               : Style.buttonColor
        border.width: Style.buttonBorderWidth
        border.color: Style.buttonBorderColor
        Behavior on color { ColorAnimation { duration: 100 } }
    }

    contentItem: Text {
        text: control.text
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        color: control.highlighted ? "#ffffff"
               : control.enabled ? Style.buttonTextColor : Style.inactiveTextColor
        font.family: Style.fontFamily
        font.pixelSize: Style.fontSizeL
    }
}
