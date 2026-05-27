pragma Singleton
import QtQuick 2.12

/*!
    \qmltype PopupManager
    \inqmlmodule imtcontrols
    \internal

    Global registry of every live imtcontrols Popup. Provides cross-cutting
    services that any popup may need (broadcast of window resize, screen
    changes, language updates) and a single place to clean up orphan popups
    when their owning window is destroyed.

    Popups register themselves with \l register() during \c Component.onCompleted
    and unregister via \l unregister() in \c Component.onDestruction. The
    registry is intentionally kept as a plain JS array of weak references
    (raw QML object refs); destroyed objects are pruned lazily by \l prune().
*/
QtObject {
    id: root

    // ---- registry ----------------------------------------------------------

    property var popups: []          // live Popup items
    property int generation: 0       // bumped on every (un)register; used as a binding trigger

    function register(popup) {
        if (!popup) return;
        if (popups.indexOf(popup) !== -1) return;
        popups.push(popup);
        generation = generation + 1;
    }

    function unregister(popup) {
        if (!popup) return;
        var idx = popups.indexOf(popup);
        if (idx !== -1) {
            popups.splice(idx, 1);
            generation = generation + 1;
        }
    }

    function prune() {
        // Remove refs whose QML object has been destroyed. We detect this
        // by reading any always-present property and catching the resulting
        // exception.
        var kept = [];
        for (var i = 0; i < popups.length; ++i) {
            var p = popups[i];
            try {
                if (p && p.objectName !== undefined) {
                    kept.push(p);
                }
            } catch (e) {
                // destroyed - drop
            }
        }
        if (kept.length !== popups.length) {
            popups = kept;
            generation = generation + 1;
        }
    }

    // ---- broadcasts --------------------------------------------------------

    /*! Notify all open popups about a window geometry change so positioning
        can be recomputed. */
    signal windowGeometryChanged()

    /*! Notify all open popups about a language/locale change. */
    signal localeChanged()

    function notifyWindowGeometryChanged() { windowGeometryChanged(); }
    function notifyLocaleChanged() { localeChanged(); }

    // ---- queries -----------------------------------------------------------

    /*! Returns the topmost open popup that lives inside \a window, or null. */
    function topOpenPopup(window) {
        var top = null;
        for (var i = 0; i < popups.length; ++i) {
            var p = popups[i];
            try {
                if (!p || !p.opened) continue;
                if (window && p.window !== window) continue;
                if (!top || (p.z >= top.z)) top = p;
            } catch (e) { /* destroyed */ }
        }
        return top;
    }

    /*! Returns true if any modal popup lives inside \a window. */
    function hasModal(window) {
        for (var i = 0; i < popups.length; ++i) {
            var p = popups[i];
            try {
                if (p && p.opened && p.modal && (!window || p.window === window))
                    return true;
            } catch (e) { /* destroyed */ }
        }
        return false;
    }
}
