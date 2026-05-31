import QtQuick 2.12
import QtQuick.Window 2.1
import Acf 1.0
import imtcontrols 1.0

/*!
    \qmltype ApplicationWindow
    \inqmlmodule imtcontrols

    Top-level application window, API-compatible with Qt Quick Controls
    \c ApplicationWindow.

    ApplicationWindow is a \c Window that adds the conventional application
    layout: an optional \l header docked at the top, an optional \l footer
    docked at the bottom and the page content (its default children) in
    between. The window background is themed through the application
    \c Style singleton.

    \qml
    ApplicationWindow {
        visible: true
        header: ToolBar { Label { text: qsTr("My App") } }
        Page { }
    }
    \endqml

    \sa Page, ToolBar
*/
Window {
    id: appWindow
    objectName: "ImtControlsApplicationWindow"

    /*! Item docked at the top of the window (e.g. a \l ToolBar). */
    property Item header: null
    /*! Item docked at the bottom of the window (e.g. a \l ToolBar footer). */
    property Item footer: null

    /*! User content placed in the central area between header and footer. */
    default property alias contentData: contentHolder.data

    width: Style.startWindowWidth
    height: Style.startWindowHeight
    color: Style.backgroundColor2

    onHeaderChanged: _placeHeader()
    onFooterChanged: _placeFooter()
    Component.onCompleted: {
        _placeHeader();
        _placeFooter();
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
