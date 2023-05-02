import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Rectangle {
    id: topRightPanelDecorator;

    color: "transparent";

    radius: 7;

    Component.onCompleted: {
        Events.subscribeEvent("SetUserPanelButtonVisible", topRightPanelDecorator.setUserPanelButtonVisible);
        Events.subscribeEvent("SetUserPanelButtonEnabled", topRightPanelDecorator.setUserPanelButtonEnabled);
        Events.subscribeEvent("SetPreferenceButtonVisible", topRightPanelDecorator.setPreferenceButtonVisible);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("SetUserPanelButtonVisible", topRightPanelDecorator.setUserPanelButtonVisible);
        Events.unSubscribeEvent("SetPreferenceButtonVisible", topRightPanelDecorator.setPreferenceButtonVisible);
        Events.unSubscribeEvent("SetUserPanelButtonEnabled", topRightPanelDecorator.setUserPanelButtonEnabled);
    }

    function setUserPanelButtonVisible(visible){
        userPanel.visible = visible;
    }

    function setUserPanelButtonEnabled(enabled){
        userPanel.enabled = enabled;
    }

    function setPreferenceButtonVisible(visible){
        preferenceButton.visible = visible;
    }

    UserPanel {
        id: userPanel;
        anchors.verticalCenter: parent.verticalCenter;
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
