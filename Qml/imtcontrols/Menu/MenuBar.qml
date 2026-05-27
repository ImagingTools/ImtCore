import QtQuick 2.12
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

    /*! Default property collects Menu children. */
    default property alias menus: container.data

    /*! Currently visible MenuBarItem index (-1 if none). */
    property int currentIndex: -1

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
        spacing: 2
    }

    // Holder for declared Menu children. We do not render them; we just
    // harvest them and create MenuBarItem proxies in the visible row.
    Item {
        id: container
        visible: false
    }

    Component.onCompleted: _rebuild()

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
    }

    Keys.onPressed: {
        if (!active) return;
        if (event.key === Qt.Key_Right) {
            _openAt((currentIndex + 1) % _items.length);
            event.accepted = true;
        } else if (event.key === Qt.Key_Left) {
            _openAt((currentIndex - 1 + _items.length) % _items.length);
            event.accepted = true;
        }
    }
}
