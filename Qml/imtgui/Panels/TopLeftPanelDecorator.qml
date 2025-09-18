import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

DecoratorBase {
    id: topLeftPanelDecorator;
    width: content.width;
	property int maxWidth: Style.sizeHintXXS;

    Item {
        id: content;
        width: tempText.width + 2*Style.marginM > topLeftPanelDecorator.maxWidth ?
                   topLeftPanelDecorator.maxWidth:
                   tempText.width + 2*Style.marginM;
        height: topLeftPanelDecorator.height;

        Image {
            id: applicationIcon
            anchors.verticalCenter: content.verticalCenter;
            anchors.left: parent.left
            anchors.leftMargin: Style.marginM
            height: parent.height - Style.marginM
            width: height
            visible: source !== ''
            source: context && context.appIcon && context.appIcon !== '' ? context.appIcon : ''
            sourceSize.width: width;
            sourceSize.height: height;
        }

        Text {
            id: applicationName;
            anchors.verticalCenter: content.verticalCenter;
            anchors.left: applicationIcon.right;
            anchors.leftMargin: Style.marginM;
            anchors.right: parent.right;
            anchors.rightMargin: Style.marginM;
            font.family: Style.fontFamilyBold;
            font.pixelSize: Style.fontSizeXXL;
            color: Style.textColor;
            text: context && context.appName && context.appName !== "" ? context.appName : "";
            elide: Text.ElideRight;
        }
    }

    Text {
        id: tempText;
        font.family: Style.fontFamilyBold;
        font.pixelSize: Style.fontSizeXXL;
        color: Style.textColor;
        text: applicationName.text;
        visible: false;
    }
}

