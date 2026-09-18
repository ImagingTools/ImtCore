import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

ButtonDecorator {
    id: commonButtonDecorator

    property bool isLast: false

    Component.onCompleted: {
        patch.createObject(commonButtonDecorator.backgroundItem)
    }

    Component {
        id: patch
        Item {
            id: background

            x: commonButtonDecorator.isLast ? 0 : width

            height: commonButtonDecorator.height
            width: commonButtonDecorator.width / 2

            // Opaque fill that squares off the inner half of the segment and
            // hides the base background's rounded corners on this side.
            Rectangle {
                anchors.fill: parent
                color: commonButtonDecorator.backgroundItem.color
            }

            // Only the top and bottom borders are continued across the squared
            // half - no vertical borders are drawn. The previous implementation
            // used a fully-bordered rectangle plus an inner cover that was 1px
            // shorter than the border on top and bottom, leaving the vertical
            // border corners exposed as small marks overlapping the border at
            // the middle of each segment and at the seam between two segments.
            Rectangle {
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                height: commonButtonDecorator.backgroundItem.border.width
                color: commonButtonDecorator.backgroundItem.border.color
            }
            Rectangle {
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                height: commonButtonDecorator.backgroundItem.border.width
                color: commonButtonDecorator.backgroundItem.border.color
            }
        }
    }
}


