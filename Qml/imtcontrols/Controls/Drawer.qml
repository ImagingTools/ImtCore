import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

/*!
    \qmltype Drawer
    \inqmlmodule imtcontrols

    Side panel that slides in from a screen edge, API-compatible with Qt
    Quick Controls \c Drawer.

    Drawer overlays its parent and slides a panel in from one of the four
    \l edge values. Open it with \l open() and close it with \l close();
    \l position animates between \c 0 (fully closed) and \c 1 (fully open).
    When \l modal is true a dim layer covers the rest of the parent and a
    click on it closes the drawer.

    \qml
    Item {
        Drawer {
            id: drawer
            edge: Qt.LeftEdge
            Column { Label { text: qsTr("Menu") } }
        }
        Button { text: qsTr("Open"); onClicked: drawer.open() }
    }
    \endqml

    \sa Popup
*/
Item {
    id: drawer
    objectName: "ImtControlsDrawer"

    /*! Edge the drawer slides in from: \c Qt.LeftEdge, \c Qt.RightEdge,
        \c Qt.TopEdge or \c Qt.BottomEdge. */
    property int edge: Qt.LeftEdge

    /*! How far the drawer is open, in [0, 1] (\c 0 closed, \c 1 open). */
    property real position: 0

    /*! Whether the drawer dims and blocks the area behind it. */
    property bool modal: true

    /*! Panel thickness for left/right edges. */
    property real drawerWidth: Math.round(parent ? parent.width * 0.66 : Style.startWindowWidth * 0.66)

    /*! Panel thickness for top/bottom edges. */
    property real drawerHeight: Math.round(parent ? parent.height * 0.5 : Style.startWindowHeight * 0.5)

    /*! User content placed inside the sliding panel. */
    default property alias contentData: panel.data

    signal aboutToShow()
    signal aboutToHide()
    signal closed()
    signal opened()

    anchors.fill: parent
    visible: position > 0

    /*! Open the drawer. */
    function open() {
        aboutToShow();
        position = 1;
    }

    /*! Close the drawer. */
    function close() {
        aboutToHide();
        position = 0;
    }

    onPositionChanged: {
        if (position >= 1)
            drawer.opened();
        else if (position <= 0)
            drawer.closed();
    }

    Behavior on position {
        NumberAnimation { duration: 220; easing.type: Easing.OutCubic }
    }

    // Dim / modal layer behind the panel.
    Rectangle {
        id: dim
        anchors.fill: parent
        color: "#000000"
        opacity: drawer.modal ? drawer.position * 0.4 : 0
        visible: drawer.modal

        MouseArea {
            anchors.fill: parent
            enabled: drawer.modal && drawer.position > 0
            onClicked: drawer.close()
        }
    }

    // Sliding panel hosting the user content.
    Rectangle {
        id: panel
        color: Style.backgroundColor2
        border.width: Style.buttonBorderWidth
        border.color: Style.borderColor

        width:  (drawer.edge === Qt.LeftEdge || drawer.edge === Qt.RightEdge)
                ? drawer.drawerWidth : drawer.width
        height: (drawer.edge === Qt.TopEdge || drawer.edge === Qt.BottomEdge)
                ? drawer.drawerHeight : drawer.height

        x: drawer.edge === Qt.LeftEdge  ? (drawer.position - 1) * width
           : drawer.edge === Qt.RightEdge ? drawer.width - drawer.position * width
           : 0
        y: drawer.edge === Qt.TopEdge   ? (drawer.position - 1) * height
           : drawer.edge === Qt.BottomEdge ? drawer.height - drawer.position * height
           : 0
    }
}
