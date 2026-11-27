import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtauthgui 1.0
import imtgui 1.0
import imtcontrols 1.0

DecoratorBase {
    id: topRightPanelDecorator;

    // The bell and the assistant get their own places after the user panel and the preference button
    implicitWidth: Style.sizeHintBXS + (notificationBellLoader.active ? Style.buttonWidthM + Style.marginM : 0) + (assistantButtonLoader.active ? Style.buttonWidthM + Style.marginM : 0);

    Component.onCompleted: {
        onLogout();

        Events.subscribeEvent("NotificationCenterActiveChanged", topRightPanelDecorator.onNotificationCenterActiveChanged);
        Events.sendEvent("NotificationCenterStateRequested");

        Events.subscribeEvent("AssistantStateChanged", topRightPanelDecorator.onAssistantStateChanged);
        Events.sendEvent("AssistantStateRequested");
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("NotificationCenterActiveChanged", topRightPanelDecorator.onNotificationCenterActiveChanged);
        Events.unSubscribeEvent("AssistantStateChanged", topRightPanelDecorator.onAssistantStateChanged);
    }

    function onAssistantStateChanged(state){
        assistantButtonLoader.active = state.active;
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

    // Loaded by URL only while the assistant is enabled for the organization: applications without it do not need imtassistgui
    Loader {
        id: assistantButtonLoader;

        anchors.right: notificationBellLoader.active ? notificationBellLoader.left : parent.right;
        anchors.rightMargin: Style.marginM;
        anchors.verticalCenter: parent ? parent.verticalCenter : undefined;

        active: false;
        source: "qrc:/qml/imtassistgui/AssistantButton.qml";
    }
}
