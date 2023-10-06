import QtQuick 2.12
import Acf 1.0

Rectangle{
    id: commonButtonDecorator

    width: iconObj.width + textObj.width + Style.paddingSmall * 3
    height: Style.buttonWidthMedium
    radius: Style.buttonRadius

    property var baseElement;
    property alias icon: iconObj

//    color: baseElement.down || baseElement.checked ? Style.buttonPressedColor : Style.buttonColor

//    border.width: Style.buttonBorderWidth
//    border.color: baseElement.hovered ? Style.iconColorOnSelected : commonButtonDecorator.baseElement.focus ? "gray" : Style.borderColor
    color: !baseElement ? "transtarent" : baseElement.down || baseElement.checked ? Style.buttonPressedColor : baseElement.hovered ?  Style.buttonHoverColor : Style.buttonColor

    border.width: Style.buttonBorderWidth
    border.color: !baseElement ? "transtarent" : baseElement.focus ? Style.borderFocusColor : Style.borderColor

    Image {
        id: iconObj

        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: textObj.text == "" ? parent.width/2 - width/2 : Style.paddingSmall

        width: source === "" ? 0 : Style.iconSizeSmall
        height: width

        sourceSize.width: width
        sourceSize.height: height
        source: !baseElement ? "" : baseElement.iconSource
    }

    Text {
        id: textObj

        anchors.verticalCenter: parent.verticalCenter
        anchors.left: !baseElement ? parent.left : baseElement.iconSource === "" ? parent.left : iconObj.right
        anchors.leftMargin: !baseElement ? 0 : baseElement.iconSource === "" ? parent.width/2 - width/2 : Style.paddingSmall

        color: !baseElement ? "transtarent" : baseElement.enabled ? Style.textColor : Style.inactive_textColor

        font.pixelSize: Style.fontSize_common
        font.family: Style.fontFamily

        text: !baseElement ? "" : baseElement.text

    }

}


