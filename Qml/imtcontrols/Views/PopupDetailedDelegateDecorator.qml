import QtQuick 2.12
import Qt5Compat.GraphicalEffects 6.0
import QtGraphicalEffects 1.12
import Acf 1.0
import imtcontrols 1.0

DecoratorBase {
    id: commonButtonDecorator

    height: content.height + Style.sizeMainMargin;

    clip: true;

    Rectangle {
        id: background

        anchors.fill: parent
        color: !commonButtonDecorator.baseElement ? "transparent" :
                              commonButtonDecorator.baseElement.highlighted ? Style.selectedColor :
                              commonButtonDecorator.baseElement.down || commonButtonDecorator.baseElement.checked ?
                                   Style.buttonPressedColor : commonButtonDecorator.baseElement.selected ?
                                       Style.buttonHoverColor : "transparent";
    }

    Column {
        id: content;

        anchors.verticalCenter: commonButtonDecorator.verticalCenter;
        anchors.left: commonButtonDecorator.left;
        anchors.leftMargin: Style.sizeMainMargin;
        anchors.right: commonButtonDecorator.right;
        anchors.rightMargin: Style.sizeMainMargin;

        Text {
            id: textObj

            width: parent.width;

            color: !commonButtonDecorator.baseElement ? "transparent" : commonButtonDecorator.baseElement.enabled ? Style.textColor : Style.inactive_textColor

            font.pixelSize: Style.fontSizeXSmall
            font.family: Style.fontFamily

            text: !commonButtonDecorator.baseElement ? "" : commonButtonDecorator.baseElement.text
            elide: Text.ElideRight;
        }

        Text {
            id: descriptionText

            width: parent.width;

            color: Style.inactive_textColor

            font.pixelSize: Style.fontSizeXXSmall
            font.family: Style.fontFamily

            text: !commonButtonDecorator.baseElement ? "" : commonButtonDecorator.baseElement.description;
            elide: Text.ElideRight;

            visible: text !== "";
        }
    }
}


