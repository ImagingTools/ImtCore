import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

/*!
    \qmltype TabButton
    \inqmlmodule imtcontrols

    Tab header button, API-compatible with Qt Quick Controls \c TabButton.

    TabButton is a checkable, auto-exclusive button intended to be placed
    inside a \l TabBar. The currently selected tab is highlighted with an
    accent underline.

    \qml
    TabBar {
        TabButton { text: qsTr("General") }
        TabButton { text: qsTr("Network") }
    }
    \endqml

    \sa TabBar
*/
AbstractButton {
    id: control
    objectName: "ImtControlsTabButton"

    checkable: true
    autoExclusive: true

    padding: Style.spacingM
    implicitWidth:  Math.max(Style.buttonWidthXL, implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Style.controlHeightL

    background: Rectangle {
        color: control.checked ? Style.backgroundColor2
               : control.hovered ? Style.buttonHoverColor
               : "transparent"
        // Accent underline on the active tab.
        Rectangle {
            anchors.bottom: parent.bottom
            width: parent.width
            height: control.checked ? 2 : Style.buttonBorderWidth
            color: control.checked ? Style.imaginToolsAccentColor : Style.borderColor
        }
    }

    contentItem: Text {
        text: control.text
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
        color: !control.enabled ? Style.inactiveTextColor
               : control.checked ? Style.textSelectedColor
               : Style.textColor
        font.family: Style.fontFamily
        font.pixelSize: Style.fontSizeM
        font.bold: control.checked
    }
}
