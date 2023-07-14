import QtQuick 2.12
import Acf 1.0
import QtGraphicalEffects 1.0

BaseButtonDecorator {
    id: topButtonDecorator;

    height: 56;
    width: isEmpty ? 30 : 73;
    scale: isHighlighted ? 0.985 : 1;


    property string textColor: Style.textColor;
    property string textColorDisabled: "gray";
    property real fontSize: Style.fontSize_small;
    property int radius: 4;
    property bool isEmpty: text == "";


    DropShadow {
       id: dropShadow;

       anchors.fill: button;

       verticalOffset: 1;

       radius: 2;
       color: Style.shadowColor;
       source: button;

       visible: button.visible;
    }

    Rectangle{
        id: button;

        anchors.top: parent.top;
        anchors.topMargin: 6;
        anchors.horizontalCenter: parent.horizontalCenter;

        width: parent.width - 10;
        height: parent.height/2;

        radius: topButtonDecorator.radius;
        visible: !topButtonDecorator.isEmpty;

        color: Style.baseColor;

        gradient: selection.visible ? pressedGradientButton : standardGradientButton;

        Gradient {
            id: standardGradientButton;

            GradientStop { position: 0.0; color: Style.imagingToolsGradient0; }
            GradientStop { position: 1.0; color: Style.imagingToolsGradient1; }
        }

        Gradient {
            id: pressedGradientButton;

            GradientStop { position: 0.0; color: Style.imagingToolsGradient0; }
            GradientStop { position: 1.0; color: Style.imagingToolsGradient0; }
        }

        Image {
            id: image;

            anchors.centerIn: parent;

            height: parent.height - 8;
            width: height;

            source: topButtonDecorator.imageSource;
            fillMode: Image.PreserveAspectFit;

            sourceSize.width: width;
            sourceSize.height: height;
        }
    }

    Rectangle{
        id: selection;

        anchors.top: button.bottom;
        anchors.horizontalCenter: parent.horizontalCenter;

        height: 4;
        width: button.width/1.8;
        color: Style.greenColor;
        visible: topButtonDecorator.enabled && topButtonDecorator.isHighlighted;
    }

    Text {
        id: description;

        anchors.top: selection.bottom;
        anchors.topMargin: 0;
        anchors.horizontalCenter: parent.horizontalCenter;

        color: topButtonDecorator.enabled ? topButtonDecorator.textColor : topButtonDecorator.textColorDisabled;
        font.pixelSize: topButtonDecorator.fontSize;
        visible: !topButtonDecorator.isEmpty;

        font.family: Style.fontFamily;

        text: topButtonDecorator.text;
    }
}
