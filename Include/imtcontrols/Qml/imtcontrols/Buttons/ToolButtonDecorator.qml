import QtQuick 2.12
import Acf 1.0


Rectangle{
    id: commonButtonDecorator

    height: 40
    width: textObj.width + Style.paddingSmall * 2 < height ? height : textObj.width + Style.paddingSmall * 2

    radius: Style.buttonRadius

    property var baseButton;
    property alias icon: iconObj

    color: baseButton.down || baseButton.checked ? Style.buttonPressedColor : Style.buttonColor

    border.width: 1
    border.color: baseButton.hovered || baseButton.checked ? Style.iconColorOnSelected : commonButtonDecorator.baseButton.focus ? "gray" : Style.borderColor

    Image {
        id: iconObj

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
//        anchors.topMargin: Style.paddingSmall

        width: Style.fontSize_common * 2
        height: width

        sourceSize.width: width
        sourceSize.height: height
        source: baseButton.iconSource
    }

    Text {
        id: textObj

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
//        anchors.bottomMargin: Style.paddingSmall

        color: baseButton.enabled ? Style.textColor : Style.inactive_textColor

        font.pixelSize: Style.fontSize_common
        font.family: Style.fontFamily

        text: baseButton.text
    }

}


