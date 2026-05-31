import QtQuick 2.12
import Acf 1.0
import imtcontrols 2.0

/*!
    \qmltype BusyIndicator
    \inqmlmodule imtcontrols

    Activity spinner, API-compatible with Qt Quick Controls
    \c BusyIndicator.

    BusyIndicator shows that an operation of unknown duration is in progress.
    It animates a rotating ring while \l running is \c true and hides itself
    when stopped.

    \qml
    BusyIndicator {
        running: loader.status === Loader.Loading
    }
    \endqml

    \sa ProgressBar
*/
Control {
    id: control
    objectName: "ImtControlsBusyIndicator"

    /*! Whether the indicator is animating and visible. */
    property bool running: true

    implicitWidth:  Style.controlHeightL
    implicitHeight: Style.controlHeightL

    opacity: running ? 1.0 : 0.0

    contentItem: Item {
        implicitWidth:  Style.controlHeightL
        implicitHeight: Style.controlHeightL

        Canvas {
            id: ring
            anchors.fill: parent
            rotation: 0
            onPaint: {
                var ctx = getContext("2d");
                var w = width;
                var h = height;
                var radius = Math.min(w, h) / 2 - 2;
                ctx.reset();
                ctx.lineWidth = Math.max(2, radius * 0.18);
                ctx.lineCap = "round";
                ctx.strokeStyle = Style.imaginToolsAccentColor;
                ctx.beginPath();
                ctx.arc(w / 2, h / 2, radius, 0, Math.PI * 1.4);
                ctx.stroke();
            }

            RotationAnimator {
                target: ring
                running: control.running && control.visible
                from: 0
                to: 360
                duration: 900
                loops: Animation.Infinite
            }
        }
    }
}
