import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
// import imtqml 1.0
// import QtGraphicalEffects 1.0

Item {
    id: commonButtonDecorator;

    width: icon.width + text.width + Style.paddingSmall * 3;
    height: 25;

    property var baseButton;

    Rectangle{
        id: mainRec;

        anchors.fill: parent;

        radius: 2;

        color: baseButton.color !== "" ? baseButton.color : baseButton.isHighlighted && baseButton.isHovered ? Style.hover :
                                                                              baseButton.isHovered ? Style.selectedColor:
                                                                              Style.buttonColor;


        border.width: 1;
        border.color: baseButton.isHovered || baseButton.selected ? Style.iconColorOnSelected : Style.borderColor;

        Image {
            id: icon
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: Style.paddingSmall
            width: Style.fontSize_common * 2
            height: width
            sourceSize.width: width
            sourceSize.height: height
            source: baseButton.imageSource
        }

        Text {
            id: text;
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: icon.right
            anchors.leftMargin: Style.paddingSmall

            color: baseButton.enabled ? Style.textColor : Style.inactive_textColor;

            font.pixelSize: Style.fontSize_common;
            font.family: Style.fontFamily;

            text: baseButton.text;

        }

    }

}

