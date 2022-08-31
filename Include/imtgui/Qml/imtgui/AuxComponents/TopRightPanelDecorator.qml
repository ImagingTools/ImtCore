import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Rectangle {
    id: topRightPanelDecorator;

    color: "transparent";

    radius: 7;

    Row {
        id: container;

        anchors.fill: parent;

        height: parent.height;
        spacing: 10;

        PreferenceButton {
            id: preferenceButton;

            width: 24;
            height: 24;

            anchors.verticalCenter: parent.verticalCenter;
        }

//        LoginButton {
//            id: loginButton;

//            width: 24;
//            height: 24;

//            anchors.verticalCenter: parent.verticalCenter;
//        }
    }
}
