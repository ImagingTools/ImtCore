import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

/*!
    \qmltype Tumbler
    \inqmlmodule imtcontrols

    Spinnable wheel of items, API-compatible with Qt Quick Controls
    \c Tumbler.

    Tumbler presents a vertical, scrollable column of items and keeps the
    centred item selected. Flick or drag the wheel to change the selection;
    \l currentIndex tracks the centred item. It is typically used to pick a
    value from a short list, such as an hour or a minute.

    \qml
    Tumbler {
        model: 12
        onCurrentIndexChanged: console.log("value:", currentIndex)
    }
    \endqml

    \sa SpinBox, Dial
*/
Item {
    id: tumbler
    objectName: "ImtControlsTumbler"

    /*! The model providing the items (a number, list or model). */
    property var model

    /*! Index of the centred (selected) item. */
    property int currentIndex: 0

    /*! Number of items in the model. */
    readonly property int count: view.count

    /*! The centred (selected) item, or \c null. */
    readonly property Item currentItem: view.currentItem

    /*! Number of items visible at once (the wheel height). */
    property int visibleItemCount: 5

    /*! Whether the wheel wraps around at the ends. */
    property bool wrap: false

    /*! True while the wheel is being flicked or dragged. */
    readonly property bool moving: view.moving

    /*! Delegate used to render each item. The default shows the item text. */
    property Component delegate: defaultDelegate

    /*! Height of a single item row. */
    property real itemHeight: Style.controlHeightL

    implicitWidth:  Style.buttonWidthXL
    implicitHeight: itemHeight * visibleItemCount

    Component {
        id: defaultDelegate
        Text {
            width: view.width
            height: tumbler.itemHeight
            text: modelData !== undefined ? modelData : index
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            color: index === tumbler.currentIndex ? Style.textColor : Style.inactiveTextColor
            font.family: Style.fontFamily
            font.pixelSize: Style.fontSizeM
        }
    }

    ListView {
        id: view
        anchors.fill: parent
        clip: true
        model: tumbler.model
        delegate: tumbler.delegate

        snapMode: ListView.SnapToItem
        highlightRangeMode: ListView.StrictlyEnforceRange
        preferredHighlightBegin: tumbler.itemHeight * Math.floor(tumbler.visibleItemCount / 2)
        preferredHighlightEnd: preferredHighlightBegin + tumbler.itemHeight
        boundsBehavior: tumbler.wrap ? Flickable.DragOverBounds : Flickable.StopAtBounds

        onCurrentIndexChanged: {
            if (tumbler.currentIndex !== currentIndex)
                tumbler.currentIndex = currentIndex;
        }
    }

    onCurrentIndexChanged: {
        if (view.currentIndex !== currentIndex)
            view.currentIndex = currentIndex;
    }

    // Selection guides framing the centred item.
    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        height: Style.buttonBorderWidth
        color: Style.borderColor
        y: tumbler.itemHeight * Math.floor(tumbler.visibleItemCount / 2)
    }

    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        height: Style.buttonBorderWidth
        color: Style.borderColor
        y: tumbler.itemHeight * (Math.floor(tumbler.visibleItemCount / 2) + 1)
    }
}
