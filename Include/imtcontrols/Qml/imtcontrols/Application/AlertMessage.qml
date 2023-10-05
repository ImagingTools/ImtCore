import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

Rectangle {
    id: alertMessage;

    width: 500;
    height: 40;

    color: "#f9dedc";

    property int margin: 10;
    property string message: "";
    property string colorText: Style.textColor;
    property string iconSource: "qrc:/Icons/Light/Alert_On_Active";

    Image {
        id: icon;

        anchors.left: parent.left;
        anchors.leftMargin: alertMessage.margin;
        anchors.verticalCenter: parent.verticalCenter;

        width: 20;
        height: 20;

        sourceSize.height: height;
        sourceSize.width: width;

        source: alertMessage.iconSource;
    }

    BaseText {
        id: message;

        anchors.left: icon.right;
        anchors.leftMargin: alertMessage.margin;
        anchors.right: parent.right;
        anchors.rightMargin: alertMessage.margin;
        anchors.verticalCenter: parent.verticalCenter;

        text: alertMessage.message;

        color: alertMessage.colorText;
    }
}
