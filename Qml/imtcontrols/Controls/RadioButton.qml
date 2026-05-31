import QtQuick 2.12
import Acf 1.0
import imtcontrols 2.0

/*!
    \qmltype RadioButton
    \inqmlmodule imtcontrols

    Exclusive option button, API-compatible with Qt Quick Controls
    \c RadioButton.

    RadioButton presents one of several mutually exclusive options. Radio
    buttons that share the same parent are auto-exclusive, so checking one
    unchecks the others. It extends \l AbstractButton with a circular
    indicator.

    \qml
    Column {
        RadioButton { text: qsTr("Small");  checked: true }
        RadioButton { text: qsTr("Medium") }
        RadioButton { text: qsTr("Large") }
    }
    \endqml

    \sa CheckBox, Switch
*/
AbstractButton {
    id: control
    objectName: "ImtControlsRadioButton"

    checkable: true
    autoExclusive: true

    spacing: Style.spacingS
    padding: 0
    background: null

    contentItem: Item {
        implicitWidth:  indicator.width + (label.text !== "" ? control.spacing + label.implicitWidth : 0)
        implicitHeight: Math.max(indicator.height, label.implicitHeight)

        Rectangle {
            id: indicator
            anchors.verticalCenter: parent.verticalCenter
            width:  Style.controlHeightS
            height: Style.controlHeightS
            radius: width / 2
            color: Style.backgroundColor
            opacity: control.enabled ? 1.0 : 0.5
            border.width: Style.buttonBorderWidth
            border.color: control.checked ? Style.imaginToolsAccentColor : Style.borderColor

            Rectangle {
                anchors.centerIn: parent
                visible: control.checked
                width: parent.width / 2
                height: width
                radius: width / 2
                color: Style.imaginToolsAccentColor
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
