import QtQuick 2.0
import Acf 1.0;

Rectangle {
    id: container;

    width: 13;
    height: width;
    //color: container.checkState == 2 ? "green" : (container.checkState == 0) ? "white" : "black";
    color: "transparent";

    border.width: 1;
    border.color: Style.alternateBaseColor;

    property int checkState: 0;

    Image {
        anchors.centerIn: parent;

        height: container.height - 1;
        width: height;

        sourceSize.width: width;
        sourceSize.height: height;

        visible: container.checkState === 2;
        source: "../../../" + "Icons/" + Style.theme + "/Ok_On_Normal.svg";
    }

}
