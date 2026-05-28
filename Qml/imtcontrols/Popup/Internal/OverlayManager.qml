pragma Singleton
import QtQuick 2.12

/*!
    \qmltype OverlayManager
    \inqmlmodule imtcontrols
    \internal

    Owns a single \l Overlay item per hosting root Item (typically the
    window's contentItem, resolved by walking the QML parent chain). The
    overlay is the parent surface for every open popup that belongs to
    that root; it sits above the regular content tree, hosts the modal
    dim and the outside-click catcher, and orders popups by z.

    Overlays are created lazily: the first \l attach() call for a given
    root builds one, parents it to the root and fills it. When its last
    popup detaches, the overlay item is kept alive (cheap, single Item)
    so subsequent opens are immediate.
*/
QtObject {
    id: root

    // Map from rootItem -> overlay Item. Stored as parallel arrays because
    // QML's `var` map keys must be strings.
    property var _roots: []
    property var _overlays: []

    // Internal Component used to build an overlay item on demand. The
    // overlay itself is intentionally minimal: it just provides a click
    // catcher and a dim rectangle. The actual popup content is reparented
    // into it directly by Popup.qml.
    property Component _overlayComponent: Component {
        Item {
            id: overlay
            objectName: "ImtControlsOverlay"
            z: 1000000   // above all regular content

            // Popups attached to this overlay, ordered by insertion (== open order).
            property var openPopups: []

            // Modal dim rectangle. Visible whenever at least one modal popup
            // is open in this window; its z sits just below the topmost
            // modal popup.
            Rectangle {
                id: dim
                objectName: "ImtControlsOverlayDim"
                anchors.fill: parent
                color: "#80000000"
                visible: false
                opacity: 0
                z: -1

                Behavior on opacity { NumberAnimation { duration: 120 } }

                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.AllButtons
                    hoverEnabled: true
                    propagateComposedEvents: false
                    onPressed: {
                        overlay._handleOutsidePress(mouse, "dim");
                        mouse.accepted = true;
                    }
                    onReleased: {
                        overlay._handleOutsideRelease(mouse, "dim");
                        mouse.accepted = true;
                    }
                    onWheel: { wheel.accepted = true; }
                }
            }

            // Outside-click catcher for non-modal popups. It is a single
            // MouseArea that fills the overlay but lives below all popup
            // content; press events that reach it are by definition outside
            // every popup.
            MouseArea {
                id: outsideCatcher
                objectName: "ImtControlsOverlayCatcher"
                anchors.fill: parent
                acceptedButtons: Qt.AllButtons
                hoverEnabled: false
                propagateComposedEvents: true
                visible: overlay.openPopups.length > 0 && !root._anyModal(overlay)
                z: -2
                onPressed: {
                    overlay._handleOutsidePress(mouse, "catcher");
                    mouse.accepted = true;
                }
                onReleased: {
                    overlay._handleOutsideRelease(mouse, "catcher");
                    mouse.accepted = true;
                }
            }

            // Re-evaluate modal dim & top-modal z whenever the popup set
            // changes.
            function refresh() {
                var topModal = null;
                for (var i = 0; i < openPopups.length; ++i) {
                    var p = openPopups[i];
                    try {
                        if (p && p.opened && p.modal) topModal = p;
                    } catch (e) { /* destroyed */ }
                }
                if (topModal) {
                    dim.z = topModal.z - 1;
                    dim.visible = true;
                    dim.opacity = topModal.dim ? 0.5 : 0;
                } else {
                    dim.opacity = 0;
                    dim.visible = false;
                }
                outsideCatcher.visible = openPopups.length > 0 && !topModal;
            }

            function _handleOutsidePress(mouse, source) {
                // Iterate top-down so the topmost popup decides first.
                var sorted = openPopups.slice().sort(function(a,b){ return b.z - a.z; });
                for (var i = 0; i < sorted.length; ++i) {
                    var p = sorted[i];
                    try {
                        if (!p || !p.opened) continue;
                        p._onOverlayPressOutside(mouse, source);
                    } catch (e) { /* destroyed */ }
                }
            }

            function _handleOutsideRelease(mouse, source) {
                var sorted = openPopups.slice().sort(function(a,b){ return b.z - a.z; });
                for (var i = 0; i < sorted.length; ++i) {
                    var p = sorted[i];
                    try {
                        if (!p || !p.opened) continue;
                        p._onOverlayReleaseOutside(mouse, source);
                    } catch (e) { /* destroyed */ }
                }
            }

            // Global key dispatcher: Escape goes to the topmost popup that
            // accepts CloseOnEscape.
            Keys.onShortcutOverride: {
                if (event.key === Qt.Key_Escape && openPopups.length > 0) {
                    event.accepted = true;
                }
            }
            Keys.onPressed: {
                if (event.key === Qt.Key_Escape) {
                    var top = root._topPopup(overlay);
                    if (top && top._tryEscape()) {
                        event.accepted = true;
                        return;
                    }
                }
            }
        }
    }

    /*! Return (creating if needed) the overlay item for \a rootItem. */
    function overlayFor(rootItem) {
        if (!rootItem) return null;
        var idx = _roots.indexOf(rootItem);
        if (idx !== -1) return _overlays[idx];
        var ov = _overlayComponent.createObject(rootItem, {
            "width": Qt.binding(function(){ return rootItem.width; }),
            "height": Qt.binding(function(){ return rootItem.height; })
        });
        if (!ov) {
            console.warn("OverlayManager: failed to create overlay for root", rootItem);
            return null;
        }
        _roots.push(rootItem);
        _overlays.push(ov);
        // Detect root destruction.
        rootItem.Component.destruction.connect(function() {
            var i = _roots.indexOf(rootItem);
            if (i !== -1) {
                _roots.splice(i, 1);
                _overlays.splice(i, 1);
            }
        });
        return ov;
    }

    /*! Attach \a popup to the overlay belonging to its target root.
        Returns the overlay item or null on failure. */
    function attach(popup, rootItem) {
        var ov = overlayFor(rootItem);
        if (!ov) return null;
        if (ov.openPopups.indexOf(popup) === -1) {
            // Assign a z above all currently open popups in this overlay.
            var maxZ = 0;
            for (var i = 0; i < ov.openPopups.length; ++i) {
                var p = ov.openPopups[i];
                try { if (p && p.z > maxZ) maxZ = p.z; } catch(e) {}
            }
            if (popup.z <= maxZ) popup.z = maxZ + 1;
            ov.openPopups.push(popup);
        }
        ov.refresh();
        return ov;
    }

    /*! Detach \a popup from its overlay. */
    function detach(popup) {
        for (var w = 0; w < _overlays.length; ++w) {
            var ov = _overlays[w];
            var idx = ov.openPopups.indexOf(popup);
            if (idx !== -1) {
                ov.openPopups.splice(idx, 1);
                ov.refresh();
                return;
            }
        }
    }

    /*! Bring \a popup to the top of its overlay's z-stack. */
    function raise(popup) {
        for (var w = 0; w < _overlays.length; ++w) {
            var ov = _overlays[w];
            if (ov.openPopups.indexOf(popup) === -1) continue;
            var maxZ = 0;
            for (var i = 0; i < ov.openPopups.length; ++i) {
                var p = ov.openPopups[i];
                try { if (p !== popup && p.z > maxZ) maxZ = p.z; } catch(e) {}
            }
            popup.z = maxZ + 1;
            ov.refresh();
            return;
        }
    }

    // ---- internal helpers --------------------------------------------------

    function _anyModal(overlay) {
        for (var i = 0; i < overlay.openPopups.length; ++i) {
            var p = overlay.openPopups[i];
            try { if (p && p.opened && p.modal) return true; } catch(e) {}
        }
        return false;
    }

    function _topPopup(overlay) {
        var top = null;
        for (var i = 0; i < overlay.openPopups.length; ++i) {
            var p = overlay.openPopups[i];
            try {
                if (!p || !p.opened) continue;
                if (!top || p.z > top.z) top = p;
            } catch (e) {}
        }
        return top;
    }
}
