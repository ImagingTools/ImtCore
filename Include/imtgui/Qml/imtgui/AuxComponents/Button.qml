import QtQuick 2.12
import Acf 1.0

Rectangle {
    id: container;
    property alias iconSource: image.source;
    radius: container.height * 0.15;
    color: ma.mouseX <= 0 || ma.mouseX >= container.width
           || ma.mouseY <= 0 || ma.mouseY >= height
           || ma.pressed ? Style.buttonColor : Style.buttonColor;
//    property alias iconSource: ma.mouseX <= 0 || ma.mouseX >= container.width
//                               || ma.mouseY <= 0 || ma.mouseY >= height
//                               || ma.pressed ? Style.buttonColor : Style.buttonColor;

    signal clicked;


    Image {
        id: image;
        anchors.centerIn: parent;
        height: parent.height * 0.8;
        width: container.height;
//        sourceSize.width: width;
//        sourceSize.height: height;
    }

    MouseArea {
        id: ma;
        anchors.fill: parent;
        hoverEnabled: true;
//        onEntered: {
//            container.color = "white";
//        }
//        onExited: {
//            container.color = "transparent";
//        }
        onPressed: {
//            container.color = "transparent";
            image.anchors.verticalCenterOffset = 1;
        }

        onReleased: {
            image.anchors.verticalCenterOffset = 0;
            if(ma.mouseX <= 0 || ma.mouseX >= container.width
                    || ma.mouseY <= 0 || ma.mouseY >= container.height){
                container.color = "transparent";
            }
            else{
//                container.color = "white";
//                container.color = Style.backgroundColor;
            }

        }

        onClicked: {
            container.clicked();
        }
    }

}
