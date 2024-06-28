import QtQuick 2.12
import Acf 1.0
import QtGraphicalEffects 1.0
import Qt5Compat.GraphicalEffects 6.0


ButtonDecorator {
    property bool isEmpty: baseElement.text === "";

    width: isEmpty ? 50 : contentWidth + 2 * Style.size_mainMargin;

    maxTextWidth: 200;
    color: Style.backgroundColor2;
}

//Item {
//    id: topButtonDecorator;

//    width: isEmpty ? Style.size_mainMargin : button.width;
//    height: button.height;

//    property var baseElement;
//    property string textColor: Style.textColor;
//    property string textColorDisabled: Style.inactive_textColor;
//    property int radius: Style.buttonRadius;
//    property bool isEmpty: baseElement.text === "";

//    visible: !isEmpty;

//    Button {
//        id: button;

//        widthFromDecorator: true;
//        height: 30;

//        iconSource: topButtonDecorator.baseElement.iconSource;
//        text: !topButtonDecorator.baseElement ? "" : topButtonDecorator.baseElement.text;

//        decorator: Component {
//            ButtonDecorator {
//                maxTextWidth: 200;
//                color: Style.backgroundColor2;
//            }
//        }
//    }
//}

//Item {
//    id: topButtonDecorator;

//    width: isEmpty ? 20 : description.width > button.width ? description.width: button.width;
////    width: isEmpty ? 20 : description.width + button.width;
//    height: button.height + description.height;

//    property var baseElement;
//    property string textColor: Style.textColor;
//    property string textColorDisabled: Style.inactive_textColor;
//    property int radius: Style.buttonRadius;
//    property bool isEmpty: baseElement.text === "";

//    visible: !isEmpty;

//    DropShadow {
//       id: dropShadow;

//       anchors.fill: button;

//       verticalOffset: 1;
//       horizontalOffset: 1;

//       radius: topButtonDecorator.radius;
//       color: Style.shadowColor;
//       source: button;

//       visible: !(topButtonDecorator.baseElement.checked || topButtonDecorator.baseElement.down)
//    }

//    Rectangle {
//        id: button;

//        anchors.horizontalCenter: topButtonDecorator.horizontalCenter;
//        anchors.top: topButtonDecorator.top;
//        anchors.topMargin: Style.size_smallMargin;

//        width: 55;
//        height: 25;

//        radius: topButtonDecorator.radius;

//        color: Style.baseColor;

//        gradient: topButtonDecorator.baseElement.checked || topButtonDecorator.baseElement.down ? pressedGradientButton : standardGradientButton;

//        Gradient {
//            id: standardGradientButton;

//            GradientStop { position: 0.0; color: Style.imagingToolsGradient0; }
//            GradientStop { position: 1.0; color: Style.imagingToolsGradient1; }
//        }

//        Gradient {
//            id: pressedGradientButton;

//            GradientStop { position: 0.0; color: Style.imagingToolsGradient1; }
//            GradientStop { position: 1.0; color: Style.imagingToolsGradient1; }
//        }

//        Image {
//            id: image;

//            anchors.centerIn: parent;

//            height: parent.height - 5;
//            width: height;

//            source: topButtonDecorator.baseElement.iconSource;
//            fillMode: Image.PreserveAspectFit;

//            sourceSize.width: width;
//            sourceSize.height: height;
//        }
//    }

//    DropShadow {
//        id: innerShadow;

//        anchors.fill: button;

//        verticalOffset: -1;
//        horizontalOffset: -1;

//        radius: topButtonDecorator.radius;
//        color: Style.shadowColor;
//        source: button;

//        visible: topButtonDecorator.baseElement.checked || topButtonDecorator.baseElement.down
//    }

//    Text {
//        id: description;

//        anchors.horizontalCenter: topButtonDecorator.horizontalCenter;
//        anchors.top: button.bottom;
//        anchors.topMargin: 3;

//        color: topButtonDecorator.baseElement && topButtonDecorator.baseElement.enabled ? topButtonDecorator.textColor : topButtonDecorator.textColorDisabled;

//        font.pixelSize: Style.fontSize_small
//        font.family: Style.fontFamily

//        text: !topButtonDecorator.baseElement ? "" : topButtonDecorator.baseElement.text
//    }
//}


