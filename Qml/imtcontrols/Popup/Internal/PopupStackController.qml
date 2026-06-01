pragma Singleton
import QtQuick 2.12

/*!
    \qmltype PopupStackController
    \inqmlmodule imtcontrols
    \internal

    Maintains parent/child relationships between popups (submenu chains and
    other "nested popup" patterns) and enforces the cascade-close behaviour
    expected by Qt Quick Controls Menu.

    Hierarchy rules:
      * Closing a parent popup recursively closes all of its descendants.
      * Pressing/clicking on a popup in the middle of a chain closes every
        popup above it.
      * A child popup never outlives its parent.
*/
QtObject {
    id: root

    // Parallel arrays: parents[i] is parent popup, children[i] is array of
    // direct children. Using arrays keeps everything immutable-friendly.
    property var _parents: []
    property var _children: []

    function _indexOf(p) { return _parents.indexOf(p); }

    /*! Register \a child as a sub-popup of \a parent. */
    function attach(parent, child) {
        if (!parent || !child || parent === child) return;
        var i = _indexOf(parent);
        if (i === -1) {
            _parents.push(parent);
            _children.push([child]);
        } else if (_children[i].indexOf(child) === -1) {
            _children[i].push(child);
        }
    }

    /*! Unregister \a child from its parent (no-op if not attached). */
    function detach(child) {
        for (var i = 0; i < _children.length; ++i) {
            var idx = _children[i].indexOf(child);
            if (idx !== -1) _children[i].splice(idx, 1);
        }
    }

    /*! Direct children of \a popup. */
    function children(popup) {
        var i = _indexOf(popup);
        return i === -1 ? [] : _children[i].slice();
    }

    /*! Recursively close every descendant of \a popup. */
    function closeDescendants(popup) {
        var i = _indexOf(popup);
        if (i === -1) return;
        var ch = _children[i].slice();
        for (var j = 0; j < ch.length; ++j) {
            var c = ch[j];
            try {
                closeDescendants(c);
                if (c && c.opened) c.close();
            } catch (e) { /* destroyed */ }
        }
    }

    /*! Close every popup whose z is strictly greater than \a popup.z within
        the same overlay (used to collapse a partially-open chain when the
        user clicks an item lower in the stack). */
    function closeAbove(popup) {
        if (!popup) return;
        // Walk all known parent records and close any descendant chain
        // belonging to the same overlay whose z is above the given one.
        for (var i = 0; i < _parents.length; ++i) {
            var ch = _children[i];
            for (var j = 0; j < ch.length; ++j) {
                var c = ch[j];
                try {
                    if (c && c.opened && c.z > popup.z
                            && c._overlay === popup._overlay) {
                        closeDescendants(c);
                        c.close();
                    }
                } catch (e) {}
            }
        }
    }

    /*! Returns true if \a candidate is a (transitive) descendant of \a popup. */
    function isDescendant(popup, candidate) {
        var i = _indexOf(popup);
        if (i === -1) return false;
        var ch = _children[i];
        for (var j = 0; j < ch.length; ++j) {
            if (ch[j] === candidate) return true;
            if (isDescendant(ch[j], candidate)) return true;
        }
        return false;
    }
}
