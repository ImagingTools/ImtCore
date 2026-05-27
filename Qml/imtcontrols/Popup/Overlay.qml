import QtQuick 2.12
import imtcontrols 1.0
import "Internal" as Internal

/*!
    \qmltype Overlay
    \inqmlmodule imtcontrols

    Convenience accessor for the per-window overlay layer managed by
    OverlayManager. Most code doesn't need to instantiate this directly:
    Popup creates its overlay on demand. Use this when you need a stable
    reference (e.g. for hand-rendered dim layers or visual debug).

    \qml
    Overlay { id: ov; window: Window.window }
    Item { parent: ov.item; ... }
    \endqml
*/
QtObject {
    id: root

    /*! The Window whose overlay should be exposed. */
    property var window: null

    /*! Lazily resolved overlay Item. Null until \l window is set. */
    readonly property Item item: window ? Internal.OverlayManager.overlayFor(window) : null

    /*! Number of popups currently open on this overlay. */
    readonly property int popupCount: item ? item.openPopups.length : 0
}
