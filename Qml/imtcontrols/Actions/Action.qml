import QtQuick 2.12

Shortcut {
    id: baseButton;

    property Image icon
    property string text: "";

    property bool enabled: true;
    property bool checkable: false;
    property bool checked: false;

    // ActionGroup membership. Setting this auto-registers with the group
    // (kept loose-typed for backward compatibility with existing call sites).
    property var group: null

    signal triggered(QtObject source);
    signal toggled(QtObject source);

    function toggle(source){
        if (checkable) {
            checked = !checked;
        }
        toggled(source)
    }
    function trigger(source){
        // Note: kept call-compatible with previous versions; toggling is
        // the caller's responsibility via toggle() unless an ActionGroup
        // manages this Action.
        triggered(source)
    }

    onGroupChanged: {
        if (group && group.addAction && group.actions
                && group.actions.indexOf(baseButton) === -1) {
            group.addAction(baseButton);
        }
    }
}
