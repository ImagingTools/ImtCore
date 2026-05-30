import QtQuick 2.12

/*!
    \qmltype PopupAnchors
    \inqmlmodule imtcontrols

    Subset of \c Qt.labs / QQC2 \c Popup.anchors. Attach to a Popup via
    \c { anchors: PopupAnchors { centerIn: parent } }.

    Currently supported:
    \list
      \li \c centerIn - center the popup over the given Item.
    \endlist

    The popup reads these values during repositioning to override the
    default top-left-of-parent placement.
*/
QtObject {
    /*! When set, the popup is centered over this item (typically the
        popup's \c parent or the overlay). */
    property Item centerIn: null
}
