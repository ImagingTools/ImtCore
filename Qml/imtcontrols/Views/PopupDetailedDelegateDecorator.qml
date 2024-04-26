import QtQuick 2.12
import Qt5Compat.GraphicalEffects 6.0
import QtGraphicalEffects 1.12
import Acf 1.0
import imtcontrols 1.0

DecoratorBase {
    id: commonButtonDecorator

    height: content.height + Style.size_mainMargin;

    clip: true;

    Rectangle {
        id: background

        anchors.fill: parent
        color: !baseElement ? "transparent" :
                              baseElement.highlighted ? Style.selectedColor :
                              baseElement.down || baseElement.checked ?
                                   Style.buttonPressedColor : baseElement.selected ?
                                       Style.buttonHoverColor : "transparent";
    }

    Column {
        id: content;

        anchors.verticalCenter: commonButtonDecorator.verticalCenter;
        anchors.left: commonButtonDecorator.left;
        anchors.leftMargin: Style.size_mainMargin;
        anchors.right: commonButtonDecorator.right;
        anchors.rightMargin: Style.size_mainMargin;

        Text {
            id: textObj

            width: parent.width;

            color: !commonButtonDecorator.baseElement ? "transparent" : commonButtonDecorator.baseElement.enabled ? Style.textColor : Style.inactive_textColor

            font.pixelSize: Style.fontSize_common
            font.family: Style.fontFamily

            text: !commonButtonDecorator.baseElement ? "" : commonButtonDecorator.baseElement.text
            elide: Text.ElideRight;
        }

        Text {
            id: descriptionText

            width: parent.width;

            color: Style.inactive_textColor

            font.pixelSize: Style.fontSize_small
            font.family: Style.fontFamily

            text: !commonButtonDecorator.baseElement ? "" : commonButtonDecorator.baseElement.description;
            elide: Text.ElideRight;

            visible: text !== "";
        }
    }
}


