import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

ButtonDecorator {
    id: commonButtonDecorator

    property bool isLast: false

    Component.onCompleted: {
        patch.createObject(commonButtonDecorator.backgroundItem)
    }

    Component {
        id: patch
        Rectangle {
            id: background

            x: commonButtonDecorator.isLast ? 0 : width

            height: commonButtonDecorator.height
            width: commonButtonDecorator.width / 2
            radius: 0
            color: commonButtonDecorator.backgroundItem.color

            border.width: commonButtonDecorator.backgroundItem.border.width
            border.color: commonButtonDecorator.backgroundItem.border.color
            Rectangle {
                anchors.centerIn: parent
                color: background.color
                height: background.height - background.border.width * 2
                width: background.width
            }
        }
    }
}


