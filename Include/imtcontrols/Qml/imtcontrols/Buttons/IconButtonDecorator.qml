import QtQuick 2.12
import Acf 1.0
//import imtcontrols 1.0
// import imtqml 1.0
// import QtGraphicalEffects 1.0

Item {
    id: commonButtonDecorator;

    width: iconObj.width + textObj.width + Style.paddingSmall * 3;
    height: 25;

    property var baseButton;

    property alias radius: mainRec.radius;
    property alias color: mainRec.color;
    property alias icon: iconObj;



    Rectangle{
        id: mainRec;

        anchors.fill: parent;

        radius: 2;

//        color: baseButton.pressed || baseButton.checked ? Style.buttonPressedColor : baseButton.hoverEnabled && baseButton.hovered ? Style.hover :
//                                                                              baseButton.hovered ? Style.selectedColor:
//                                                                              Style.buttonColor;
        color: baseButton.pressed || baseButton.checked ? Style.buttonPressedColor : Style.buttonColor;

        border.width: 1;
        border.color: baseButton.hovered || baseButton.selected ? Style.iconColorOnSelected : commonButtonDecorator.baseButton.focus ? "gray" : Style.borderColor;

        Image {
            id: iconObj;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.left: parent.left;
            anchors.leftMargin: textObj.text == "" ? parent.width/2 - width/2 : Style.paddingSmall;

            width: Style.fontSize_common * 2;
            height: width;

            sourceSize.width: width;
            sourceSize.height: height;
            source: baseButton.iconSource;
        }

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

