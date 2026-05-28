pragma Singleton
import QtQuick 2.12

/*!
    \qmltype PositioningEngine
    \inqmlmodule imtcontrols
    \internal

    Computes final on-overlay coordinates for a Popup based on its anchor
    item (\c popup.parent), its own size, declared \c x/y/margins, RTL
    mirroring, and the bounds of its overlay/window.

    The algorithm:
      1. Map the popup's \c parent origin into overlay coordinates.
      2. Add the popup's declared \c x and \c y (treated as offsets relative
         to that origin, as in Qt Quick Controls Popup).
      3. If \c popup.mirrored is true, mirror the x offset around the parent.
      4. Clamp inside [margin, overlayBound - size - margin]. If the popup
         doesn't fit on its declared side relative to the parent, flip to
         the opposite side (edge flipping).
      5. If \c popup.cursorRelative is true, the popup origin is the cursor
         position rather than the parent.
*/
QtObject {
    id: root

    /*! Compute and apply the absolute overlay position for \a popup. */
    function position(popup) {
        if (!popup || !popup._overlay || !popup._contentRoot) return;
        var overlay = popup._overlay;
        var parentItem = popup.parent;
        var w = popup._contentRoot.width;
        var h = popup._contentRoot.height;
        if (w <= 0 || h <= 0) return;

        var origin = Qt.point(0, 0);
        if (popup.cursorRelative && popup._cursorPos) {
            origin = popup._cursorPos;
        } else if (parentItem) {
            origin = parentItem.mapToItem(overlay, 0, 0);
        }

        var x = origin.x + (popup.x || 0);
        var y = origin.y + (popup.y || 0);

        // RTL: mirror x around the parent's right edge.
        if (popup.mirrored && parentItem) {
            x = origin.x + parentItem.width - (popup.x || 0) - w;
        }

        var marginL = popup._effectiveMargin("left");
        var marginR = popup._effectiveMargin("right");
        var marginT = popup._effectiveMargin("top");
        var marginB = popup._effectiveMargin("bottom");

        var maxX = overlay.width - w - marginR;
        var maxY = overlay.height - h - marginB;
        var minX = marginL;
        var minY = marginT;

        // Edge flipping: if popup doesn't fit below its parent, try above.
        if (parentItem && (y + h > overlay.height - marginB)) {
            var aboveY = origin.y - h;
            if (aboveY >= marginT) {
                y = aboveY;
                popup._flippedVertically = true;
            } else {
                popup._flippedVertically = false;
            }
        } else {
            popup._flippedVertically = false;
        }

        if (parentItem && (x + w > overlay.width - marginR)) {
            var leftX = origin.x - w + (parentItem ? parentItem.width : 0);
            if (leftX >= marginL) {
                x = leftX;
                popup._flippedHorizontally = true;
            } else {
                popup._flippedHorizontally = false;
            }
        } else {
            popup._flippedHorizontally = false;
        }

        // Final clamp.
        if (maxX < minX) maxX = minX;
        if (maxY < minY) maxY = minY;
        if (x < minX) x = minX;
        if (x > maxX) x = maxX;
        if (y < minY) y = minY;
        if (y > maxY) y = maxY;

        popup._contentRoot.x = x;
        popup._contentRoot.y = y;
    }
}
