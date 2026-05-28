import QtQuick 2.12
import imtcontrols 1.0
import "Internal" as Internal

/*!
    \qmltype Overlay
    \inqmlmodule imtcontrols

    Convenience accessor for the per-root overlay layer managed by
    OverlayManager. Most code doesn't need to instantiate this directly:
    Popup creates its overlay on demand. Use this when you need a stable
    reference (e.g. for hand-rendered dim layers or visual debug).

    Assign \l anchor to any item in the visual tree; the overlay for the
    topmost ancestor of that item is exposed via \l item.

    \qml
    Overlay { id: ov; anchor: someItem }
    Item { parent: ov.item; ... }
    \endqml
*/
QtObject {
    id: root

    /*! Any item in the visual tree whose root overlay should be exposed. */
    property Item anchor: null

    /*! Lazily resolved overlay Item. Null until \l anchor is set. */
    readonly property Item item: anchor ? Internal.OverlayManager.overlayFor(_rootOf(anchor)) : null

    /*! Number of popups currently open on this overlay. */
    readonly property int popupCount: item ? item.openPopups.length : 0

    function _rootOf(it) {
        var n = it;
        if (!n) return null;
        while (n.parent) n = n.parent;
        return n;
    }
}
