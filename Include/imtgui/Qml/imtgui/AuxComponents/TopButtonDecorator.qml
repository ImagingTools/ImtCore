import QtQuick 2.12
import Acf 1.0
import QtGraphicalEffects 1.0
import Qt5Compat.GraphicalEffects 1.0

BaseButtonDecorator {
    id: topButtonDecorator;

    width: isEmpty ? 20 : description.width > 60 ? description.width : 60;
    height: 28;

    property string textColor: Style.textColor;
    property string textColorDisabled: "gray";
    property real fontSize: Style.fontSize_small;
    property int radius: 4;
    property bool isEmpty: text == "";

    onIsHighlightedChanged: {
//        if (isToggleable){
//            return;
//        }

//        if (isHighlighted){
//            innerShadow.verticalOffset = 1;
//            innerShadow.horizontalOffset = 1;

//            button.width += 2;
//        }
//        else{
//            dropShadow.verticalOffset = 1;
//            dropShadow.horizontalOffset = 1;

//            button.width = 60;
//        }
    }

    onIsToggledChanged: {
        console.log("TopButtonDecorator onIsToggledChanged", isToggled);
//        if (isToggled){
//            innerShadow.visible = true;
//            dropShadow.visible = false;

//            button.width += 2;
//        }
//        else{
//            innerShadow.visible = false;
//            dropShadow.visible = true;

//            button.width = 60;
//        }
    }

    DropShadow {
       id: dropShadow;

       anchors.fill: button;

       verticalOffset: 1;
       horizontalOffset: 1;

       radius: 2;
       color: Style.shadowColor;
       source: button;

       visible: topButtonDecorator.isToggleable ? !topButtonDecorator.isToggled : button.visible && !topButtonDecorator.isHighlighted;
    }

    Rectangle{
        id: button;

        anchors.top: parent.top;
        anchors.topMargin: 6;
        anchors.horizontalCenter: parent.horizontalCenter;

        width: 60;
        height: parent.height;

        radius: topButtonDecorator.radius;
        visible: !topButtonDecorator.isEmpty;

        color: Style.baseColor;

        gradient: topButtonDecorator.isHighlighted || topButtonDecorator.isToggled ? pressedGradientButton : standardGradientButton;

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

            source: topButtonDecorator.imageSource;
            fillMode: Image.PreserveAspectFit;

            sourceSize.width: width;
            sourceSize.height: height;
        }
    }

//    InnerShadow {
//        id: innerShadow;

//        anchors.fill: button;

//        verticalOffset: 1;
//        horizontalOffset: 1;

//        radius: 2;
//        color: Style.shadowColor;
//        source: button;

//        samples: 16;
//        visible: topButtonDecorator.isToggleable ? topButtonDecorator.isToggled : button.visible && topButtonDecorator.isHighlighted;
//    }

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
        visible: !topButtonDecorator.isEmpty;

        font.family: Style.fontFamily;

        text: topButtonDecorator.text;
    }
}
