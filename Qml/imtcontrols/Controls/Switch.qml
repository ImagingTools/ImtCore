import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

/*!
    \qmltype Switch
    \inqmlmodule imtcontrols

    On/off toggle, API-compatible with Qt Quick Controls \c Switch.

    Switch is a checkable button rendered as a sliding handle inside a track,
    with an optional text label. Clicking or tapping toggles \l checked and
    animates the handle between the off and on positions.

    \qml
    Switch {
        text: qsTr("Wi-Fi")
        checked: true
        onToggled: console.log("checked:", checked)
    }
    \endqml

    \sa CheckBox, RadioButton
*/
AbstractButton {
    id: control
    objectName: "ImtControlsSwitch"

    checkable: true

    /*! Normalised handle position in [0, 1]; 1 means fully on. */
    readonly property real position: checked ? 1 : 0

    spacing: Style.spacingS

    // No outer background; the indicator carries the look.
    background: null

    contentItem: Item {
        implicitWidth:  indicator.width + (label.text !== "" ? control.spacing + label.implicitWidth : 0)
        implicitHeight: Math.max(indicator.height, label.implicitHeight)

        // Track + handle.
        Item {
            id: indicator
            anchors.verticalCenter: parent.verticalCenter
            width:  Style.controlHeightL
            height: Style.controlHeightS

            Rectangle {
                id: track
                anchors.fill: parent
                radius: height / 2
                color: control.checked ? Style.imaginToolsAccentColor
                                       : Style.buttonInactiveColor
                opacity: control.enabled ? 1.0 : 0.5
            }

            Rectangle {
                id: handle
                width:  parent.height - 4
                height: width
                radius: width / 2
                y: 2
                x: 2 + control.position * (parent.width - width - 4)
                color: "#ffffff"
                border.width: Style.buttonBorderWidth
                border.color: Style.borderColor
            }
        }

        Text {
            id: label
            anchors.left: indicator.right
            anchors.leftMargin: control.spacing
            anchors.verticalCenter: parent.verticalCenter
            visible: control.text !== ""
            text: control.text
            color: control.enabled ? Style.textColor : Style.inactiveTextColor
            font.family: Style.fontFamily
            font.pixelSize: Style.fontSizeM
        }
    }
}
