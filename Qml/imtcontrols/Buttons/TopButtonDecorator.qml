import QtQuick 2.12
import Acf 1.0
import QtGraphicalEffects 1.0
import Qt5Compat.GraphicalEffects 6.0


Item {
    id: topButtonDecorator;

    width: isEmpty ? 20 : 55;
    height: button.height + description.height;

    property var baseElement;
    property string textColor: Style.textColor;
    property string textColorDisabled: "gray";
    property int radius: 4;
    property bool isEmpty: baseElement.text === "";

    visible: !isEmpty;

    DropShadow {
       id: dropShadow;

       anchors.fill: button;

       verticalOffset: 1;
       horizontalOffset: 1;

       radius: 2;
       color: Style.shadowColor;
       source: button;

       visible: !(topButtonDecorator.baseElement.checked || topButtonDecorator.baseElement.down)
    }

    Rectangle{
        id: button;

        anchors.top: topButtonDecorator.top;
        anchors.topMargin: 6;
        anchors.left: topButtonDecorator.left;
        anchors.horizontalCenter: topButtonDecorator.horizontalCenter;

        width: topButtonDecorator.width;
        height: 25;

        radius: topButtonDecorator.radius;

        color: Style.baseColor;

        gradient: topButtonDecorator.baseElement.checked || topButtonDecorator.baseElement.down ? pressedGradientButton : standardGradientButton;

        Gradient {
            id: standardGradientButton;

            GradientStop { position: 0.0; color: Style.imagingToolsGradient0; }
            GradientStop { position: 1.0; color: Style.imagingToolsGradient1; }
        }

        Gradient {
            id: pressedGradientButton;

            GradientStop { position: 0.0; color: Style.imagingToolsGradient1; }
            GradientStop { position: 1.0; color: Style.imagingToolsGradient1; }
        }

        Image {
            id: image;

            anchors.centerIn: parent;

            height: parent.height - 5;
            width: height;

            source: topButtonDecorator.baseElement.iconSource;
            fillMode: Image.PreserveAspectFit;

            sourceSize.width: width;
            sourceSize.height: height;
        }
    }

    DropShadow {
        id: innerShadow;

        anchors.fill: button;

        verticalOffset: -1;
        horizontalOffset: -1;

        radius: 2;
        color: Style.shadowColor;
        source: button;

        visible: topButtonDecorator.baseElement.checked || topButtonDecorator.baseElement.down
    }

    Rectangle{
        id: selection;

        anchors.top: button.bottom;
        anchors.horizontalCenter: topButtonDecorator.horizontalCenter;

        height: 4;
        width: button.width/1.8;
        color: Style.greenColor;
        visible: false;
    }

    Text {
        id: description;

        anchors.top: selection.bottom;
        anchors.topMargin: 0;
        anchors.horizontalCenter: topButtonDecorator.horizontalCenter;

        color: topButtonDecorator.enabled ? topButtonDecorator.textColor : topButtonDecorator.textColorDisabled;

        font.pixelSize: Style.fontSize_small
        font.family: Style.fontFamily

        text: !topButtonDecorator.baseElement ? "" : topButtonDecorator.baseElement.text
    }
}

