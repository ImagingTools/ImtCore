import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

/*!
    \qmltype ApplicationWindow
    \inqmlmodule imtcontrols

    Top-level application surface, API-compatible with Qt Quick Controls
    \c ApplicationWindow.

    ApplicationWindow provides the conventional application layout: an
    optional \l header docked at the top, an optional \l footer docked at the
    bottom and the page content (its default children) in between. It is built
    from a plain \c Item (no \c QtQuick.Window dependency) so it can be hosted
    inside any window surface provided by the application. The background is
    themed through the application \c Style singleton.

    \qml
    ApplicationWindow {
        visible: true
        header: ToolBar { Label { text: qsTr("My App") } }
        Page { }
    }
    \endqml

    \sa Page, ToolBar
*/
Item {
    id: appWindow
    objectName: "ImtControlsApplicationWindow"

    /*! Item docked at the top of the window (e.g. a \l ToolBar). */
    property Item header: null
    /*! Item docked at the bottom of the window (e.g. a \l ToolBar footer). */
    property Item footer: null

    /*! Surface background color, themed through the \c Style singleton. */
    property color color: Style.backgroundColor2

    /*! User content placed in the central area between header and footer. */
    default property alias contentData: contentHolder.data

    width: Style.startWindowWidth
    height: Style.startWindowHeight

    onHeaderChanged: {
        _placeHeader()
    }
    onFooterChanged: {
        _placeFooter()
    }
    Component.onCompleted: {
        _placeHeader();
        _placeFooter();
    }

    Rectangle {
        id: surface
        anchors.fill: parent
        color: appWindow.color
        z: -1
    }

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
        height: appWindow.header ? appWindow.header.implicitHeight : 0
    }

    Item {
        id: footerSlot
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: appWindow.footer ? appWindow.footer.implicitHeight : 0
    }

    Item {
        id: contentHolder
        anchors.top: headerSlot.bottom
        anchors.bottom: footerSlot.top
        anchors.left: parent.left
        anchors.right: parent.right
    }
}
