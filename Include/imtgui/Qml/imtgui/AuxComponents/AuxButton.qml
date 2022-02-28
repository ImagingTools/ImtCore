import QtQuick 2.12
import Acf 1.0

Rectangle {
    id: auxButtonContainer;

    radius: 2;

    border.width: 1;
    border.color: auxButtonContainer.borderColor;

    color: auxButtonContainer.highlighted ? Style.selectedColor : auxButtonContainer.backgroundColor;

    property bool highlighted: ma.containsMouse;
    property bool hasIcon: auxButtonContainer.iconSource !== "";
    property bool hasText: false;
    property bool borderExist: false;
    property bool enabled: true;

    property string borderColor: "transparent";
    property string backgroundColor: "transparent";
    property string textButton;

    property int iconWidth: auxButtonContainer.width;
    property int iconHeight: auxButtonContainer.height;
    property int fontPixelSize: Style.fontSize_common;

    property alias iconSource: image.source;

    signal clicked;

    Image {
        id: image;

        anchors.centerIn: parent;

        height: auxButtonContainer.iconHeight - 3;
        width: auxButtonContainer.iconWidth - 3;

        sourceSize.width: width;
        sourceSize.height: height;

        visible: auxButtonContainer.hasIcon;

        Component.onCompleted: {
        }
    }

    Text {
//        id: text;

        anchors.horizontalCenter: auxButtonContainer.horizontalCenter;
        anchors.verticalCenter: auxButtonContainer.verticalCenter;

        color: auxButtonContainer.enabled ? Style.textColor : Style.disabledInActiveTextColor;

        font.pixelSize: auxButtonContainer.fontPixelSize;
        font.family: Style.fontFamily;
        text: auxButtonContainer.textButton;

        visible: auxButtonContainer.hasText;

        Component.onCompleted: {
            if (auxButtonContainer.hasText) {

//                if (auxButtonContainer.hasIcon) {
//                    text.anchors.left = image.right;
//                    text.anchors.leftMargin = 5;
//                    image.anchors.verticalCenter = auxButtonContainer.verticalCenter;
//                } else{
//                    text.anchors.horizontalCenter = auxButtonContainer.horizontalCenter;
//                    text.anchors.verticalCenter = auxButtonContainer.verticalCenter;
//                }
//                text.anchors.horizontalCenter = auxButtonContainer.horizontalCenter;
//                text.anchors.verticalCenter = auxButtonContainer.verticalCenter;
            }
        }
    }

    MouseArea {
        id: ma;
        anchors.fill: parent;

        hoverEnabled: true;
        //acceptedButtons: Qt.LeftButton;
//        cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor;
        cursorShape: Qt.PointingHandCursor;

        visible: auxButtonContainer.enabled;

        onPressed: {
            //image.anchors.verticalCenterOffset = 1;
        }

        onClicked: {
            auxButtonContainer.clicked();
        }
    }

}
