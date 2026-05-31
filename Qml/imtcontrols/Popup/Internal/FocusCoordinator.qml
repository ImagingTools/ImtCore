pragma Singleton
import QtQuick 2.12

/*!
    \qmltype FocusCoordinator
    \inqmlmodule imtcontrols
    \internal

    Saves and restores keyboard focus around popup lifecycles. Maintains a
    per-root-item stack of focus owners so deeply nested popups can each
    restore their own caller on close.
*/
QtObject {
    id: root

    // Map rootItem -> array of saved focus items (most recent at end).
    property var _roots: []
    property var _stacks: []

    function _stackFor(rootItem, createIfMissing) {
        if (!rootItem) return null;
        var idx = _roots.indexOf(rootItem);
        if (idx === -1) {
            if (!createIfMissing) return null;
            _roots.push(rootItem);
            _stacks.push([]);
            return _stacks[_stacks.length - 1];
        }
        return _stacks[idx];
    }

    /*! Push \a focusOwner (typically the item that opened the popup) onto
        the stack for \a rootItem. */
    function push(rootItem, focusOwner) {
        if (!rootItem) return;
        var stack = _stackFor(rootItem, true);
        stack.push(focusOwner || null);
    }

    /*! Pop and restore focus on \a rootItem. If \a fallback is provided
        and the originally saved item is gone, fallback is focused instead. */
    function pop(rootItem, fallback) {
        if (!rootItem) return;
        var stack = _stackFor(rootItem, false);
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
    function discard(rootItem) {
        var stack = _stackFor(rootItem, false);
        if (stack && stack.length > 0) stack.pop();
    }

    /*! Returns true if \a item is a (transitive) child of \a rootItem. */
    function isAncestor(rootItem, item) {
        var n = item;
        while (n) {
            if (n === rootItem) return true;
            n = n.parent;
        }
        return false;
    }
}
