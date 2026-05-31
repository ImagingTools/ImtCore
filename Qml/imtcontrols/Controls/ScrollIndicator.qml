import QtQuick 2.12
import Acf 1.0
import imtcontrols 2.0

/*!
    \qmltype ScrollIndicator
    \inqmlmodule imtcontrols

    Non-interactive scroll indicator, API-compatible with Qt Quick Controls
    \c ScrollIndicator.

    ScrollIndicator visualises the scrolled position of a \l Flickable
    without allowing the user to drag it. \l size is the visible fraction of
    the content (0..1) and \l position the scrolled fraction (0..1). It fades
    in while the flickable is moving and fades out when idle.

    \qml
    Flickable {
        id: flick
        ScrollIndicator { flickable: flick; anchors.right: parent.right; anchors.top: parent.top; anchors.bottom: parent.bottom }
    }
    \endqml

    \sa ScrollBar
*/
Item {
    id: control
    objectName: "ImtControlsScrollIndicator"

    /*! Fraction of the content that is visible, in [0, 1]. */
    property real size: 0
    /*! Scrolled fraction of the scrollable distance, in [0, 1]. */
    property real position: 0
    /*! Layout direction: \c Qt.Vertical (default) or \c Qt.Horizontal. */
    property int orientation: Qt.Vertical
    readonly property bool vertical: orientation === Qt.Vertical
    /*! True while the indicator is visible (flickable moving). */
    property bool active: false

    /*! Optional Flickable to observe. */
    property var flickable: null

    width:  vertical ? Style.spacingS : 200
    height: vertical ? 200 : Style.spacingS

    opacity: active ? 0.75 : 0.0
    Behavior on opacity { NumberAnimation { duration: 250 } }

    Rectangle {
        id: handle
        radius: width < height ? width / 2 : height / 2
        color: Style.scrollIndicatorColor

        width:  control.vertical ? control.width
                                 : Math.max(Style.spacingL, control.width  * control.size)
        height: control.vertical ? Math.max(Style.spacingL, control.height * control.size)
                                 : control.height
        x: control.vertical ? 0 : control.position * (control.width  - width)
        y: control.vertical ? control.position * (control.height - height) : 0
    }

    onFlickableChanged: _syncFromFlickable()

    function _syncFromFlickable() {
        if (!flickable)
            return;
        if (vertical) {
            var h = flickable.contentHeight;
            var vh = flickable.height;
            if (h > 0) {
                size = Math.min(1, vh / h);
                position = h > vh ? flickable.contentY / (h - vh) : 0;
            }
        } else {
            var w = flickable.contentWidth;
            var vw = flickable.width;
            if (w > 0) {
                size = Math.min(1, vw / w);
                position = w > vw ? flickable.contentX / (w - vw) : 0;
            }
        }
    }

    Connections {
        target: control.flickable
        ignoreUnknownSignals: true
        function onContentYChanged() { if (control.vertical) control._syncFromFlickable(); }
        function onContentXChanged() { if (!control.vertical) control._syncFromFlickable(); }
        function onContentHeightChanged() { control._syncFromFlickable(); }
        function onContentWidthChanged() { control._syncFromFlickable(); }
        function onMovingChanged() { control.active = control.flickable.moving; }
    }
}
