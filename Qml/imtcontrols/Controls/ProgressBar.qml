import QtQuick 2.12
import Acf 1.0
import imtcontrols 2.0

/*!
    \qmltype ProgressBar
    \inqmlmodule imtcontrols

    Progress indicator, API-compatible with Qt Quick Controls
    \c ProgressBar.

    ProgressBar shows the progress of an operation as a horizontal fill
    between \l from and \l to. Set \l indeterminate when the duration is
    unknown to animate a sweeping highlight instead of a fixed fill.

    \qml
    ProgressBar {
        from: 0; to: 100
        value: download.percent
    }
    \endqml

    \sa BusyIndicator, Slider
*/
Control {
    id: control
    objectName: "ImtControlsProgressBar"

    /*! Lower bound of the progress range. */
    property real from: 0
    /*! Upper bound of the progress range. */
    property real to: 1
    /*! Current progress value, clamped to [\l from, \l to]. */
    property real value: 0
    /*! Animate a sweep instead of showing a definite value. */
    property bool indeterminate: false

    /*! Normalised progress in [0, 1] (read-only). */
    readonly property real position: to > from ? (Math.max(from, Math.min(to, value)) - from) / (to - from) : 0
    /*! Visual progress in [0, 1] (read-only). */
    readonly property real visualPosition: position

    implicitWidth:  Style.buttonWidthL
    implicitHeight: Style.controlHeightS

    contentItem: Item {
        implicitWidth:  Style.buttonWidthL
        implicitHeight: Style.controlHeightS

        Rectangle {
            id: track
            anchors.verticalCenter: parent.verticalCenter
            width: parent.width
            height: Math.max(4, parent.height / 3)
            radius: height / 2
            color: Style.buttonInactiveColor

            // Definite fill.
            Rectangle {
                visible: !control.indeterminate
                width: parent.width * control.visualPosition
                height: parent.height
                radius: parent.radius
                color: Style.imaginToolsAccentColor
                Behavior on width { NumberAnimation { duration: 120 } }
            }

            // Indeterminate sweep.
            Rectangle {
                id: sweep
                visible: control.indeterminate
                width: parent.width * 0.3
                height: parent.height
                radius: parent.radius
                color: Style.imaginToolsAccentColor
                x: 0

                NumberAnimation {
                    target: sweep
                    property: "x"
                    running: control.indeterminate && control.visible
                    from: -sweep.width
                    to: track.width
                    duration: 1200
                    loops: Animation.Infinite
                }
            }
        }
    }
}
