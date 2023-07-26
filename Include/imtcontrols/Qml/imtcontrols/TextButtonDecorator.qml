import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
// import imtqml 1.0
// import QtGraphicalEffects 1.0

FocusScope {
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

        color: baseButton.color !== "" ? baseButton.color : baseButton.isHighlighted && baseButton.isHovered ? Style.hover :
                                                                              baseButton.isHovered ? Style.selectedColor:
                                                                              Style.buttonColor;


        border.width: 1;
        border.color: baseButton.isHovered || baseButton.selected ? Style.iconColorOnSelected : Style.borderColor;


        Text {
            id: textObj;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.left: baseButton.imageSource ? iconObj.right : parent.left;
            anchors.leftMargin: baseButton.imageSource ? Style.paddingSmall : baseButton.width/2 - width/2;

            color: baseButton.enabled ? Style.textColor : Style.inactive_textColor;

            font.pixelSize: Style.fontSize_common;
            font.family: Style.fontFamily;

            text: baseButton.text;

        }

    }

}

