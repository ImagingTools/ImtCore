import QtQuick 2.12

/*!
    \qmltype ActionGroup
    \inqmlmodule imtcontrols

    Logical grouping of \l Action objects, API-compatible with Qt Quick
    Controls \c ActionGroup.

    Set \l exclusive to true (the default) so that at most one Action in
    the group is \c checked at a time; toggling another Action in the
    group automatically unchecks the previous one.
*/
QtObject {
    id: group

    /*! When true, only one action in the group can be checked. */
    property bool exclusive: true

    /*! When true, group is enabled. Disabling propagates to actions. */
    property bool enabled: true

    /*! Actions belonging to this group. Populate either by setting
        \c Action.group on each action or by appending to this list. */
    property var actions: []

    /*! Currently checked action, or null. */
    readonly property var checkedAction: _checked

    signal triggered(var action)

    property var _checked: null

    function addAction(action) {
        if (!action) return;
        if (actions.indexOf(action) !== -1) return;
        actions.push(action);
        action.group = group;
        action.triggered.connect(function() { group.triggered(action); });
        if (action.toggledChanged) {
            action.toggledChanged.connect(function() { group._syncFrom(action); });
        }
        if (action.checkedChanged) {
            action.checkedChanged.connect(function() { group._syncFrom(action); });
        }
        _syncFrom(action);
    }

    function removeAction(action) {
        var i = actions.indexOf(action);
        if (i === -1) return;
        actions.splice(i, 1);
        if (_checked === action) _checked = null;
    }

    function _syncFrom(action) {
        if (!exclusive) return;
        if (action.checked) {
            if (_checked && _checked !== action) {
                _checked.checked = false;
            }
            _checked = action;
        } else if (_checked === action) {
            _checked = null;
        }
    }
}
