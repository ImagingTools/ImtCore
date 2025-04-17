import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

Item {
    id: commonButtonDecorator;

    width: textObj.width + Style.paddingSmall * 3;
    height: 25;

    property var baseButton;

    property alias radius: mainRec.radius;
    property alias color: mainRec.color;


    Rectangle{
        id: mainRec;

        anchors.fill: parent;

        radius: 2;

        color: commonButtonDecorator.baseButton.focus ? "red" : commonButtonDecorator.baseButton.color !== "" ? commonButtonDecorator.baseButton.color : commonButtonDecorator.baseButton.isHighlighted && commonButtonDecorator.baseButton.isHovered ? Style.hover :
                                                                              commonButtonDecorator.baseButton.isHovered ? Style.selectedColor:
                                                                              Style.buttonColor;


        border.width: 1;
        border.color: commonButtonDecorator.baseButton.isHovered || commonButtonDecorator.baseButton.selected ? Style.iconColorOnSelected : Style.borderColor;


        Text {
            id: textObj;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.left: parent.left;
            anchors.leftMargin: commonButtonDecorator.baseButton.imageSource ? Style.paddingSmall : commonButtonDecorator.baseButton.width/2 - width/2;

            color: commonButtonDecorator.baseButton.enabled ? Style.textColor : Style.inactiveTextColor;

            font.pixelSize: Style.fontSizeNormal;
            font.family: Style.fontFamily;

            text: commonButtonDecorator.baseButton.text;

        }

    }

}

