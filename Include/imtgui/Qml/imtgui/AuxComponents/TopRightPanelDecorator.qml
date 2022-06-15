import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Rectangle {
    id: topRightPanelDecorator;

    width: parent.width;
    height: parent.height;

    color: "transparent";

    radius: 7;

    Component.onCompleted: {
        console.log("TopRightPanelDecorator onCompleted")
    }

    Row {
        anchors.leftMargin: 10;
        anchors.horizontalCenter: parent.horizontalCenter;
//        anchors.verticalCenter: parent.verticalCenter;

        height: parent.height;
        spacing: 5;

        PreferenceButton {
            id: preferenceButton;

            width: 24;
            height: 24;

            anchors.verticalCenter: parent.verticalCenter;

        }
    }
}

