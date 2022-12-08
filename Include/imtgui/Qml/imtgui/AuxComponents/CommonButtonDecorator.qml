import QtQuick 2.12
import Acf 1.0
import QtGraphicalEffects 1.0

BaseButtonDecorator {
    id: commonButtonDecorator;

    width: 70;
    height: 25;

    Rectangle{
        id: mainRec;

        anchors.fill: parent;

        radius: 2;

        color: commonButtonDecorator.color !== "" ? commonButtonDecorator.color : commonButtonDecorator.isHighlighted && commonButtonDecorator.isHovered ? Style.hover :
                                                                              commonButtonDecorator.isHovered ? Style.selectedColor:
                                                                              Style.buttonColor;


        border.width: 1;
        border.color: commonButtonDecorator.isHovered || commonButtonDecorator.selected ? Style.iconColorOnSelected : Style.buttonBorderColor;

        Text {
            id: text;

            anchors.centerIn: parent;

            color: commonButtonDecorator.enabled ? Style.buttonText : Style.inactive_buttonText;

            font.pixelSize: Style.fontSize_common;
            font.family: Style.fontFamily;

            text: commonButtonDecorator.text;

        }

    }

}

