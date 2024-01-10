import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

DecoratorBase {
    id: commonButtonDecorator

    width: iconObj.width + textObj.width + Style.paddingSmall * 3
    height: Style.buttonWidthMedium

    property alias icon: iconObj
    property alias font: textObj.font
    property alias textColor: textObj.color
    property alias color: background.color
    property alias radius: background.radius
    property alias border: background.border

    Rectangle {
        id: background

        anchors.fill: parent
        radius: Style.buttonRadius
        color: !commonButtonDecorator.baseElement ? "transtarent" : commonButtonDecorator.baseElement.down || commonButtonDecorator.baseElement.checked ? Style.buttonPressedColor : commonButtonDecorator.baseElement.hovered ?  Style.buttonHoverColor : Style.buttonColor
        border.width: Style.buttonBorderWidth
        border.color: !commonButtonDecorator.baseElement ? "transtarent" : commonButtonDecorator.baseElement.focus ? Style.borderFocusColor : Style.borderColor
    }

    Image {
        id: iconObj

        anchors.verticalCenter: commonButtonDecorator.verticalCenter
        anchors.left: commonButtonDecorator.left
        anchors.leftMargin: textObj.text == "" ? commonButtonDecorator.width/2 - width/2 : Style.paddingSmall

        width: source === "" ? 0 : Style.iconSizeSmall
        height: width

        sourceSize.width: width
        sourceSize.height: height
        source: !commonButtonDecorator.baseElement ? "" : commonButtonDecorator.baseElement.iconSource
    }

    Text {
        id: textObj

        anchors.verticalCenter: commonButtonDecorator.verticalCenter
        anchors.left: !commonButtonDecorator.baseElement ? commonButtonDecorator.left : commonButtonDecorator.baseElement.iconSource === "" ? commonButtonDecorator.left : iconObj.right
        anchors.leftMargin: !commonButtonDecorator.baseElement ? 0 : commonButtonDecorator.baseElement.iconSource === "" ? commonButtonDecorator.width/2 - width/2 : Style.paddingSmall

        color: !commonButtonDecorator.baseElement ? "transtarent" : commonButtonDecorator.baseElement.enabled ? Style.textColor : Style.inactive_textColor

        font.pixelSize: Style.fontSize_common
        font.family: Style.fontFamily

        text: !baseElement ? "" : baseElement.text
    }

}


