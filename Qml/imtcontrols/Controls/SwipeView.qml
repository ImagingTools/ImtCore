import QtQuick 2.12
import Acf 1.0
import imtcontrols 2.0

/*!
    \qmltype SwipeView
    \inqmlmodule imtcontrols

    Swipe navigation, API-compatible with Qt Quick Controls \c SwipeView.

    SwipeView lays out its pages (its default children) side by side and
    shows one at a time, selected by \l currentIndex. It extends \l Container,
    so \l count, \l currentItem and \l incrementCurrentIndex() /
    \l decrementCurrentIndex() are available. Pair it with a
    \l PageIndicator for navigation dots.

    \qml
    SwipeView {
        id: view
        Page { }
        Page { }
    }
    PageIndicator {
        count: view.count
        currentIndex: view.currentIndex
    }
    \endqml

    \sa Container, PageIndicator, TabBar
*/
Container {
    id: control
    objectName: "ImtControlsSwipeView"

    clip: true

    onCurrentIndexChanged: _layout()
    onWidthChanged: _layout()
    onHeightChanged: _layout()
    onCountChanged: _layout()
    Component.onCompleted: _layout()

    function _layout() {
        for (var i = 0; i < count; ++i) {
            var it = itemAt(i);
            if (!it)
                continue;
            it.x = 0;
            it.y = 0;
            it.width = width;
            it.height = height;
            it.visible = (i === currentIndex);
        }
    }
}
