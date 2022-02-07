import QtQuick 2.0
import Acf 1.0;

Rectangle {
    id: container;
    width: 10;
    height: width;
    color: container.checkState == 2 ? "green" : (container.checkState == 0) ? "white" : "black";
    border.color: "gray";

    property int checkState: 0;

//    Image {
//        anchors.fill: parent;
//        visible: container.checkState === 2;
//        source: "../../../" + "Icons/" + Style.theme + "/Checkup_On_Normal.svg";
//    }

}
