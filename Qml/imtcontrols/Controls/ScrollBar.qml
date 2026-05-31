import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

/*!
    \qmltype ScrollBar
    \inqmlmodule imtcontrols

    Interactive scroll bar, API-compatible with Qt Quick Controls
    \c ScrollBar.

    ScrollBar indicates and controls the scrolled position of a
    \l Flickable. \l size is the fraction of the content currently visible
    (0..1) and \l position is the fraction scrolled past (0..1). The handle
    can be dragged, and \l increase() / \l decrease() step by \l stepSize.

    Bind it to a Flickable through the \l flickable convenience property:
    \qml
    Flickable {
        id: flick
        ScrollBar { flickable: flick; anchors.right: parent.right; anchors.top: parent.top; anchors.bottom: parent.bottom }
    }
    \endqml

    \sa ScrollIndicator
*/
Item {
    id: control
    objectName: "ImtControlsScrollBar"

    /*! Fraction of the content that is visible, in [0, 1]. */
    property real size: 0
    /*! Scroll position of the top/left of the visible area, in [0, 1]. */
    property real position: 0
    /*! Step taken by \l increase() / \l decrease(). */
    property real stepSize: 0.1
    /*! Layout direction: \c Qt.Vertical (default) or \c Qt.Horizontal. */
    property int orientation: Qt.Vertical
    readonly property bool vertical: orientation === Qt.Vertical
    /*! Whether the bar is being interacted with or hovered. */
    readonly property bool active: hover.containsMouse || hover.drag.active
    /*! True while the user presses the handle. */
    readonly property bool pressed: hover.drag.active

    /*! Optional Flickable to drive/observe. When set, \l size and
        \l position follow the flickable and dragging scrolls it. */
    property var flickable: null

    width:  vertical ? Style.spacingM : 200
    height: vertical ? 200 : Style.spacingM

    // Track.
    Rectangle {
        anchors.fill: parent
        radius: width < height ? width / 2 : height / 2
        color: Style.backgroundColor
        opacity: control.active ? 0.6 : 0.3
        Behavior on opacity { NumberAnimation { duration: 120 } }
    }

    // Handle.
    Rectangle {
        id: handle
        radius: width < height ? width / 2 : height / 2
        color: control.pressed ? Style.buttonPressedColor
               : control.active ? Style.buttonHoverColor : Style.borderColor2

        width:  control.vertical ? control.width
                                 : Math.max(Style.spacingL, control.width  * control.size)
        height: control.vertical ? Math.max(Style.spacingL, control.height * control.size)
                                 : control.height
        x: control.vertical ? 0 : control.position * (control.width  - width)
        y: control.vertical ? control.position * (control.height - height) : 0

        MouseArea {
            id: hover
            anchors.fill: parent
            hoverEnabled: true
            drag.target: handle
            drag.axis: control.vertical ? Drag.YAxis : Drag.XAxis
            drag.minimumX: 0
            drag.maximumX: control.vertical ? 0 : (control.width  - handle.width)
            drag.minimumY: 0
            drag.maximumY: control.vertical ? (control.height - handle.height) : 0
            onPositionChanged: {
                if (!drag.active)
                    return;
                var span = control.vertical ? (control.height - handle.height)
                                            : (control.width  - handle.width);
                if (span <= 0)
                    return;
                var p = (control.vertical ? handle.y : handle.x) / span;
                control.setPosition(p);
            }
        }
    }

    /*! Increase \l position by \l stepSize. */
    function increase() { setPosition(position + stepSize); }
    /*! Decrease \l position by \l stepSize. */
    function decrease() { setPosition(position - stepSize); }

    function setPosition(p) {
        position = Math.max(0, Math.min(1 - size, p));
        if (flickable)
            _applyToFlickable();
    }

    // ---- Flickable integration -----------------------------------------

    onFlickableChanged: _bindFlickable()

    function _bindFlickable() {
        if (!flickable)
            return;
        _syncFromFlickable();
    }

    function _syncFromFlickable() {
        if (!flickable)
            return;
        if (vertical) {
            var h = flickable.contentHeight;
            if (h > 0) {
                size = Math.min(1, flickable.height / h);
                position = h > flickable.height
                        ? flickable.contentY / h : 0;
            }
        } else {
            var w = flickable.contentWidth;
            if (w > 0) {
                size = Math.min(1, flickable.width / w);
                position = w > flickable.width
                        ? flickable.contentX / w : 0;
            }
        }
    }

    function _applyToFlickable() {
        if (!flickable)
            return;
        if (vertical)
            flickable.contentY = position * flickable.contentHeight;
        else
            flickable.contentX = position * flickable.contentWidth;
    }

    Connections {
        target: control.flickable
        ignoreUnknownSignals: true
        function onContentYChanged() { if (control.vertical) control._syncFromFlickable(); }
        function onContentXChanged() { if (!control.vertical) control._syncFromFlickable(); }
        function onContentHeightChanged() { control._syncFromFlickable(); }
        function onContentWidthChanged() { control._syncFromFlickable(); }
    }
}
