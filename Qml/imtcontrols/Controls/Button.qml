import QtQuick 2.12
import Acf 1.0
import imtcontrols 2.0

/*!
    \qmltype Button
    \inqmlmodule imtcontrols

    Push button, API-compatible with Qt Quick Controls \c Button.

    Button is the standard command control: a labelled, clickable surface
    that emits \l clicked() when activated. It extends \l AbstractButton with
    a themed \l background and \l contentItem and adds the \l highlighted and
    \l flat presentation flags.

    \qml
    Button {
        text: qsTr("Apply")
        highlighted: true
        onClicked: form.apply()
    }
    \endqml

    \sa AbstractButton, ToolButton, RoundButton
*/
AbstractButton {
    id: control
    objectName: "ImtControlsButton"

    /*! Render the button with the accent colour to mark the default action. */
    property bool highlighted: false

    /*! Draw the button without a filled background until hovered/pressed. */
    property bool flat: false

    padding: Style.spacingS
    leftPadding: Style.spacingM
    rightPadding: Style.spacingM

    implicitWidth:  Math.max(Style.buttonWidthM, implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(Style.controlHeightL, implicitContentHeight + topPadding + bottomPadding)

    background: Rectangle {
        radius: Style.buttonRadius
        color: !control.enabled ? Style.buttonInactiveColor
               : control.pressed ? Style.buttonPressedColor
               : control.hovered ? Style.buttonHoverColor
               : control.highlighted ? Style.imaginToolsAccentColor
               : control.flat ? "transparent"
               : Style.buttonColor
        border.width: control.flat ? 0 : Style.buttonBorderWidth
        border.color: Style.buttonBorderColor
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
