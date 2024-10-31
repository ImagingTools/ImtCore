import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

DecoratorBase {
    id: topLeftPanelDecorator;
    width: content.width;
    property int maxWidth: 200;

    Item {
        id: content;
        width: tempText.width + 2*Style.size_mainMargin > topLeftPanelDecorator.maxWidth ?
                   topLeftPanelDecorator.maxWidth:
                   tempText.width + 2*Style.size_mainMargin;
        height: topLeftPanelDecorator.height;
        Text {
            id: applicationName;
            anchors.verticalCenter: content.verticalCenter;
            anchors.left: parent.left;
            anchors.leftMargin: Style.size_mainMargin;
            anchors.right: parent.right;
            anchors.rightMargin: Style.size_mainMargin;
            font.family: Style.fontFamilyBold;
            font.pixelSize: Style.fontSize_title;
            color: Style.textColor;
            text: context && context.appName && context.appName !== "" ? context.appName : "";
            elide: Text.ElideRight;
        }
    }

    Text {
        id: tempText;
        font.family: Style.fontFamilyBold;
        font.pixelSize: Style.fontSize_title;
        color: Style.textColor;
        text: applicationName.text;
        visible: false;
    }
}

