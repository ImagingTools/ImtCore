import QtQuick 2.12
import Acf 1.0
import imtauthgui 1.0
import imtgui 1.0
import imtcontrols 1.0

DecoratorBase {
    id: topRightPanelDecorator;

    Component.onCompleted: {
        Events.subscribeEvent("UserModeChanged", topRightPanelDecorator.onUserModeChanged);
        Events.subscribeEvent("Login", topRightPanelDecorator.onLogin);
        Events.subscribeEvent("Logout", topRightPanelDecorator.onLogout);

        onLogout();
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("UserModeChanged", topRightPanelDecorator.onUserModeChanged);
        Events.unSubscribeEvent("Login", topRightPanelDecorator.onLogin);
        Events.unSubscribeEvent("Logout", topRightPanelDecorator.onLogout);
    }

    function onUserModeChanged(userMode){
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

    function onLogin(){
        if (Qt.platform.os === "web"){
            preferenceButton.visible = true;
        }
    }

    function onLogout(){
        if (Qt.platform.os === "web"){
            preferenceButton.visible = false;
        }
    }

    UserPanel {
        id: userPanel;

        anchors.verticalCenter: parent ? parent.verticalCenter : undefined;

        visible: false;
    }

    PreferenceButton {
        id: preferenceButton;

        anchors.left: userPanel.right;
        anchors.leftMargin: Style.size_mainMargin;
        anchors.verticalCenter: parent ? parent.verticalCenter : undefined;

        width: 24;
        height: 24;
    }
}
