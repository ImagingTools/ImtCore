import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

/*!
    \qmltype MenuBar
    \inqmlmodule imtcontrols

    Horizontal strip of \l MenuBarItem entries, each presenting a \l Menu
    when activated. API-compatible with Qt Quick Controls \c MenuBar.

    \qml
    MenuBar {
        Menu { title: qsTr("&File"); MenuItem { text: qsTr("Quit") } }
        Menu { title: qsTr("&Edit"); MenuItem { text: qsTr("Undo") } }
    }
    \endqml

    Behaviour:
    \list
      \li Click on an item toggles its menu.
      \li When any menu is open, hovering a sibling item switches to that
          menu without an extra click.
      \li Left/Right arrows cycle between items while a menu is open.
      \li Closing the active menu deactivates the bar (the Escape key
          inside the menu also closes the menu and returns focus to the
          bar item).
    \endlist
*/
FocusScope {
    id: bar
    objectName: "ImtControlsMenuBar"

    activeFocusOnTab: true

    /*! Default property collects Menu children. */
    default property alias menus: container.data

    /*! Currently visible MenuBarItem index (-1 if none). */
    property int currentIndex: -1

    /*! Index that the keyboard cursor is on. Independent from
        \l currentIndex, which tracks the currently-open menu. The visual
        focus highlight on bar items follows this index when the bar has
        keyboard focus. */
    property int focusedIndex: -1

    /*! True when at least one menu in this bar is open. */
    readonly property bool active: currentIndex >= 0 && currentIndex < _items.length
                                    && _items[currentIndex].menu
                                    && _items[currentIndex].menu.opened

    /*! Generated MenuBarItem proxies, one per added Menu. */
    property var _items: []

    implicitHeight: row.implicitHeight
    implicitWidth: row.implicitWidth
    height: implicitHeight

    Row {
        id: row
        anchors.left: parent.left
        anchors.top: parent.top
        spacing: Style.spacingXXS
    }

    // Holder for declared Menu children. We do not render them; we just
    // harvest them and create MenuBarItem proxies in the visible row.
    Item {
        id: container
        visible: false
    }

    Component.onCompleted: {
        _rebuild()
    }

    function _rebuild() {
        // Destroy any pre-existing items.
        for (var i = 0; i < _items.length; ++i) {
            try { _items[i].destroy(); } catch (e) {}
        }
        _items = [];
        // Walk container children for Menus.
        var arr = container.children;
        for (var j = 0; j < arr.length; ++j) {
            var c = arr[j];
            if (c && c.title !== undefined && c.objectName === "ImtControlsPopup") {
                _addMenu(c);
            }
        }
    }

    function _addMenu(menu) {
        var proxy = Qt.createQmlObject(
            'import imtcontrols 1.0; MenuBarItem {}',
            row, "MenuBar._addMenu");
        proxy.text = menu.title;
        proxy.menu = menu;
        proxy.menuBar = bar;
        proxy.barIndex = _items.length;
        _items.push(proxy);
        menu.parent = proxy;
        // When the menu closes (e.g. via Escape or outside click), keep the
        // keyboard cursor on its bar item and pull focus back to the bar so
        // arrow keys keep working without an extra Tab.
        menu.closed.connect(function() { bar._onMenuClosed(proxy.barIndex); });
    }

    /*! Add a menu programmatically. */
    function addMenu(menu) { _addMenu(menu); }

    function _toggleAt(index) {
        if (index < 0 || index >= _items.length) return;
        var it = _items[index];
        if (!it || !it.menu) return;
        if (it.menu.opened) {
            it.menu.close();
            currentIndex = -1;
        } else {
            _openAt(index);
        }
    }

    function _onHover(index) {
        if (!active) return;
        if (currentIndex === index) return;
        _openAt(index);
    }

    function _onMenuClosed(index) {
        if (currentIndex === index) currentIndex = -1;
        // If our bar still owns the focus chain, keep the visual cursor on
        // the item the user just dismissed.
        if (bar.activeFocus && focusedIndex < 0)
            focusedIndex = index;
    }

    function _focusFirstEnabled() {
        for (var i = 0; i < _items.length; ++i) {
            if (_items[i] && _items[i].menu) {
                focusedIndex = i;
                return;
            }
        }
    }

    function _moveFocus(step) {
        if (_items.length === 0) return;
        var n = _items.length;
        var i = focusedIndex < 0 ? (step > 0 ? -1 : 0) : focusedIndex;
        for (var tries = 0; tries < n; ++tries) {
            i = (i + step + n) % n;
            if (_items[i] && _items[i].menu) {
                if (active) {
                    _openAt(i);
                } else {
                    focusedIndex = i;
                }
                return;
            }
        }
    }

    function _openAt(index) {
        // Close currently open one first.
        if (currentIndex >= 0 && currentIndex < _items.length) {
            var cur = _items[currentIndex];
            if (cur && cur.menu && cur.menu.opened) cur.menu.close();
        }
        var it = _items[index];
        if (!it || !it.menu) return;
        it.menu.x = 0;
        it.menu.y = it.height;
        it.menu.parent = it;
        it.menu.open();
        currentIndex = index;
        focusedIndex = index;
    }

    onActiveFocusChanged: {
        if (activeFocus && focusedIndex < 0 && !active)
            _focusFirstEnabled();
    }

    Keys.onPressed: {
        if (_items.length === 0) return;
        switch (event.key) {
        case Qt.Key_Right:
            _moveFocus(+1); event.accepted = true; return;
        case Qt.Key_Left:
            _moveFocus(-1); event.accepted = true; return;
        case Qt.Key_Home:
            focusedIndex = -1; _moveFocus(+1); event.accepted = true; return;
        case Qt.Key_End:
            focusedIndex = -1; _moveFocus(-1); event.accepted = true; return;
        case Qt.Key_Down:
        case Qt.Key_Return:
        case Qt.Key_Enter:
        case Qt.Key_Space:
            if (!active && focusedIndex >= 0) {
                _openAt(focusedIndex);
                event.accepted = true;
                return;
            }
            break;
        case Qt.Key_Escape:
            if (active) {
                _items[currentIndex].menu.close();
                event.accepted = true;
                return;
            }
            break;
        }
    }
}
