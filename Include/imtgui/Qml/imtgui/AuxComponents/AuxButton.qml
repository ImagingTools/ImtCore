import QtQuick 2.12
import Acf 1.0

Rectangle {
    id: container;

    property alias iconSource: image.source;
//    radius: container.height * 0.15;
    radius: 2;
    border.color: container.borderColor;
    property bool highlighted: ma.containsMouse;

    property bool hasIcon: container.iconSource !== "";

    property bool hasText: false;
    property bool borderExist: false;

    property bool enabled: true;

    property string borderColor: "transparent";
    property string backgroundColor: "transparent";

    property int iconWidth: container.width;
    property int iconHeight: container.height;

    property int fontPixelSize: Style.fontSize_common;

    color: container.highlighted ? Style.hover : container.backgroundColor;

//    color: "red";

    property string textButton;
//    border.color: container.highlighted ? Style.buttonBorderColor : "transparent";
//    border.color: Style.buttonColor;
    signal clicked;


    Image {
        id: image;
        anchors.centerIn: parent;
        height: container.iconHeight - 3;
        width: container.iconWidth - 3;
        sourceSize.width: width;
        sourceSize.height: height;

        visible: container.hasIcon;

        Component.onCompleted: {
//            if (container.hasIcon) {
//                image.anchors.left = container.left;
//                image.anchors.leftMargin = 5;
//                image.anchors.verticalCenter = container.verticalCenter;
//            } else {
//                image.anchors.centerIn = container;
//            }
        }
    }

    Text {
        id: text;
//        anchors.left: image.right;

        color: container.enabled ? Style.textColor : Style.disabledInActiveTextColor;
        font.pixelSize: container.fontPixelSize;
        font.family: Style.fontFamily;
        text: container.textButton;

        visible: container.hasText;

        Component.onCompleted: {
            if (container.hasText) {

//                if (container.hasIcon) {
//                    text.anchors.left = image.right;
//                    text.anchors.leftMargin = 5;
//                    image.anchors.verticalCenter = container.verticalCenter;
//                } else{
//                    text.anchors.horizontalCenter = container.horizontalCenter;
//                    text.anchors.verticalCenter = container.verticalCenter;
//                }
                text.anchors.horizontalCenter = container.horizontalCenter;
                text.anchors.verticalCenter = container.verticalCenter;
            }
        }
    }

    MouseArea {
        id: ma;
        anchors.fill: parent;
        hoverEnabled: true;
        cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor;
        visible: container.enabled;
        onPressed: {
            image.anchors.verticalCenterOffset = 1;
        }

        onClicked: {
            container.clicked();
        }
    }

}
