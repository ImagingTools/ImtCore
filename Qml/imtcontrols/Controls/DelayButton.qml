import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

/*!
    \qmltype DelayButton
    \inqmlmodule imtcontrols

    Press-and-hold button, API-compatible with Qt Quick Controls
    \c DelayButton.

    DelayButton is a checkable button that is activated only after being held
    pressed for \l delay milliseconds. While held, \l progress fills from
    \c 0 to \c 1; reaching \c 1 toggles \l checked and emits \l activated().
    Releasing the button before completion drains the progress back to \c 0.
    This guards against accidental activation of critical actions.

    \qml
    DelayButton {
        text: qsTr("Hold to delete")
        delay: 2000
        onActivated: model.removeSelected()
    }
    \endqml

    \sa AbstractButton, RoundButton
*/
AbstractButton {
    id: control
    objectName: "ImtControlsDelayButton"

    checkable: true

    /*! Hold duration before the button activates, in milliseconds. */
    property int delay: 3000

    /*! Normalised hold progress in [0, 1]; \c 1 means activated. */
    readonly property real progress: d.progress

    /*! Emitted once the button has been held for \l delay milliseconds. */
    signal activated()

    padding: Style.spacingS
    implicitWidth:  Math.max(Style.buttonWidthXL, implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Style.controlHeightL

    QtObject {
        id: d
        property real progress: control.checked ? 1 : 0
        onProgressChanged: {
            if (progress >= 1 && !control.checked) {
                control.checked = true;
                control.activated();
            }
        }
    }

    NumberAnimation {
        id: fillAnimation
        target: d
        property: "progress"
        to: 1
        easing.type: Easing.Linear
    }

    NumberAnimation {
        id: drainAnimation
        target: d
        property: "progress"
        to: 0
        duration: 200
        easing.type: Easing.Linear
    }

    onPressedChanged: {
        if (pressed) {
            drainAnimation.stop();
            fillAnimation.duration = control.delay * (1 - d.progress);
            fillAnimation.start();
        } else if (!checked) {
            fillAnimation.stop();
            drainAnimation.start();
        }
    }

    onCheckedChanged: {
        if (!checked && !pressed) {
            fillAnimation.stop();
            drainAnimation.start();
        }
    }

    background: Rectangle {
        radius: Style.radiusM
        color: !control.enabled ? Style.buttonInactiveColor
               : control.checked ? Style.imaginToolsAccentColor
               : control.hovered ? Style.buttonHoverColor
               : Style.buttonColor
        border.width: Style.buttonBorderWidth
        border.color: Style.buttonBorderColor

        // Progress overlay growing from the left as the button is held.
        Rectangle {
            height: parent.height
            width: parent.width * control.progress
            radius: parent.radius
            color: Style.imaginToolsAccentColor
            opacity: control.checked ? 0 : 0.5
        }
    }

    contentItem: Text {
        text: control.text
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        color: control.checked ? "#ffffff"
               : control.enabled ? Style.buttonTextColor : Style.inactiveTextColor
        font.family: Style.fontFamily
        font.pixelSize: Style.fontSizeM
    }
}
