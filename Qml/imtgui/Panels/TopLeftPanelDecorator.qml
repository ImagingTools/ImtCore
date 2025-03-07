import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

DecoratorBase {
    id: topLeftPanelDecorator;
    width: content.width;
    property int maxWidth: 200;

    Item {
        id: content;
        width: tempText.width + 2*Style.sizeMainMargin > topLeftPanelDecorator.maxWidth ?
                   topLeftPanelDecorator.maxWidth:
                   tempText.width + 2*Style.sizeMainMargin;
        height: topLeftPanelDecorator.height;
        Text {
            id: applicationName;
            anchors.verticalCenter: content.verticalCenter;
            anchors.left: parent.left;
            anchors.leftMargin: Style.sizeMainMargin;
            anchors.right: parent.right;
            anchors.rightMargin: Style.sizeMainMargin;
            font.family: Style.fontFamilyBold;
            font.pixelSize: Style.fontSizeLarge;
            color: Style.textColor;
            text: context && context.appName && context.appName !== "" ? context.appName : "";
            elide: Text.ElideRight;
        }
    }

    Text {
        id: tempText;
        font.family: Style.fontFamilyBold;
        font.pixelSize: Style.fontSizeLarge;
        color: Style.textColor;
        text: applicationName.text;
        visible: false;
    }
}

