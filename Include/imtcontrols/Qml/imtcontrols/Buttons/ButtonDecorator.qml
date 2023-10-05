import QtQuick 2.12
import Acf 1.0

Rectangle{
    id: commonButtonDecorator

    width: iconObj.width + textObj.width + Style.paddingSmall * 3
    height: Style.buttonWidthMedium
    radius: Style.buttonRadius

    property var baseButton;
    property alias icon: iconObj

//    color: baseButton.down || baseButton.checked ? Style.buttonPressedColor : Style.buttonColor

//    border.width: Style.buttonBorderWidth
//    border.color: baseButton.hovered ? Style.iconColorOnSelected : commonButtonDecorator.baseButton.focus ? "gray" : Style.borderColor
    color: !baseButton ? "transtarent" : baseButton.down || baseButton.checked ? Style.buttonPressedColor : baseButton.hovered ?  Style.buttonHoverColor : Style.buttonColor

    border.width: Style.buttonBorderWidth
    border.color: !baseButton ? "transtarent" : baseButton.focus ? Style.borderFocusColor : Style.borderColor

    Image {
        id: iconObj

        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: textObj.text == "" ? parent.width/2 - width/2 : Style.paddingSmall

        width: source === "" ? 0 : Style.iconSizeSmall
        height: width

        sourceSize.width: width
        sourceSize.height: height
        source: !baseButton ? "" : baseButton.iconSource
    }

    Text {
        id: textObj

        anchors.verticalCenter: parent.verticalCenter
        anchors.left: !baseButton ? parent.left : baseButton.iconSource === "" ? parent.left : iconObj.right
        anchors.leftMargin: !baseButton ? 0 : baseButton.iconSource === "" ? parent.width/2 - width/2 : Style.paddingSmall

        color: !baseButton ? "transtarent" : baseButton.enabled ? Style.textColor : Style.inactive_textColor

        font.pixelSize: Style.fontSize_common
        font.family: Style.fontFamily

        text: !baseButton ? "" : baseButton.text

    }

}


