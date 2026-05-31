import QtQuick 2.12
import Acf 1.0
import imtcontrols 2.0

/*!
    \qmltype CheckBox
    \inqmlmodule imtcontrols

    Checkable option box, API-compatible with Qt Quick Controls \c CheckBox.

    CheckBox presents an option that can be toggled on (checked) or off
    (unchecked) with an optional text label. It extends \l AbstractButton
    with a check indicator and supports the tri-state mode through
    \l tristate / \l checkState.

    \qml
    CheckBox {
        text: qsTr("Enable notifications")
        checked: true
        onToggled: settings.notify = checked
    }
    \endqml

    \sa RadioButton, Switch
*/
AbstractButton {
    id: control
    objectName: "ImtControlsCheckBox"

    checkable: true

    /*! Allow a third, partially-checked state. */
    property bool tristate: false

    /*! Tri-state value: \c Qt.Unchecked, \c Qt.PartiallyChecked or
        \c Qt.Checked. */
    property int checkState: checked ? Qt.Checked : Qt.Unchecked

    spacing: Style.spacingS
    padding: 0
    background: null

    onCheckStateChanged: checked = (checkState !== Qt.Unchecked)
    onCheckedChanged: {
        if (!tristate)
            checkState = checked ? Qt.Checked : Qt.Unchecked;
    }

    contentItem: Item {
        implicitWidth:  indicator.width + (label.text !== "" ? control.spacing + label.implicitWidth : 0)
        implicitHeight: Math.max(indicator.height, label.implicitHeight)

        Rectangle {
            id: indicator
            anchors.verticalCenter: parent.verticalCenter
            width:  Style.controlHeightS
            height: Style.controlHeightS
            radius: Style.radiusS
            color: control.checkState !== Qt.Unchecked ? Style.imaginToolsAccentColor
                                                        : Style.backgroundColor
            opacity: control.enabled ? 1.0 : 0.5
            border.width: Style.buttonBorderWidth
            border.color: control.checkState !== Qt.Unchecked ? Style.imaginToolsAccentColor
                                                              : Style.borderColor
            Behavior on color { ColorAnimation { duration: 100 } }

            // Check mark.
            Text {
                anchors.centerIn: parent
                visible: control.checkState === Qt.Checked
                text: "\u2713"
                color: "#ffffff"
                font.family: Style.fontFamily
                font.pixelSize: Style.fontSizeS
            }

            // Partial mark.
            Rectangle {
                anchors.centerIn: parent
                visible: control.checkState === Qt.PartiallyChecked
                width: parent.width / 2
                height: 2
                color: "#ffffff"
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
