pragma Singleton
import QtQuick 2.12

/*!
    \qmltype InputCoordinator
    \inqmlmodule imtcontrols
    \internal

    Helpers shared by Popup for input handling decisions that don't belong
    to a particular instance: hit-testing parent bounds for the
    \c CloseOnPressOutsideParent policy and grouping mouse press/release into
    close intents according to a popup's \c closePolicy.

    The actual event delivery happens on the overlay's MouseArea (see
    OverlayManager); this singleton only encapsulates the rules.
*/
QtObject {
    id: root

    // Bitmask values for Popup.closePolicy. Mirrors Qt Quick Controls.
    readonly property int noAutoClose:                  0x00
    readonly property int closeOnPressOutside:          0x01
    readonly property int closeOnPressOutsideParent:    0x02
    readonly property int closeOnReleaseOutside:        0x04
    readonly property int closeOnReleaseOutsideParent:  0x08
    readonly property int closeOnEscape:                0x10

    /*! Default policy matches Qt Quick Controls Popup. */
    readonly property int defaultPolicy: closeOnEscape | closeOnPressOutside

    /*! Returns true if \a popup should close given the press event \a mouse
        coming from \a source ("dim"|"catcher"). */
    function shouldClosePress(popup, mouse, source) {
        if (!popup) return false;
        var policy = popup.closePolicy;
        if (policy === noAutoClose) return false;
        var insideParent = _isInsideParent(popup, mouse);
        if ((policy & closeOnPressOutside) && !insideParent && !_isInsidePopup(popup, mouse))
            return true;
        if ((policy & closeOnPressOutsideParent) && !insideParent && !_isInsidePopup(popup, mouse))
            return true;
        return false;
    }

    /*! Returns true if \a popup should close given the release event. */
    function shouldCloseRelease(popup, mouse, source) {
        if (!popup) return false;
        var policy = popup.closePolicy;
        if (policy === noAutoClose) return false;
        var insideParent = _isInsideParent(popup, mouse);
        if ((policy & closeOnReleaseOutside) && !insideParent && !_isInsidePopup(popup, mouse))
            return true;
        if ((policy & closeOnReleaseOutsideParent) && !insideParent && !_isInsidePopup(popup, mouse))
            return true;
        return false;
    }

    function _isInsideParent(popup, mouse) {
        var p = popup.parent;
        if (!p || !popup._overlay) return false;
        var local = popup._overlay.mapToItem(p, mouse.x, mouse.y);
        return local.x >= 0 && local.y >= 0 && local.x <= p.width && local.y <= p.height;
    }

    function _isInsidePopup(popup, mouse) {
        var c = popup._contentRoot;
        if (!c) return false;
        return mouse.x >= c.x && mouse.y >= c.y
                && mouse.x <= c.x + c.width && mouse.y <= c.y + c.height;
    }
}
