import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtauthgui 1.0
import imtgui 1.0
import imtcontrols 1.0

DecoratorBase {
    id: topRightPanelDecorator;

    // The bell gets its own place after the user panel and the preference button
    implicitWidth: notificationBellLoader.active ? Style.sizeHintBXS + Style.buttonWidthM + Style.marginM : Style.sizeHintBXS;

    Component.onCompleted: {
        onLogout();

        Events.subscribeEvent("NotificationCenterActiveChanged", topRightPanelDecorator.onNotificationCenterActiveChanged);
        Events.sendEvent("NotificationCenterStateRequested");
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("NotificationCenterActiveChanged", topRightPanelDecorator.onNotificationCenterActiveChanged);
    }

    function onNotificationCenterActiveChanged(active){
        notificationBellLoader.active = active;
    }

    Connections {
        target: AuthorizationController;

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

		function onLoggedIn(){
            if (Qt.platform.os === "web"){
                preferenceButton.visible = true;
            }
        }

		function onLoggedOut(){
            topRightPanelDecorator.onLogout()
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
        anchors.leftMargin: Style.marginM;
        anchors.verticalCenter: parent ? parent.verticalCenter : undefined;

		width: Style.buttonWidthM;
		height: width;
    }

    // Loaded by URL only while the notification center runs: applications without it do not need imtnotifygui
    Loader {
        id: notificationBellLoader;

        anchors.right: parent ? parent.right : undefined;
        anchors.rightMargin: Style.marginM;
        anchors.verticalCenter: parent ? parent.verticalCenter : undefined;

        active: false;
        source: "qrc:/qml/imtnotifygui/NotificationBell.qml";
    }
}
