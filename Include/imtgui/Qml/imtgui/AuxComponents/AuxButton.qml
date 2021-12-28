import QtQuick 2.12
import Acf 1.0

Rectangle {
    id: container;
    property alias iconSource: image.source;
    radius: container.height * 0.15;
    property bool highlighted: ma.containsMouse;

    color: container.highlighted ? Style.baseColor : "transparent";
    border.color: container.highlighted ? "gray" : "transparent";
    signal clicked;


    Image {
        id: image;
        anchors.centerIn: parent;
        height: parent.height;
        width: container.height;
//        source: "../../../Icons/" + Style.theme + "/Right_On_Normal.svg";

        sourceSize.width: width;
        sourceSize.height: height;
    }

    MouseArea {
        id: ma;
        anchors.fill: parent;
        hoverEnabled: true;
        cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor;

        onPressed: {
//            container.color = "transparent";
            image.anchors.verticalCenterOffset = 1;
        }

//        onReleased: {
//            image.anchors.verticalCenterOffset = 0;
//            if(ma.mouseX <= 0 || ma.mouseX >= container.width
//                    || ma.mouseY <= 0 || ma.mouseY >= container.height){
//                container.color = "transparent";
//            }
//            else{
////                container.color = "white";
////                container.color = Style.backgroundColor;
//            }

//        }

        onClicked: {
            container.clicked();
        }
    }

}
