import QtQuick 2.12
import Acf 1.0

Rectangle {
    id: container;
    property alias iconSource: image.source;
//    radius: container.height * 0.15;
    radius: 2;
    property bool highlighted: ma.containsMouse;

    property bool hasIcon: container.iconSource !== "";

    property int iconWidth: container.width;
    property int iconHeight: container.height;

    color: container.highlighted ? Style.hover : "transparent";

//    color: "red";

    property string textButton;
//    border.color: container.highlighted ? Style.buttonBorderColor : "transparent";
     border.color: Style.buttonColor;
    signal clicked;


    Image {
        id: image;
        anchors.centerIn: parent;
        height: container.iconHeight;
        width: container.iconWidth;
        sourceSize.width: width;
        sourceSize.height: height;

        visible: container.hasIcon;
    }

    Text {
        id: text;
        anchors.centerIn: parent;

        color: Style.textColor;
        font.pixelSize: Style.fontSize_common;
        font.family: Style.fontFamily;
        text: container.textButton;
    }

    MouseArea {
        id: ma;
        anchors.fill: parent;
        hoverEnabled: true;
        cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor;

        onPressed: {
            image.anchors.verticalCenterOffset = 1;
        }

        onClicked: {
            container.clicked();
        }
    }

}
