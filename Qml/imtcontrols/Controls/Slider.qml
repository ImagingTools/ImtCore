import QtQuick 2.12
import Acf 1.0
import imtcontrols 2.0

/*!
    \qmltype Slider
    \inqmlmodule imtcontrols

    Value slider, API-compatible with Qt Quick Controls \c Slider.

    Slider lets the user select a single \l value from a continuous range
    [\l from, \l to] by dragging a handle along a track. Set \l stepSize to
    snap to discrete values and \l orientation to lay the slider out
    horizontally (default) or vertically.

    \qml
    Slider {
        from: 0; to: 100; value: 30
        onMoved: brightness = value
    }
    \endqml

    \sa RangeSlider, Dial
*/
Control {
    id: control
    objectName: "ImtControlsSlider"

    /*! Lower bound of the range. */
    property real from: 0
    /*! Upper bound of the range. */
    property real to: 1
    /*! Current value, clamped to [\l from, \l to]. */
    property real value: 0
    /*! Snap step; \c 0 means continuous. */
    property real stepSize: 0
    /*! Layout direction: \c Qt.Horizontal (default) or \c Qt.Vertical. */
    property int orientation: Qt.Horizontal
    readonly property bool vertical: orientation === Qt.Vertical
    /*! True while the handle is pressed. */
    readonly property bool pressed: drag.drag.active

    /*! Normalised value in [0, 1] (read-only). */
    readonly property real position: to > from ? (Math.max(from, Math.min(to, value)) - from) / (to - from) : 0
    readonly property alias visualPosition: control.position

    /*! Emitted on user-driven value changes. */
    signal moved()

    spacing: 0
    implicitWidth:  vertical ? Style.controlHeightL : Style.buttonWidthL
    implicitHeight: vertical ? Style.buttonWidthL : Style.controlHeightL

    function _setFromPosition(p) {
        var clamped = Math.max(0, Math.min(1, p));
        var v = from + clamped * (to - from);
        if (stepSize > 0)
            v = from + Math.round((v - from) / stepSize) * stepSize;
        v = Math.max(from, Math.min(to, v));
        if (v !== value) {
            value = v;
            moved();
        }
    }

    contentItem: Item {
        implicitWidth:  control.vertical ? Style.controlHeightL : Style.buttonWidthL
        implicitHeight: control.vertical ? Style.buttonWidthL : Style.controlHeightL

        Rectangle {
            id: track
            anchors.centerIn: parent
            width:  control.vertical ? Math.max(4, parent.width / 8) : parent.width
            height: control.vertical ? parent.height : Math.max(4, parent.height / 8)
            radius: Math.min(width, height) / 2
            color: Style.buttonInactiveColor

            // Filled portion.
            Rectangle {
                radius: parent.radius
                color: Style.imaginToolsAccentColor
                width:  control.vertical ? parent.width : parent.width * control.position
                height: control.vertical ? parent.height * control.position : parent.height
                x: 0
                y: control.vertical ? parent.height - height : 0
            }
        }

        Rectangle {
            id: handle
            width:  Style.controlHeightS
            height: Style.controlHeightS
            radius: width / 2
            color: control.pressed ? Style.buttonPressedColor : Style.backgroundColor
            border.width: Style.buttonBorderWidth
            border.color: Style.imaginToolsAccentColor
            x: control.vertical ? (parent.width - width) / 2
                                : control.position * (parent.width - width)
            y: control.vertical ? (1 - control.position) * (parent.height - height)
                                : (parent.height - height) / 2

            MouseArea {
                id: drag
                anchors.fill: parent
                drag.target: handle
                drag.axis: control.vertical ? Drag.YAxis : Drag.XAxis
                drag.minimumX: 0
                drag.maximumX: control.vertical ? 0 : (control.contentItem.width - handle.width)
                drag.minimumY: 0
                drag.maximumY: control.vertical ? (control.contentItem.height - handle.height) : 0
                onPositionChanged: {
                    if (!drag.active)
                        return;
                    var span = control.vertical ? (control.contentItem.height - handle.height)
                                                : (control.contentItem.width  - handle.width);
                    if (span <= 0)
                        return;
                    var p = control.vertical ? (1 - handle.y / span) : (handle.x / span);
                    control._setFromPosition(p);
                }
            }
        }
    }
}
