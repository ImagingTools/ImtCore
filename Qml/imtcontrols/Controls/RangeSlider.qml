import QtQuick 2.12
import Acf 1.0
import imtcontrols 2.0

/*!
    \qmltype RangeSlider
    \inqmlmodule imtcontrols

    Two-handle range slider, API-compatible with Qt Quick Controls
    \c RangeSlider.

    RangeSlider lets the user pick a sub-range of [\l from, \l to] using two
    handles, \l first and \l second. Each handle exposes its own \c value and
    read-only \c position; dragging a handle updates the corresponding value
    and emits its \c moved() signal.

    \qml
    RangeSlider {
        from: 0; to: 100
        first.value: 20
        second.value: 80
    }
    \endqml

    \sa Slider
*/
Control {
    id: control
    objectName: "ImtControlsRangeSlider"

    /*! Lower bound of the range. */
    property real from: 0
    /*! Upper bound of the range. */
    property real to: 1

    /*! First (lower) handle, exposing \c value, \c position and \c moved(). */
    property alias first: firstNode
    /*! Second (upper) handle, exposing \c value, \c position and \c moved(). */
    property alias second: secondNode

    spacing: 0
    implicitWidth:  Style.buttonWidthL
    implicitHeight: Style.controlHeightL

    QtObject {
        id: firstNode
        property real value: control.from
        readonly property real position: control.to > control.from
            ? (Math.max(control.from, Math.min(control.to, value)) - control.from) / (control.to - control.from) : 0
        signal moved()
    }

    QtObject {
        id: secondNode
        property real value: control.to
        readonly property real position: control.to > control.from
            ? (Math.max(control.from, Math.min(control.to, value)) - control.from) / (control.to - control.from) : 0
        signal moved()
    }

    function _valueFromPosition(p) {
        var clamped = Math.max(0, Math.min(1, p));
        return control.from + clamped * (control.to - control.from);
    }

    contentItem: Item {
        implicitWidth:  Style.buttonWidthL
        implicitHeight: Style.controlHeightL

        Rectangle {
            id: track
            anchors.verticalCenter: parent.verticalCenter
            width: parent.width
            height: Math.max(4, parent.height / 8)
            radius: height / 2
            color: Style.buttonInactiveColor

            // Selected portion between the handles.
            Rectangle {
                radius: parent.radius
                color: Style.imaginToolsAccentColor
                x: firstNode.position * parent.width
                width: (secondNode.position - firstNode.position) * parent.width
                height: parent.height
            }
        }

        Rectangle {
            id: firstHandle
            width:  Style.controlHeightS
            height: Style.controlHeightS
            radius: width / 2
            color: Style.backgroundColor
            border.width: Style.buttonBorderWidth
            border.color: Style.imaginToolsAccentColor
            anchors.verticalCenter: parent.verticalCenter
            x: firstNode.position * (parent.width - width)

            MouseArea {
                anchors.fill: parent
                drag.target: firstHandle
                drag.axis: Drag.XAxis
                drag.minimumX: 0
                drag.maximumX: secondHandle.x
                onPositionChanged: {
                    if (!drag.active)
                        return;
                    var span = parent.parent.width - firstHandle.width;
                    if (span <= 0)
                        return;
                    firstNode.value = control._valueFromPosition(firstHandle.x / span);
                    firstNode.moved();
                }
            }
        }

        Rectangle {
            id: secondHandle
            width:  Style.controlHeightS
            height: Style.controlHeightS
            radius: width / 2
            color: Style.backgroundColor
            border.width: Style.buttonBorderWidth
            border.color: Style.imaginToolsAccentColor
            anchors.verticalCenter: parent.verticalCenter
            x: secondNode.position * (parent.width - width)

            MouseArea {
                anchors.fill: parent
                drag.target: secondHandle
                drag.axis: Drag.XAxis
                drag.minimumX: firstHandle.x
                drag.maximumX: parent.width - secondHandle.width
                onPositionChanged: {
                    if (!drag.active)
                        return;
                    var span = parent.parent.width - secondHandle.width;
                    if (span <= 0)
                        return;
                    secondNode.value = control._valueFromPosition(secondHandle.x / span);
                    secondNode.moved();
                }
            }
        }
    }
}
