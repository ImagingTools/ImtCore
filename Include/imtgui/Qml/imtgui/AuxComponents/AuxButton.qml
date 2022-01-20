import QtQuick 2.12
import Acf 1.0

Rectangle {
    id: container;
    property alias iconSource: image.source;
    radius: container.height * 0.15;
    property bool highlighted: ma.containsMouse;

    property bool hasIcon: container.iconSource !== "";

    color: container.highlighted ? Style.hover : "transparent";

    property string textButton;
  //  border.color: container.highlighted ? "gray" : "transparent";
    signal clicked;


    Image {
        id: image;
        anchors.centerIn: parent;
        height: parent.height;
        width: container.height;
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
