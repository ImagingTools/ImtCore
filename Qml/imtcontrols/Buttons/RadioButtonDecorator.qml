import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

DecoratorBase{
    id: decorator;

    width: buttonText.width + buttonText.x;
    height: 20;

    opacity: !baseElement ? 0.4 : baseElement.checked ? 1 : 0.25

    property string color: "gray";
    property string selectedColor: Style.imaginToolsAccent;
    property int fontSize: Style.fontSizeXSmall;

    Rectangle {
        id: circle;

        anchors.verticalCenter: parent.verticalCenter;

        width: decorator.height;
        height: width ;
        radius: width;
        color: "transparent";
        border.width: 2;
        border.color: !decorator.baseElement ? "transparent" : decorator.baseElement.checked ? decorator.selectedColor : "gray";

        Rectangle{
            id: innerRec;

            anchors.centerIn: parent;

            width: parent.width/2;
            height: width;
            radius: width;
            color: !decorator.baseElement ? "transparent" : decorator.baseElement.checked ? decorator.selectedColor : "transparent";
        }
    }

    Text {
        id: buttonText;

        anchors.verticalCenter: parent.verticalCenter;
        anchors.left: circle.right;
        anchors.leftMargin: 10;
        font.pixelSize: decorator.fontSize;
        font.bold: true;
        font.family: Style.fontFamily;

        text: decorator.baseElement ? qsTr(decorator.baseElement.text): "";
    }

}

