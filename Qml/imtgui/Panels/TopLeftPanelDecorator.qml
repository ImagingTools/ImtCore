import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

DecoratorBase {
    id: topLeftPanelDecorator;

    Text {
        anchors.centerIn: topLeftPanelDecorator;

        text: context.appName && context.appName !== "" ? context.appName : "";
        font.family: Style.fontFamilyBold;
        font.pixelSize: Style.fontSize_title;

        color: Style.textColor;
    }
}

