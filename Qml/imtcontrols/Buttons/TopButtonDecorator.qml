import QtQuick 2.12
import Acf 1.0
import QtGraphicalEffects 1.0



Item {
    id: topButtonDecorator;

    width: isEmpty ? 20 : 60;
    height: 28;

    property var baseButton;
    property string textColor: Style.textColor;
    property string textColorDisabled: "gray";
    property real fontSize: Style.fontSize_small;
    property int radius: 4;
    property bool isEmpty: baseButton.text === "";

    DropShadow {
       id: dropShadow;

       anchors.fill: button;

       verticalOffset: 1;
       horizontalOffset: 1;

       radius: 2;
       color: Style.shadowColor;
       source: button;

//       visible: topButtonDecorator.isToggleable ? !topButtonDecorator.isToggled : button.visible && !topButtonDecorator.isHighlighted;
       visible: !(baseButton.checked || baseButton.down)

    }

    Rectangle{
        id: button;

        anchors.top: parent.top;
        anchors.topMargin: 6;
        anchors.left: parent.left;
        anchors.horizontalCenter: parent.horizontalCenter;

        width: parent.width;
        height: parent.height;

        radius: topButtonDecorator.radius;
//        visible: !topButtonDecorator.isEmpty;

        color: Style.baseColor;

//        gradient: topButtonDecorator.isHighlighted || topButtonDecorator.isToggled ? pressedGradientButton : standardGradientButton;
        gradient: baseButton.checked || baseButton.down ? pressedGradientButton : standardGradientButton;

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

            height: parent.height - 8;
            width: height;

            source: baseButton.iconSource;
            fillMode: Image.PreserveAspectFit;

            sourceSize.width: width;
            sourceSize.height: height;
        }
    }

    InnerShadow {
        id: innerShadow;

        anchors.fill: button;

        verticalOffset: 1;
        horizontalOffset: 1;

        radius: 2;
        color: Style.shadowColor;
        source: button;

        samples: 16;
//        visible: topButtonDecorator.isToggleable ? topButtonDecorator.isToggled : button.visible && topButtonDecorator.isHighlighted;
        visible: baseButton.checked || baseButton.down

    }

    Rectangle{
        id: selection;

        anchors.top: button.bottom;
        anchors.horizontalCenter: parent.horizontalCenter;

        height: 4;
        width: button.width/1.8;
        color: Style.greenColor;
        visible: false;
    }

    Text {
        id: description;

        anchors.top: selection.bottom;
        anchors.topMargin: 0;
        anchors.horizontalCenter: parent.horizontalCenter;

        color: topButtonDecorator.enabled ? topButtonDecorator.textColor : topButtonDecorator.textColorDisabled;
        font.pixelSize: topButtonDecorator.fontSize;

        font.family: Style.fontFamily;

        text: baseButton.text;
    }
}

