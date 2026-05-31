import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

/*!
    \qmltype Page
    \inqmlmodule imtcontrols

    Page container with optional header and footer, API-compatible with Qt
    Quick Controls \c Page.

    Page lays out an optional \l header at the top, an optional \l footer at
    the bottom and the page content (its default children) in between. It is
    commonly used together with \l StackView and \l SwipeView, with a
    \l ToolBar as the header.

    \qml
    Page {
        title: qsTr("Inbox")
        header: ToolBar { Label { text: qsTr("Inbox") } }
        Column { ... }
    }
    \endqml

    \sa ToolBar, StackView, SwipeView
*/
Item {
    id: page
    objectName: "ImtControlsPage"

    /*! Page title (informational; rendered by the header if desired). */
    property string title: ""

    /*! Item docked at the top (e.g. a \l ToolBar). */
    property Item header: null
    /*! Item docked at the bottom (e.g. a \l ToolBar footer). */
    property Item footer: null

    /*! Padding around the content area (between header and footer). */
    property real padding: 0

    /*! User content placed in the central area. */
    default property alias contentData: contentHolder.data

    width: Style.startWindowWidth
    height: Style.startWindowHeight

    Rectangle {
        anchors.fill: parent
        color: Style.backgroundColor2
    }

    onHeaderChanged: _placeHeader()
    onFooterChanged: _placeFooter()
    Component.onCompleted: { _placeHeader(); _placeFooter(); }

    function _placeHeader() {
        if (!header)
            return;
        header.parent = headerSlot;
        header.anchors.fill = headerSlot;
    }
    function _placeFooter() {
        if (!footer)
            return;
        footer.parent = footerSlot;
        footer.anchors.fill = footerSlot;
    }

    Item {
        id: headerSlot
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: page.header ? page.header.implicitHeight : 0
    }

    Item {
        id: footerSlot
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: page.footer ? page.footer.implicitHeight : 0
    }

    Item {
        id: contentHolder
        anchors.top: headerSlot.bottom
        anchors.bottom: footerSlot.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: page.padding
    }
}
