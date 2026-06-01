import QtQuick 2.12
import Acf 1.0
import imtcontrols 2.0

/*!
    \qmltype StackView
    \inqmlmodule imtcontrols

    Stack-based navigation, API-compatible with Qt Quick Controls
    \c StackView.

    StackView manages a last-in/first-out stack of pages: \l push() adds a
    page on top, \l pop() removes the top one, \l replace() swaps it and
    \l clear() empties the stack. The top page (\l currentItem) fills the
    view. Pages may be supplied as \c Component, \c Item or source URL.

    \qml
    StackView {
        id: stack
        initialItem: homePage
    }
    Button { text: qsTr("Details"); onClicked: stack.push(detailsPage) }
    \endqml

    \sa SwipeView
*/
Item {
    id: stack
    objectName: "ImtControlsStackView"

    /*! Page pushed automatically when the view is created. */
    property var initialItem: null

    /*! Number of pages on the stack (read-only). */
    readonly property int depth: _stack.length

    /*! The page currently on top (read-only). */
    readonly property Item currentItem: depth > 0 ? _stack[depth - 1] : null

    /*! Always \c false in this re-implementation (no transition animations). */
    readonly property bool busy: false

    property var _stack: []

    implicitWidth:  Style.buttonWidthL
    implicitHeight: Style.controlHeightL

    Item {
        id: holder
        anchors.fill: parent
    }

    Component.onCompleted: {
        if (initialItem)
            push(initialItem);
    }

    /*! Push \a item (Component, Item or url) on top, optionally applying
        \a props, and make it current. */
    function push(item, props) {
        var obj = _resolve(item, props);
        if (!obj)
            return null;
        _stack.push(obj);
        _depthChanged();
        _showTop();
        return obj;
    }

    /*! Remove the top page and return the new current item. */
    function pop() {
        if (_stack.length <= 1)
            return null;
        var removed = _stack.pop();
        if (removed)
            removed.visible = false;
        _depthChanged();
        _showTop();
        return currentItem;
    }

    /*! Replace the top page with \a item. */
    function replace(item, props) {
        if (_stack.length > 0) {
            var removed = _stack.pop();
            if (removed)
                removed.visible = false;
        }
        return push(item, props);
    }

    /*! Remove every page from the stack. */
    function clear() {
        for (var i = 0; i < _stack.length; ++i) {
            if (_stack[i])
                _stack[i].visible = false;
        }
        _stack = [];
        _depthChanged();
    }

    function _resolve(item, props) {
        if (!item)
            return null;
        var comp = item;
        if (typeof item === "string")
            comp = Qt.createComponent(item);
        if (comp && comp.createObject !== undefined) {
            var created = props ? comp.createObject(holder, props)
                                : comp.createObject(holder);
            return created;
        }
        // Assume it is already an Item.
        item.parent = holder;
        return item;
    }

    function _showTop() {
        for (var i = 0; i < _stack.length; ++i) {
            var it = _stack[i];
            if (!it)
                continue;
            it.parent = holder;
            it.anchors.fill = holder;
            it.visible = (i === _stack.length - 1);
        }
    }

    // Notify bindings that depend on the private stack length.
    function _depthChanged() {
        _stack = _stack;
    }
}
