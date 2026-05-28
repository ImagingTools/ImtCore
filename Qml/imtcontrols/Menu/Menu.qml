import QtQuick 2.12
import Acf 1.0
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
    padding: Style.spacingXS
    margins: Style.spacingXS

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
    Component.onCompleted: {
        _ingestDeclaredChildren()
    }

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
        // When nothing is selected yet, position the cursor just before the
        // first row (for Down) or just after the last row (for Up) so the
        // first iteration lands on the first/last enabled entry, matching
        // QQC2 Menu behaviour.
        var i;
        if (currentIndex < 0)
            i = step > 0 ? -1 : 0;
        else
            i = currentIndex;
        for (var tries = 0; tries < n; ++tries) {
            i = (i + step + n) % n;
            var mi = rowsModel.get(i).menuItem;
            if (mi && mi.enabled && !mi.isMenuSeparator) {
                currentIndex = i;
                return;
            }
        }
    }

    function _moveToEdge(forward) {
        if (rowsModel.count === 0) return;
        var n = rowsModel.count;
        if (forward) {
            for (var i = 0; i < n; ++i) {
                var mi = rowsModel.get(i).menuItem;
                if (mi && mi.enabled && !mi.isMenuSeparator) {
                    currentIndex = i;
                    return;
                }
            }
        } else {
            for (var j = n - 1; j >= 0; --j) {
                var mj = rowsModel.get(j).menuItem;
                if (mj && mj.enabled && !mj.isMenuSeparator) {
                    currentIndex = j;
                    return;
                }
            }
        }
    }

    function _activateCurrent() {
        if (currentIndex >= 0 && currentIndex < rowsModel.count) {
            _onItemClick(currentIndex);
        }
    }

    // Strip mnemonic markers ("&File" -> "File", "&&" -> "&") and return
    // the first character (preferring the mnemonic, if any).
    function _mnemonicChar(text) {
        if (!text) return "";
        var s = String(text);
        var m = s.match(/(^|[^&])&([^&])/);
        if (m && m[2]) return m[2].toLowerCase();
        var stripped = s.replace(/&&/g, "\u0001").replace(/&(.)/g, "$1").replace(/\u0001/g, "&");
        return stripped.length > 0 ? stripped.charAt(0).toLowerCase() : "";
    }

    // Letter-key navigation. Selects the next enabled row whose label starts
    // with \a ch; activates it immediately if it's the only match.
    function _searchByLetter(ch) {
        if (!ch || rowsModel.count === 0) return false;
        var n = rowsModel.count;
        var start = currentIndex >= 0 ? currentIndex + 1 : 0;
        var matches = 0;
        var firstMatch = -1;
        ch = ch.toLowerCase();
        for (var k = 0; k < n; ++k) {
            var i = (start + k) % n;
            var mi = rowsModel.get(i).menuItem;
            if (!mi || !mi.enabled || mi.isMenuSeparator) continue;
            if (_mnemonicChar(mi.text) === ch) {
                if (firstMatch === -1) firstMatch = i;
                matches++;
            }
        }
        if (firstMatch === -1) return false;
        currentIndex = firstMatch;
        if (matches === 1)
            _activateCurrent();
        return true;
    }

    // Key handling helpers. Invoked from list.Keys.onPressed because the
    // ListView is the actual focused descendant after the popup reparents
    // its contentItem onto the overlay's content root.
    function _handleKeyPress(event) {
        switch (event.key) {
        case Qt.Key_Down:
            _moveCurrent(+1); event.accepted = true; return;
        case Qt.Key_Up:
            _moveCurrent(-1); event.accepted = true; return;
        case Qt.Key_Home:
        case Qt.Key_PageUp:
            _moveToEdge(true); event.accepted = true; return;
        case Qt.Key_End:
        case Qt.Key_PageDown:
            _moveToEdge(false); event.accepted = true; return;
        case Qt.Key_Right:
            if (currentIndex >= 0) {
                var mi = rowsModel.get(currentIndex).menuItem;
                if (mi && mi.hasSubmenu) { _openSubmenu(mi); event.accepted = true; return; }
            }
            break;
        case Qt.Key_Left:
            if (parentPopup) {
                // Restore focus to the parent menu before tearing ourselves
                // down so its keyboard navigation continues seamlessly.
                if (parentPopup._contentRoot)
                    parentPopup._contentRoot.forceActiveFocus();
                close();
                event.accepted = true;
                return;
            }
            break;
        case Qt.Key_Return:
        case Qt.Key_Enter:
        case Qt.Key_Space:
            _activateCurrent(); event.accepted = true; return;
        }
        // Letter-key search: accept printable single-character text.
        if (event.text && event.text.length === 1) {
            var c = event.text.charCodeAt(0);
            if (c >= 0x20 && c !== 0x7F && _searchByLetter(event.text)) {
                event.accepted = true;
                return;
            }
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
        Keys.onPressed: {
            menu._handleKeyPress(event)
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
