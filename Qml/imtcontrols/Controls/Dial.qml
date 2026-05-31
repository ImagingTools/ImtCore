import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

/*!
    \qmltype Dial
    \inqmlmodule imtcontrols

    Circular value control, API-compatible with Qt Quick Controls \c Dial.

    Dial lets the user choose a value in the range [\l from, \l to] by
    dragging a handle around a circular track. \l value holds the current
    value and \l position is its normalised counterpart in [0, 1].

    \qml
    Dial {
        from: 0
        to: 100
        value: 25
        onMoved: console.log("value:", value)
    }
    \endqml

    \sa Slider
*/
Item {
    id: control
    objectName: "ImtControlsDial"

    /*! Lower bound of the range. */
    property real from: 0
    /*! Upper bound of the range. */
    property real to: 1
    /*! Current value, clamped to [\l from, \l to]. */
    property real value: 0
    /*! Step used when snapping; \c 0 means continuous. */
    property real stepSize: 0

    /*! Normalised value in [0, 1]. */
    readonly property real position: (to === from) ? 0 : (value - from) / (to - from)
    /*! True while the user drags the handle. */
    readonly property bool pressed: mouse.dragging

    /*! Emitted whenever the user changes \l value by interaction. */
    signal moved()

    implicitWidth:  Style.controlHeightL * 2
    implicitHeight: Style.controlHeightL * 2
    width:  implicitWidth
    height: implicitHeight

    // Sweep from 140 to 400 degrees (i.e. -220..+40 visually), like Qt's Dial.
    readonly property real _startAngle: 140
    readonly property real _spanAngle: 260
    readonly property real _angle: _startAngle + position * _spanAngle

    Canvas {
        id: track
        anchors.fill: parent
        onPaint: {
            var ctx = getContext("2d");
            ctx.reset();
            var cx = width / 2, cy = height / 2;
            var r = Math.min(width, height) / 2 - 6;
            var a0 = control._startAngle * Math.PI / 180;
            var a1 = (control._startAngle + control._spanAngle) * Math.PI / 180;
            var av = control._angle * Math.PI / 180;
            ctx.lineWidth = 4;
            ctx.lineCap = "round";
            // Background arc.
            ctx.beginPath();
            ctx.strokeStyle = Style.borderColor;
            ctx.arc(cx, cy, r, a0, a1, false);
            ctx.stroke();
            // Progress arc.
            ctx.beginPath();
            ctx.strokeStyle = Style.imaginToolsAccentColor;
            ctx.arc(cx, cy, r, a0, av, false);
            ctx.stroke();
        }
    }

    // Repaint whenever the value changes.
    onValueChanged: track.requestPaint()
    onWidthChanged: track.requestPaint()
    onHeightChanged: track.requestPaint()

    // Handle.
    Rectangle {
        id: handle
        width: Style.spacingM
        height: width
        radius: width / 2
        color: control.pressed ? Style.buttonPressedColor : "#ffffff"
        border.width: Style.buttonBorderWidth + 1
        border.color: Style.imaginToolsAccentColor
        x: control.width  / 2 + (Math.min(control.width, control.height) / 2 - 6)
              * Math.cos(control._angle * Math.PI / 180) - width / 2
        y: control.height / 2 + (Math.min(control.width, control.height) / 2 - 6)
              * Math.sin(control._angle * Math.PI / 180) - height / 2
    }

    MouseArea {
        id: mouse
        anchors.fill: parent
        enabled: control.enabled
        property bool dragging: false
        onPressed: { dragging = true; _updateFromPoint(mouse.x, mouse.y); }
        onReleased: dragging = false
        onPositionChanged: { if (dragging) _updateFromPoint(mouse.x, mouse.y); }

        function _updateFromPoint(px, py) {
            var dx = px - control.width / 2;
            var dy = py - control.height / 2;
            var ang = Math.atan2(dy, dx) * 180 / Math.PI; // -180..180
            // Normalise into the dial's sweep starting at _startAngle.
            var rel = ang - control._startAngle;
            while (rel < 0) rel += 360;
            while (rel > 360) rel -= 360;
            if (rel > control._spanAngle) {
                // Snap to nearest end when in the dead zone.
                rel = (rel - control._spanAngle) < (360 - rel)
                      ? control._spanAngle : 0;
            }
            var pos = rel / control._spanAngle;
            var v = control.from + pos * (control.to - control.from);
            if (control.stepSize > 0)
                v = Math.round(v / control.stepSize) * control.stepSize;
            v = Math.max(control.from, Math.min(control.to, v));
            if (v !== control.value) {
                control.value = v;
                control.moved();
            }
        }
    }
}
