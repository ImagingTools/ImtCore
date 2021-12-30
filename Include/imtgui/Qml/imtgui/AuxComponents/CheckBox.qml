import QtQuick 2.0
import Acf 1.0;

Rectangle {
    id: container;
    width: 10;
    height: width;
    color: checkState == 2 ? "green" : (checkState == 0) ? "white" : "black";
    border.color: "black";

    property int checkState: 0;

    Image {
        anchors.fill: parent;
        visible: container.checkState === 2;
        source: "../../../" + "Icons/" + Style.theme + "/" + "Check" + "_On_Normal.svg";
    }

}
