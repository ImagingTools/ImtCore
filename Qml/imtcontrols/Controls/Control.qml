import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

/*!
    \qmltype Control
    \inqmlmodule imtcontrols

    Base type of all imtcontrols visual controls, API-compatible with Qt
    Quick Controls \c Control.

    Control provides the common building blocks shared by every control:
    a \l background item drawn behind the control, a \l contentItem holding
    the visible content, configurable \l padding around the content and a
    \l spacing hint used by composite controls. The control's implicit size
    is derived from the content size plus paddings (or the background's
    implicit size, whichever is larger), mirroring Qt's behaviour.

    Subtypes typically override \l background and/or \l contentItem to provide
    their own look while inheriting padding, sizing and hover handling.

    \qml
    Control {
        padding: 10
        contentItem: Text { text: "Hello" }
        background: Rectangle { color: "#eee" }
    }
    \endqml

    \sa Pane, Frame, AbstractButton
*/
Item {
    id: control
    objectName: "ImtControlsControl"

    // -------------------------------------------------------------------- //
    //                          Padding / spacing                            //
    // -------------------------------------------------------------------- //

    /*! Common padding applied to every side unless a side is overridden. */
    property real padding: 0
    /*! Padding between the content and the top edge. */
    property real topPadding:    padding
    /*! Padding between the content and the left edge. */
    property real leftPadding:   padding
    /*! Padding between the content and the right edge. */
    property real rightPadding:  padding
    /*! Padding between the content and the bottom edge. */
    property real bottomPadding: padding

    /*! Spacing hint used by composite controls to separate their parts. */
    property real spacing: Style.spacingS

    /*! Font propagated to the content (API-compatible with Controls.Control). */
    property var font

    /*! Whether the control is in a mirrored (right-to-left) layout. */
    property bool mirrored: false

    // -------------------------------------------------------------------- //
    //                          Content / background                         //
    // -------------------------------------------------------------------- //

    /*! User-supplied content. \c contentData is the default property so
        \c{Control { Text {} }} populates the content holder's children. */
    default property alias contentData: contentHolder.data

    /*! The item hosting the visible content. Overridable by subtypes. */
    property Item contentItem: contentHolder

    /*! The item drawn behind the content. Overridable by subtypes. */
    property Item background: null

    // -------------------------------------------------------------------- //
    //                           Hover handling                              //
    // -------------------------------------------------------------------- //

    /*! Whether the control tracks the mouse and updates \l hovered. */
    property bool hoverEnabled: false

    /*! True while the mouse is over the control (requires \l hoverEnabled). */
    readonly property bool hovered: hoverHandler.containsMouse && hoverEnabled

    // -------------------------------------------------------------------- //
    //                               Sizing                                  //
    // -------------------------------------------------------------------- //

    /*! Implicit size of the content (used to compute the control size). */
    property real implicitContentWidth:  contentItem ? contentItem.implicitWidth  : 0
    property real implicitContentHeight: contentItem ? contentItem.implicitHeight : 0
    /*! Implicit size of the background. */
    property real implicitBackgroundWidth:  background ? background.implicitWidth  : 0
    property real implicitBackgroundHeight: background ? background.implicitHeight : 0

    implicitWidth:  Math.max(implicitContentWidth  + leftPadding + rightPadding,
                             implicitBackgroundWidth)
    implicitHeight: Math.max(implicitContentHeight + topPadding  + bottomPadding,
                             implicitBackgroundHeight)

    width:  implicitWidth
    height: implicitHeight

    /*! Content area remaining after paddings. */
    readonly property real availableWidth:  Math.max(0, width  - leftPadding - rightPadding)
    readonly property real availableHeight: Math.max(0, height - topPadding  - bottomPadding)

    // -------------------------------------------------------------------- //
    //                          Internal plumbing                            //
    // -------------------------------------------------------------------- //

    // Slot that the background item is reparented into (drawn first/below).
    Item {
        id: backgroundSlot
        anchors.fill: parent
        z: -1
    }

    // Hover tracker. Declared before the content slot so user content (and
    // any interactive children) stays above it and keeps event priority;
    // it only reports hover for this control via `hovered`.
    MouseArea {
        id: hoverHandler
        anchors.fill: parent
        enabled: control.hoverEnabled
        hoverEnabled: control.hoverEnabled
        acceptedButtons: Qt.NoButton
        propagateComposedEvents: true
    }

    // Slot that the content item is reparented into, inset by the paddings.
    Item {
        id: contentSlot
        anchors.fill: parent
        anchors.topMargin:    control.topPadding
        anchors.leftMargin:   control.leftPadding
        anchors.rightMargin:  control.rightPadding
        anchors.bottomMargin: control.bottomPadding
    }

    // Default content holder; replaced if the user assigns their own
    // contentItem, but still used to collect default contentData children.
    Item {
        id: contentHolder
        parent: contentSlot
        anchors.fill: parent
    }

    onBackgroundChanged: _placeBackground()
    onContentItemChanged: _placeContent()
    Component.onCompleted: {
        _placeBackground();
        _placeContent();
    }

    function _placeBackground() {
        if (!background)
            return;
        background.parent = backgroundSlot;
        background.anchors.fill = backgroundSlot;
    }

    function _placeContent() {
        if (!contentItem || contentItem === contentHolder)
            return;
        contentItem.parent = contentSlot;
        contentItem.anchors.fill = contentSlot;
    }
}
