import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import "Internal" as Internal

/*!
    \qmltype Popup
    \inqmlmodule imtcontrols

    Base popup container, API-compatible with Qt Quick Controls \c Popup.

    A Popup is not a Window; instead its \l contentItem is reparented to a
    global per-window overlay during \l open(), and restored to a hidden
    state on \l close(). This matches Qt Quick Controls Popup behaviour and
    keeps the popup co-located with its declaring scope so bindings resolve
    naturally.

    The Popup item itself stays invisible and occupies no layout space in
    its declaring scope (it is a \c FocusScope with \c visible:false). The
    \l x, \l y, \l width and \l height properties therefore describe the
    popup's content geometry on the overlay, not its position in its parent.

    Typical use:
    \qml
    Popup {
        id: pop
        x: 10; y: 30
        width: 200; height: 100
        modal: true
        contentItem: Rectangle { color: "white" }
    }
    \endqml

    Lifecycle:
    \list
      \li \l open() emits \l aboutToShow(), reparents the content to the
          overlay, runs the enter transition, then emits \l opened().
      \li \l close() emits \l aboutToHide(), runs the exit transition, then
          hides the content and emits \l closed().
    \endlist

    \sa Menu
*/
FocusScope {
    id: popup

    objectName: "ImtControlsPopup"

    // The Popup is invisible in its declaring scope; visible content lives
    // on the overlay. We intentionally use Item.x/y/width/height as the
    // popup geometry: since visible is false and no layout consumes us,
    // these values are free for our positioning logic.
    visible: false
    width: implicitWidth
    height: implicitHeight
    // Participate in the Tab focus chain so popups can be reached via the
    // keyboard (matches QQC2 Popup which defaults to activeFocusOnTab).
    activeFocusOnTab: true

    // -------------------------------------------------------------------- //
    //                         Public Qt Quick Controls API                  //
    // -------------------------------------------------------------------- //

    /*! True while the popup is open or opening (visible on overlay). */
    readonly property bool opened: d.opened || d.opening

    /*! Whether the popup blocks input to items below the overlay. */
    property bool modal: false

    /*! Whether the modal dim background is rendered. Defaults to \c modal. */
    property bool dim: modal

    /*! Whether to take keyboard focus when opened. */
    property bool focusOnOpen: true

    /*! Font propagated to content (API-compatible with Controls.Popup). */
    property font font

    /*! Padding around content. Individual sides override the common value. */
    property real padding: 0
    property real topPadding:    padding
    property real leftPadding:   padding
    property real rightPadding:  padding
    property real bottomPadding: padding

    /*! Outer margins kept between popup and overlay bounds. \c -1 means
        "fall back to \c margins". */
    property real margins: 0
    property real topMargin:    -1
    property real leftMargin:   -1
    property real rightMargin:  -1
    property real bottomMargin: -1

    /*! Implicit sizing computed from \l contentItem. */
    property real implicitWidth:  Math.max(0, (contentItem ? contentItem.implicitWidth  : 0)) + leftPadding + rightPadding
    property real implicitHeight: Math.max(0, (contentItem ? contentItem.implicitHeight : 0)) + topPadding  + bottomPadding
    property real implicitContentWidth:  contentItem ? contentItem.implicitWidth  : 0
    property real implicitContentHeight: contentItem ? contentItem.implicitHeight : 0

    /*! Available content area inside paddings. */
    readonly property real availableWidth:  Math.max(0, width  - leftPadding - rightPadding)
    readonly property real availableHeight: Math.max(0, height - topPadding  - bottomPadding)

    /*! User-supplied content. \c contentChildren is the default property
        so \c{Popup { Text {} }} populates \c contentItem.children. */
    default property alias contentChildren: contentHolder.data
    property alias contentData: contentHolder.data

    /*! The item hosting user content. Overridable. */
    property Item contentItem: contentHolder

    /*! Background fills the popup behind content. Overridable. */
    property Item background: defaultBackground

    /*! Read-only reference to the overlay this popup is (or will be)
        attached to. */
    readonly property Item overlay: d.overlay

    /*! Bit-mask of close-policy flags. */
    property int closePolicy: Enums.popupDefaultClosePolicy

    /*! Visual transform origin used by enter/exit transitions. */
    property int transformOrigin: Item.Center

    /*! Visual scale/opacity reflected onto the live content root. */
    property real popupScale: 1.0
    property real popupOpacity: 1.0

    /*! RTL mirroring. */
    property bool mirrored: false

    /*! Position popup relative to the cursor at the time of open() instead
        of relative to \c parent. */
    property bool cursorRelative: false

    /*! Optional explicit parent popup. When null, parentage is inferred
        from the QML ancestor chain at open() time. */
    property var parentPopup: null

    // -------------------------------------------------------------------- //
    //                                  Signals                              //
    // -------------------------------------------------------------------- //

    signal aboutToShow()
    signal aboutToHide()
    signal closed()

    // -------------------------------------------------------------------- //
    //                              Public methods                           //
    // -------------------------------------------------------------------- //

    /*! Open the popup. Idempotent. If the popup is in its exit transition,
        that animation is cancelled and the popup re-enters from the current
        visual state, matching QQC2 Popup. */
    function open() {
        if (d.destroyed) return;
        d.cancelExitIfRunning();
        if (d.opened || d.opening) return;
        d._open();
    }

    /*! Close the popup. */
    function close() {
        if (d.destroyed) return;
        if (!d.opened && !d.opening) return;
        d._close();
    }

    /*! QQC2-style \c popup(parent): set the anchor then open. */
    function popupAt(anchor) {
        if (anchor) popup.parent = anchor;
        open();
    }

    /*! Force keyboard focus into the popup's content. */
    function forceActiveFocus() {
        if (d.contentRoot) d.contentRoot.forceActiveFocus();
        else popup.forceActiveFocus();
    }

    // -------------------------------------------------------------------- //
    //                              Internal API                             //
    // -------------------------------------------------------------------- //
    // _-prefixed properties/functions are consumed by the overlay, the
    // positioning engine and Menu. They form the contract between Popup
    // and its internal collaborators.

    readonly property Item _contentRoot: d.contentRoot
    readonly property Item _overlay: d.overlay
    property Item _rootItem: null   // resolved root content item at open()
    property point _cursorPos: Qt.point(0, 0)
    property bool _flippedVertically: false
    property bool _flippedHorizontally: false

    function _effectiveMargin(side) {
        switch (side) {
        case "top":    return topMargin    >= 0 ? topMargin    : margins;
        case "left":   return leftMargin   >= 0 ? leftMargin   : margins;
        case "right":  return rightMargin  >= 0 ? rightMargin  : margins;
        case "bottom": return bottomMargin >= 0 ? bottomMargin : margins;
        }
        return margins;
    }

    function _onOverlayPressOutside(mouse, source) {
        if (Internal.InputCoordinator.shouldClosePress(popup, mouse, source))
            popup.close();
    }
    function _onOverlayReleaseOutside(mouse, source) {
        if (Internal.InputCoordinator.shouldCloseRelease(popup, mouse, source))
            popup.close();
    }
    function _tryEscape() {
        if (closePolicy & Enums.popupCloseOnEscape) {
            close();
            return true;
        }
        return false;
    }
    function _reposition() { Internal.PositioningEngine.position(popup); }

    // -------------------------------------------------------------------- //
    //                       Lifecycle / registration                        //
    // -------------------------------------------------------------------- //

    Component.onCompleted: {
        Internal.PopupManager.register(popup);
    }
    Component.onDestruction: {
        d.destroyed = true;
        if (d.opened || d.opening) d._teardown(false);
        Internal.PopupStackController.detach(popup);
        Internal.PopupManager.unregister(popup);
    }

    onWidthChanged:  {
        if (d.opened || d.opening) popup._reposition()
    }
    onHeightChanged: {
        if (d.opened || d.opening) popup._reposition()
    }
    onXChanged:      {
        if (d.opened || d.opening) popup._reposition()
    }
    onYChanged:      {
        if (d.opened || d.opening) popup._reposition()
    }
    onParentChanged: {
        if (d.opened || d.opening) popup._reposition()
    }

    Connections {
        target: Internal.PopupManager
        function onWindowGeometryChanged() { if (d.opened || d.opening) popup._reposition(); }
    }

    Connections {
        target: d.overlay
        function onWidthChanged()  { if (d.opened || d.opening) popup._reposition(); }
        function onHeightChanged() { if (d.opened || d.opening) popup._reposition(); }
    }

    // -------------------------------------------------------------------- //
    //                            Default background                         //
    // -------------------------------------------------------------------- //

    Item {
        id: defaultBackground
        visible: false   // parented into contentRoot.bgSlot when active
        property color bgColor:   Style.baseColor
        property color borderClr: Style.borderColor
        property real  radiusVal: Style.buttonRadius
        Rectangle {
            anchors.fill: parent
            color: defaultBackground.bgColor
            radius: defaultBackground.radiusVal
            border.color: defaultBackground.borderClr
            border.width: Style.buttonBorderWidth
        }
    }

    // Holder for user content while the popup is closed.
    Item {
        id: contentHolder
        visible: false
        width: popup.availableWidth
        height: popup.availableHeight
    }

    // -------------------------------------------------------------------- //
    //                          Private state machine                        //
    // -------------------------------------------------------------------- //

    QtObject {
        id: d

        property bool opened: false
        property bool opening: false
        property bool closing: false
        property bool destroyed: false

        property Item overlay: null
        property Item targetRoot: null
        property Item contentRoot: null

        function resolveRoot() {
            // Walk the QML parent chain to the topmost Item. For controls
            // hosted in a QQuickWindow this is the window's contentItem,
            // which is what we want to use both as the overlay parent and
            // as the key identifying the hosting surface — without taking
            // any dependency on QtQuick.Window.
            var n = popup.parent;
            if (!n) return null;
            while (n.parent) n = n.parent;
            return n;
        }

        function _ensureContentRoot() {
            if (contentRoot) return contentRoot;
            contentRoot = contentRootComponent.createObject(overlay, { "popup": popup });
            return contentRoot;
        }

        function _open() {
            popup.aboutToShow();
            targetRoot = resolveRoot();
            if (!targetRoot) {
                console.warn("imtcontrols.Popup: cannot open - no root parent item");
                return;
            }
            popup._rootItem = targetRoot;
            overlay = Internal.OverlayManager.overlayFor(targetRoot);
            if (!overlay) {
                console.warn("imtcontrols.Popup: cannot open - overlay unavailable");
                return;
            }
            _ensureContentRoot();
            Internal.OverlayManager.attach(popup, targetRoot);

            // Auto-detect parent popup if not explicitly set.
            var pp = popup.parentPopup;
            if (!pp) pp = _findParentPopup();
            if (pp) Internal.PopupStackController.attach(pp, popup);

            // Reparent user content & background into the live root.
            if (popup.background) {
                popup.background.parent = contentRoot.bgSlot;
                popup.background.anchors.fill = contentRoot.bgSlot;
                popup.background.visible = true;
            }
            if (popup.contentItem) {
                popup.contentItem.parent = contentRoot.contentSlot;
                popup.contentItem.anchors.fill = contentRoot.contentSlot;
                popup.contentItem.visible = true;
            }

            if (popup.cursorRelative && popup.parent) {
                var c = popup.parent.mapToItem(overlay, popup.parent.width/2, popup.parent.height/2);
                popup._cursorPos = c;
            }

            opening = true;
            contentRoot.opacity = 0;
            contentRoot.scale = 0.9;
            contentRoot.visible = true;
            popup._reposition();

            if (popup.focusOnOpen) {
                Internal.FocusCoordinator.push(targetRoot, popup.parent);
                contentRoot.forceActiveFocus();
            }

            enterAnim.restart();
        }

        function _close() {
            Internal.PopupStackController.closeDescendants(popup);
            popup.aboutToHide();
            opening = false;
            closing = true;
            exitAnim.restart();
        }

        function _teardown(animated) {
            if (contentRoot) {
                contentRoot.visible = false;
                if (popup.contentItem && popup.contentItem !== contentHolder) {
                    // Custom contentItem: send back to popup ownership.
                    popup.contentItem.parent = popup;
                    popup.contentItem.visible = false;
                } else if (popup.contentItem === contentHolder) {
                    contentHolder.parent = popup;
                    contentHolder.visible = false;
                }
                if (popup.background && popup.background !== defaultBackground) {
                    popup.background.parent = popup;
                    popup.background.visible = false;
                } else if (popup.background === defaultBackground) {
                    defaultBackground.parent = popup;
                    defaultBackground.visible = false;
                }
            }
            Internal.OverlayManager.detach(popup);
            if (opened && popup.focusOnOpen && targetRoot)
                Internal.FocusCoordinator.pop(targetRoot);
            opened = false;
            opening = false;
            closing = false;
            if (!destroyed) popup.closed();
        }

        function cancelExitIfRunning() {
            if (closing) {
                exitAnim.stop();
                closing = false;
                // We're already attached and content is still parented to
                // the overlay; just re-run enter from current opacity.
                opening = true;
                opened = false;
                enterAnim.restart();
            }
        }

        function _findParentPopup() {
            var n = popup.parent;
            while (n) {
                if (n.objectName === "ImtControlsPopup" && n !== popup) return n;
                n = n.parent;
            }
            return null;
        }
    }

    // The content root that lives on the overlay. Built lazily.
    Component {
        id: contentRootComponent
        FocusScope {
            id: cr
            property var popup: null
            width: popup ? popup.width : 0
            height: popup ? popup.height : 0
            visible: false
            opacity: popup ? popup.popupOpacity : 1
            scale: popup ? popup.popupScale : 1
            transformOrigin: popup ? popup.transformOrigin : Item.Center
            activeFocusOnTab: true

            Item { id: bgSlot; anchors.fill: parent; z: -1 }
            property alias bgSlot: bgSlot
            Item {
                id: contentSlot
                x: popup ? popup.leftPadding : 0
                y: popup ? popup.topPadding  : 0
                width: popup ? popup.availableWidth  : 0
                height: popup ? popup.availableHeight : 0
            }
            property alias contentSlot: contentSlot

            // Eat presses so they don't reach the outside-click catcher,
            // and use them as "raise + collapse chain above me" signals.
            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.AllButtons
                propagateComposedEvents: true
                hoverEnabled: false
                onPressed: {
                    if (cr.popup) {
                        Internal.PopupStackController.closeAbove(cr.popup);
                        Internal.OverlayManager.raise(cr.popup);
                    }
                    mouse.accepted = false;
                }
            }

            // Trap Tab/Backtab inside modal popups so keyboard focus cannot
            // leak to controls underneath the modal. For non-modal popups
            // the standard focus chain is preserved.
            function _isWithin(item) {
                var n = item;
                while (n) {
                    if (n === cr) return true;
                    n = n.parent;
                }
                return false;
            }

            Keys.onPressed: {
                if (event.key === Qt.Key_Escape && cr.popup && cr.popup._tryEscape()) {
                    event.accepted = true;
                    return;
                }
                if ((event.key === Qt.Key_Tab || event.key === Qt.Key_Backtab)
                        && cr.popup && cr.popup.modal) {
                    var fwd = (event.key === Qt.Key_Tab) && !(event.modifiers & Qt.ShiftModifier);
                    var next = cr.nextItemInFocusChain(fwd);
                    if (!next || !cr._isWithin(next)) {
                        // Wrap focus back to the popup scope itself.
                        cr.forceActiveFocus();
                    } else {
                        next.forceActiveFocus();
                    }
                    event.accepted = true;
                }
            }
        }
    }

    NumberAnimation {
        id: enterAnim
        target: d.contentRoot
        property: "opacity"
        to: 1.0
        duration: 120
        easing.type: Easing.OutQuad
        onStopped: {
            if (d.opening) {
                d.opening = false;
                d.opened = true;
                if (d.contentRoot) { d.contentRoot.opacity = 1; d.contentRoot.scale = 1; }
            }
        }
    }

    NumberAnimation {
        id: enterScaleAnim
        target: d.contentRoot
        property: "scale"
        to: 1.0
        duration: 120
        easing.type: Easing.OutBack
        running: enterAnim.running
    }

    NumberAnimation {
        id: exitAnim
        target: d.contentRoot
        property: "opacity"
        to: 0.0
        duration: 100
        easing.type: Easing.InQuad
        onStopped: {
            if (d.closing) d._teardown(true);
        }
    }

    onPopupScaleChanged:   {
        if (d.contentRoot && d.opened) d.contentRoot.scale   = popupScale
    }
    onPopupOpacityChanged: {
        if (d.contentRoot && d.opened) d.contentRoot.opacity = popupOpacity
    }
}
