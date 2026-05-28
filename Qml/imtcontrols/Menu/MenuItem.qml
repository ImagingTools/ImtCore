import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

/*!
    \qmltype MenuItem
    \inqmlmodule imtcontrols

    Selectable row inside a \l Menu, API-compatible with Qt Quick Controls
    \c MenuItem.

    A MenuItem is purely a data carrier plus a clickable surface; visual
    decoration (icon, check mark, arrow, mnemonic underline) is rendered by
    the \l Menu's delegate.
*/
Item {
    id: item
    objectName: "ImtControlsMenuItem"

    // ---- public API -----------------------------------------------------

    /*! Display text. Supports an \c & mnemonic marker (e.g. "&File"). */
    property string text: ""

    /*! Optional icon source. */
    property string iconSource: ""

    /*! Optional shortcut text rendered right-aligned (display only; the
        actual shortcut binding belongs on an \l Action). */
    property string shortcut: ""

    /*! Standard button-like state. */
    property bool enabled: true
    property bool checkable: false
    property bool checked: false
    property bool highlighted: false

    /*! Bound Action (optional). When set, \l text, \l iconSource,
        \l enabled, \l checkable and \l checked are kept in sync. */
    property var action: null

    /*! True when this item opens a submenu. Set automatically by Menu when
        a nested Menu is added through addMenu(). */
    property bool hasSubmenu: false

    /*! Optional reference to the submenu Menu (set by parent Menu). */
    property var submenu: null

    /*! Implicit height used by Menu's ListView delegate. */
    property int itemHeight: Style.controlHeightM

    readonly property bool isMenuItem: true

    // ---- signals --------------------------------------------------------

    signal triggered()
    signal toggled()

    function trigger() {
        if (!enabled) return;
        if (checkable) {
            checked = !checked;
            toggled();
            if (action && action.toggle) action.toggle(item);
        }
        triggered();
        if (action && action.trigger) action.trigger(item);
    }

    // ---- Action synchronisation ----------------------------------------

    onActionChanged: {
        _syncFromAction()
    }
    Component.onCompleted: {
        _syncFromAction()
    }

    function _syncFromAction() {
        if (!action) return;
        if (action.text !== undefined && text === "")             text = action.text;
        if (action.enabled !== undefined)                          enabled = action.enabled;
        if (action.checkable !== undefined)                        checkable = action.checkable;
        if (action.checked !== undefined)                          checked = action.checked;
    }

    // The MenuItem itself is invisible; the visual delegate inside Menu
    // owns rendering. We still keep an implicit size for layouts that may
    // measure us.
    width: 0
    height: 0
    visible: false
}
