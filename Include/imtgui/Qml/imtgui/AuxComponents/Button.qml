import QtQuick 2.12


Rectangle {
    id: container;
    property alias iconSource: image.source;
    radius: height * 0.15;
    color: ma.mouseX <= 0 || ma.mouseX >= width
           || ma.mouseY <= 0 || ma.mouseY >= height
           || ma.pressed ? "transparent" : "white";
    signal clicked;


    Image {
        id: image;
        anchors.centerIn: parent;
        height: parent.height * 0.8;
        width: height;
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
            container.color = "transparent";
            image.anchors.verticalCenterOffset = 1;
        }
        onReleased: {
            image.anchors.verticalCenterOffset = 0;
            if(ma.mouseX <= 0 || ma.mouseX >= width
                    || ma.mouseY <= 0 || ma.mouseY >= height){
                container.color = "transparent";
            }
            else{
                container.color = "white";
            }

        }

        onClicked: {
            container.clicked();
        }
    }

}
