import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

/*!
    \qmltype ScrollView
    \inqmlmodule imtcontrols

    Scrollable container, API-compatible with Qt Quick Controls
    \c ScrollView.

    ScrollView wraps its content in a \c Flickable and shows \l ScrollBar
    handles when the content is larger than the view. Place a single sized
    child (e.g. an \c Image, \c Column or \c Text) inside it.

    \qml
    ScrollView {
        width: 200; height: 200
        Column {
            Repeater { model: 50; Label { text: "Row " + index } }
        }
    }
    \endqml

    \sa ScrollBar, Flickable
*/
Item {
    id: control
    objectName: "ImtControlsScrollView"

    /*! Single content child placed inside the internal Flickable. */
    default property alias contentData: holder.data

    /*! The internal Flickable, exposed for advanced binding. */
    property alias flickable: flickable

    width:  Style.startWindowWidth / 2
    height: Style.startWindowHeight / 2

    Flickable {
        id: flickable
        anchors.fill: parent
        clip: true
        boundsBehavior: Flickable.StopAtBounds
        // Derive content size from the (single) child placed in the holder.
        contentWidth:  holder.childrenRect.width
        contentHeight: holder.childrenRect.height

        // User content is collected here (reparented into contentItem).
        Item { id: holder }
    }

    ScrollBar {
        id: vBar
        orientation: Qt.Vertical
        flickable: flickable
        visible: flickable.contentHeight > flickable.height
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.bottomMargin: hBar.visible ? hBar.height : 0
    }

    ScrollBar {
        id: hBar
        orientation: Qt.Horizontal
        flickable: flickable
        visible: flickable.contentWidth > flickable.width
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.rightMargin: vBar.visible ? vBar.width : 0
    }
}
