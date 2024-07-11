import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtgui 1.0

Item {
    id: root;

    width: 50;
    height: 30;

    property string username;
    property string userId;
    property string passwordHash;

    property bool enabled: false;

    property alias iconSource: loginButton.iconSource;
    property bool isExitButton: false;

    Component.onCompleted: {
        Events.subscribeEvent("OnLocalizationChanged", root.onLocalizationChanged);
        Events.subscribeEvent("SetUserPanelEnabled", root.setUserPanelEnabled);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("OnLocalizationChanged", root.onLocalizationChanged);
        Events.unSubscribeEvent("SetUserPanelEnabled", root.setUserPanelEnabled);
    }

    Connections {
        target: AuthorizationController;

        function onLoginSuccessful(){
            root.username = AuthorizationController.userTokenProvider.login;
            root.userId = AuthorizationController.userTokenProvider.userId;
            root.passwordHash = AuthorizationController.userTokenProvider.passwordHash;

            root.enabled = true;
        }

        function onLogoutSignal(){
            root.username = "";
            root.userId = "";
            root.passwordHash = "";

            root.enabled = false;
        }
    }

    function setUserPanelEnabled(enabled){
        root.enabled = enabled;
    }

    function setVisible(visible){
        root.visible = visible;
    }

    function onLocalizationChanged(language){
        contextMenuModel.fillModel();
    }

    Text {
        id: usernameText;

        anchors.verticalCenter: root.verticalCenter;
        anchors.right: loginButton.left;
        anchors.rightMargin: 5;

        color: Style.textColor;
        font.family: Style.fontFamily;
        font.pixelSize: Style.fontSize_common;

        text: root.username;
    }

    ToolButton {
        id: loginButton;

        anchors.right: parent ? parent.right : undefined;
        anchors.verticalCenter: root.verticalCenter;

        width: Style.buttonWidthMedium;
        height: width;

        iconSource: "../../../" + Style.getIconPath("Icons/Account", Icon.State.On, Icon.Mode.Normal);

        enabled: root.enabled;

        onClicked: {
            if(root.isExitButton){
                panelDelegate.logout();
            }
            else{
                var point = mapToItem(null, x - width, y + height);
                point.x = point.x - 200;
                ModalDialogManager.openDialog(popupMenu, {"x": point.x, "y": point.y, "model": contextMenuModel});
            }
        }
    }

    Component {
        id: popupMenu;

        PopupMenuDialog {
            id: popupMenuDialog;

            onFinished: {
                if (commandId == "Logout"){
                    panelDelegate.logout();
                }
                else if (commandId == "ChangePassword"){
                    panelDelegate.changePassword();
                }
            }
        }
    }

    ListModel {
        id: contextMenuModel;

        Component.onCompleted: {
            fillModel();
        }

        function fillModel(){
            contextMenuModel.clear();
            contextMenuModel.append({"Id": "ChangePassword", "Name": qsTr("Change Password"), "Icon": "Icons/Empty"});
            contextMenuModel.append({"Id": "", "Name": "", "Icon": ""});
            contextMenuModel.append({"Id": "Logout", "Name": qsTr("Logout"), "Icon": "Icons/Exit"});
        }
    }

    UserPanelDelegate {
        id: panelDelegate;

        userId: root.userId;
        passwordHash: root.passwordHash;
        login: root.username;

        onUserUpdated: {
            root.passwordHash = Qt.md5(cacheData.login + cacheData.password);
        }
    }
}

