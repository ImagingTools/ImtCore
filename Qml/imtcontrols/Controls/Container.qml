import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

/*!
    \qmltype Container
    \inqmlmodule imtcontrols

    Abstract container of items, API-compatible with Qt Quick Controls
    \c Container.

    Container is the non-visual base type used to collect a set of child
    items and expose a \l currentIndex / \l currentItem selection on top of
    them. It does not lay the items out itself; concrete containers such as
    \l SwipeView or \l TabBar position the items and decide how the current
    item is presented.

    \list
      \li \l count is the number of contained items.
      \li \l currentIndex selects one of them (\c -1 when empty).
      \li \l currentItem is the item at \l currentIndex.
      \li \l itemAt() returns the item at a given index.
      \li \l incrementCurrentIndex() / \l decrementCurrentIndex() move the
          selection.
    \endlist

    \qml
    Container {
        id: container
        Item { }
        Item { }
    }
    \endqml

    \sa SwipeView, TabBar
*/
Item {
    id: container
    objectName: "ImtControlsContainer"

    /*! Items declared inside the container. \c contentData is the default
        property so child items populate the internal holder. */
    default property alias contentData: contentHolder.data

    /*! Number of items in the container. */
    readonly property int count: contentHolder.children.length

    /*! Index of the currently selected item (\c -1 when empty). */
    property int currentIndex: count > 0 ? 0 : -1

    /*! The item at \l currentIndex (\c null when empty). */
    readonly property Item currentItem: itemAt(currentIndex)

    /*! Spacing hint forwarded to concrete subtypes. */
    property real spacing: 0

    /*! Returns the item at \a index, or \c null when out of range. */
    function itemAt(index) {
        if (index < 0 || index >= contentHolder.children.length)
            return null;
        return contentHolder.children[index];
    }

    /*! Selects the next item, clamped to the last one. */
    function incrementCurrentIndex() {
        if (currentIndex < count - 1)
            currentIndex = currentIndex + 1;
    }

    /*! Selects the previous item, clamped to the first one. */
    function decrementCurrentIndex() {
        if (currentIndex > 0)
            currentIndex = currentIndex - 1;
    }

    // Holder collecting the contained items; subtypes read its children.
    Item {
        id: contentHolder
        anchors.fill: parent
    }
}
