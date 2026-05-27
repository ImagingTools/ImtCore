import QtQuick 2.12
import imtcontrols 1.0
import "../Popup" as ImtPopup
import "../Popup/Internal" as PopupInternal
import "Internal" as MenuInternal

/*!
    \qmltype Menu
    \inqmlmodule imtcontrols

    Pop-up menu type, API-compatible with Qt Quick Controls \c Menu.

    Menu extends \l Popup with a vertical list of \l MenuItem rows. Submenus
    are supported by adding nested Menu objects through \l addMenu(): hovering
    or pressing the parent row opens the child Menu to the side, with the
    standard hover-delay behaviour.

    \qml
    Menu {
        MenuItem { text: qsTr("New");  onTriggered: ... }
        MenuItem { text: qsTr("Open"); onTriggered: ... }
        MenuSeparator {}
        Menu {
            title: qsTr("Recent")
            MenuItem { text: "file1.txt" }
            MenuItem { text: "file2.txt" }
        }
    }
    \endqml

    Keyboard navigation:
    \list
      \li Up/Down  - move \l currentIndex, skipping disabled rows and separators
      \li Enter/Return/Space - activate current item
      \li Right    - open submenu (if any)
      \li Left     - close submenu / return to parent
      \li Escape   - close the menu (subject to closePolicy)
    \endlist
*/
ImtPopup.Popup {
    id: menu
    objectName: "ImtControlsPopup"   // intentionally same tag as Popup so
                                     // PopupStackController auto-detects
                                     // ancestor popups uniformly

    // -------------------------------------------------------------------- //
    //                                Public API                             //
    // -------------------------------------------------------------------- //

    /*! Title used when this menu is presented as a submenu row or in MenuBar. */
    property string title: ""

    /*! Currently highlighted row (-1 if none). */
    property int currentIndex: -1

    /*! Whether submenus open as a cascade. Always true here; kept for API
        parity. */
    property bool cascade: true

    /*! Pixels of overlap between a submenu and its parent menu. */
    property int overlap: 1

    /*! Hover delay (ms) before a submenu opens. */
    property int submenuHoverDelay: 250

    /*! Model exposed to the internal ListView. Auto-built from
        \l contentChildren but can also be assigned a user-supplied
        ListModel. */
    property var model: rowsModel

    /*! Per-row delegate. Receives \c model.menuItem. */
    property Component delegate: defaultRowDelegate

    /*! Sensible defaults for a menu: no padding, narrower margins, close
        on outside press or escape. */
    padding: 4
    margins: 4

    // contentItem is overridden to be the ListView; user contentChildren
    // go into Menu's own item array (separated below into rowsModel and
    // submenusList).
    contentItem: list

    // The default content holder is replaced with a ListView, but we still
    // collect the user's declared children so we can scan them for
    // MenuItem / MenuSeparator / nested Menu instances.

    // -------------------------------------------------------------------- //
    //                       Signals over Popup's own                        //
    // -------------------------------------------------------------------- //

    /*! Emitted when any MenuItem in this menu (not a submenu) is triggered. */
    signal triggered(var menuItem)

    // -------------------------------------------------------------------- //
    //                          QQC2-style item API                          //
    // -------------------------------------------------------------------- //

    /*! Number of rows (items + separators + submenus). */
    readonly property int count: rowsModel.count

    function itemAt(index) {
        if (index < 0 || index >= rowsModel.count) return null;
        return rowsModel.get(index).menuItem;
    }

    function addItem(menuItem) {
        if (!menuItem) return;
        rowsModel.append({ "menuItem": menuItem });
    }

    function addSeparator() {
        // Create a MenuSeparator dynamically and store it.
        var sep = Qt.createQmlObject(
            'import imtcontrols 1.0; MenuSeparator {}',
            menu, "Menu.addSeparator");
        rowsModel.append({ "menuItem": sep });
    }

    function addAction(action) {
        var mi = Qt.createQmlObject(
            'import imtcontrols 1.0; MenuItem {}',
            menu, "Menu.addAction");
        mi.action = action;
        addItem(mi);
    }

    function addMenu(sub) {
        // Add a placeholder MenuItem row whose hasSubmenu=true; clicking
        // or hovering opens the nested Menu.
        if (!sub) return;
        submenusList.push(sub);
        var mi = Qt.createQmlObject(
            'import imtcontrols 1.0; MenuItem { hasSubmenu: true }',
            menu, "Menu.addMenu");
        mi.text = sub.title;
        mi.submenu = sub;
        addItem(mi);
    }

    function removeItem(index) {
        if (index < 0 || index >= rowsModel.count) return;
        rowsModel.remove(index);
    }

    function takeItem(index) {
        var it = itemAt(index);
        removeItem(index);
        return it;
    }

    // Convenience to open a menu at a cursor / item.
    function popup(parentItem) {
        if (parentItem) parent = parentItem;
        open();
    }

    // -------------------------------------------------------------------- //
    //                          Default contents scan                        //
    // -------------------------------------------------------------------- //

    // Collect declared children. We intentionally don't override the
    // default property; Popup's contentChildren goes into contentHolder.data
    // and we scan it at completion.
    Component.onCompleted: _ingestDeclaredChildren()

    function _ingestDeclaredChildren() {
        // Find the default content holder. Popup exposes its data slot via
        // contentItem when not overridden, but we've overridden contentItem
        // to `list`. The original children remain accessible through the
        // popup's `data` property (default property of FocusScope/Item).
        var arr = menu.children;
        for (var i = 0; i < arr.length; ++i) {
            var c = arr[i];
            if (!c) continue;
            if (c.isMenuItem === true || c.isMenuSeparator === true) {
                addItem(c);
            } else if (c.isMenuSeparator === true) {
                addItem(c);
            } else if (c.objectName === "ImtControlsPopup" && c !== menu && c.title !== undefined) {
                addMenu(c);
            }
        }
    }

    // -------------------------------------------------------------------- //
    //                              Internal state                           //
    // -------------------------------------------------------------------- //

    /*! ListModel feeding the ListView. */
    ListModel { id: rowsModel }

    /*! Plain JS array of nested Menu refs. */
    property var submenusList: []

    /*! Currently open submenu, if any. */
    property var openSubmenu: null

    // Hover timer for delayed submenu opening.
    Timer {
        id: hoverTimer
        interval: menu.submenuHoverDelay
        repeat: false
        property int pendingIndex: -1
        onTriggered: {
            if (pendingIndex < 0 || pendingIndex >= rowsModel.count) return;
            var mi = rowsModel.get(pendingIndex).menuItem;
            menu._openSubmenu(mi);
        }
    }

    function _onItemHover(index) {
        currentIndex = index;
        var mi = rowsModel.get(index).menuItem;
        if (!mi) return;
        if (mi.hasSubmenu) {
            hoverTimer.pendingIndex = index;
            hoverTimer.restart();
        } else {
            hoverTimer.stop();
            _closeSubmenu();
        }
    }

    function _onItemClick(index) {
        currentIndex = index;
        var mi = rowsModel.get(index).menuItem;
        if (!mi) return;
        if (mi.hasSubmenu) {
            _openSubmenu(mi);
        } else {
            mi.trigger();
            menu.triggered(mi);
            menu.close();
        }
    }

    function _openSubmenu(parentMi) {
        if (!parentMi || !parentMi.submenu) return;
        if (openSubmenu === parentMi.submenu) return;
        if (openSubmenu) openSubmenu.close();
        var sub = parentMi.submenu;
        sub.parentPopup = menu;
        // Position to the right of the current row.
        var rowItem = list.itemAtIndex ? list.itemAtIndex(currentIndex) : null;
        if (rowItem && menu._overlay) {
            var top = rowItem.mapToItem(menu._overlay, 0, 0);
            sub.x = top.x + menu.width - menu.overlap - (menu.parent ? menu.parent.x : 0);
            sub.y = top.y - (menu.parent ? menu.parent.y : 0);
        }
        sub.parent = menu.parent || menu;
        sub.open();
        openSubmenu = sub;
    }

    function _closeSubmenu() {
        if (openSubmenu) {
            openSubmenu.close();
            openSubmenu = null;
        }
    }

    // Skip-disabled-and-separator navigation helpers.
    function _moveCurrent(step) {
        if (rowsModel.count === 0) return;
        var n = rowsModel.count;
        var i = currentIndex;
        for (var tries = 0; tries < n; ++tries) {
            i = (i + step + n) % n;
            var mi = rowsModel.get(i).menuItem;
            if (mi && mi.enabled && !mi.isMenuSeparator) {
                currentIndex = i;
                return;
            }
        }
    }

    function _activateCurrent() {
        if (currentIndex >= 0 && currentIndex < rowsModel.count) {
            _onItemClick(currentIndex);
        }
    }

    Keys.onPressed: function(event) {
        switch (event.key) {
        case Qt.Key_Down:   _moveCurrent(+1); event.accepted = true; return;
        case Qt.Key_Up:     _moveCurrent(-1); event.accepted = true; return;
        case Qt.Key_Right:
            if (currentIndex >= 0) {
                var mi = rowsModel.get(currentIndex).menuItem;
                if (mi && mi.hasSubmenu) { _openSubmenu(mi); event.accepted = true; return; }
            }
            break;
        case Qt.Key_Left:
            if (parentPopup) { close(); event.accepted = true; return; }
            break;
        case Qt.Key_Return:
        case Qt.Key_Enter:
        case Qt.Key_Space:
            _activateCurrent(); event.accepted = true; return;
        }
    }

    onAboutToShow: { currentIndex = -1; }
    onClosed: { _closeSubmenu(); currentIndex = -1; }

    // -------------------------------------------------------------------- //
    //                          Internal contentItem                         //
    // -------------------------------------------------------------------- //

    ListView {
        id: list
        clip: true
        focus: true
        interactive: true
        keyNavigationEnabled: false   // Menu handles keys itself
        model: menu.model
        delegate: Loader {
            width: ListView.view.width
            sourceComponent: menu.delegate
            onLoaded: {
                if (item) {
                    item.menu = menu;
                    if (item.rowIndex === undefined) item.rowIndex = index;
                }
            }
        }
        implicitWidth: {
            var w = 0;
            for (var i = 0; i < contentItem.children.length; ++i) {
                var c = contentItem.children[i];
                if (c && c.implicitWidth > w) w = c.implicitWidth;
            }
            return Math.max(w, 160);
        }
        implicitHeight: contentHeight
    }

    Component {
        id: defaultRowDelegate
        MenuInternal.MenuItemDelegate {}
    }
}
