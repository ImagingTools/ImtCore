pragma Singleton
import QtQuick 2.12

/*!
    \qmltype FocusCoordinator
    \inqmlmodule imtcontrols
    \internal

    Saves and restores keyboard focus around popup lifecycles. Maintains a
    per-window stack of prior \c activeFocusItem references so deeply nested
    popups can each restore their own caller on close.
*/
QtObject {
    id: root

    // Map window -> array of saved focus items (most recent at end).
    property var _windows: []
    property var _stacks: []

    function _stackFor(window, createIfMissing) {
        if (!window) return null;
        var idx = _windows.indexOf(window);
        if (idx === -1) {
            if (!createIfMissing) return null;
            _windows.push(window);
            _stacks.push([]);
            return _stacks[_stacks.length - 1];
        }
        return _stacks[idx];
    }

    /*! Push current \a window.activeFocusItem onto the stack. */
    function push(window) {
        if (!window) return;
        var stack = _stackFor(window, true);
        stack.push(window.activeFocusItem || null);
    }

    /*! Pop and restore focus on \a window. If \a fallback is provided and
        the originally saved item is gone, fallback is focused instead. */
    function pop(window, fallback) {
        if (!window) return;
        var stack = _stackFor(window, false);
        if (!stack || stack.length === 0) {
            if (fallback) fallback.forceActiveFocus();
            return;
        }
        var prev = stack.pop();
        try {
            if (prev && prev.forceActiveFocus) {
                prev.forceActiveFocus();
                return;
            }
        } catch (e) { /* prev was destroyed */ }
        if (fallback) fallback.forceActiveFocus();
    }

    /*! Discard the topmost saved focus without restoring (used when the
        caller explicitly does not want focus restoration). */
    function discard(window) {
        var stack = _stackFor(window, false);
        if (stack && stack.length > 0) stack.pop();
    }

    /*! Returns true if \a item is a (transitive) child of \a root. */
    function isAncestor(rootItem, item) {
        var n = item;
        while (n) {
            if (n === rootItem) return true;
            n = n.parent;
        }
        return false;
    }
}
