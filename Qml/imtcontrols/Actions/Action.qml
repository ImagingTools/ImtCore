import QtQuick 2.12

Shortcut {
    id: baseButton;

    property Image icon
    property string text: "";

    property bool enabled: true;
    property bool checkable: false;
    property bool checked: false;

    signal triggered(QtObject source);
    signal toggled(QtObject source);

    function toggle(source) { toggled(source) }
    function trigger(source) { triggered(source) }
}
