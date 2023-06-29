import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Rectangle {
    id: topRightPanelDecorator;

    color: "transparent";

    radius: 7;

    Component.onCompleted: {
        Events.subscribeEvent("UserModeChanged", topRightPanelDecorator.onUserModeChanged);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("UserModeChanged", topRightPanelDecorator.onUserModeChanged);
    }

    function onUserModeChanged(userMode){
        console.log("onUserModeChanged", userMode);
        if (userMode === "NO_USER_MANAGEMENT"){
            userPanel.visible = false;
        }
        else if (userMode === "OPTIONAL_USER_MANAGEMENT"){
            userPanel.visible = true;
        }
        else if (userMode === "STRONG_USER_MANAGEMENT"){
            userPanel.visible = true;
        }
    }

    UserPanel {
        id: userPanel;
        anchors.verticalCenter: parent.verticalCenter;

        visible: false;
    }

    PreferenceButton {
        id: preferenceButton;

        anchors.left: userPanel.right;
        anchors.leftMargin: 10;
        anchors.verticalCenter: parent.verticalCenter;

        width: 24;
        height: 24;
    }
}
