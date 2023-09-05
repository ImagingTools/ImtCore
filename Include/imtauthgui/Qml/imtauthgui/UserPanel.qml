import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

Item {
    id: root;

    height: 30;
    width: 50;

    property string username;
    property string userId;
    property string passwordHash;

    property bool enabled: false;

    property alias iconSource: loginButton.iconSource;
    property bool isExitButton: false;

    Component.onCompleted: {
        Events.subscribeEvent("SetUserPanelVisible", root.setVisible);
        Events.subscribeEvent("Logout", root.onLogout);
        Events.subscribeEvent("Login", root.onLogin);
        Events.subscribeEvent("OnLocalizationChanged", root.onLocalizationChanged);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("SetUserPanelVisible", root.setVisible);
        Events.unSubscribeEvent("Logout", root.onLogout);
        Events.unSubscribeEvent("Login", root.onLogin);
        Events.unSubscribeEvent("OnLocalizationChanged", root.onLocalizationChanged);
    }

    function onLogout(){
        root.username = "";
        root.userId = "";
        root.passwordHash = "";

        root.enabled = false;
    }

    function setVisible(visible){
        root.visible = visible;
    }

    function onLogin(loginData){
        root.username = loginData["Login"];
        root.userId = loginData["UserId"];
        root.passwordHash = loginData["PasswordHash"];

        root.enabled = true;
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

    AuxButton {
        id: loginButton;

        anchors.right: parent.right;
        anchors.verticalCenter: root.verticalCenter;

        width: Style.buttonWidthMedium;
        height: width;

        highlighted: Style.highlightedButtons !==undefined ? Style.highlightedButtons : containsMouse;

        iconSource: "../../../Icons/" + Style.theme + "/Account.svg";

        enabled: root.enabled;

        onClicked: {
            if(root.isExitButton){
                panelDelegate.logout();
            }
            else{
                //var point = mapToItem(thumbnailDecoratorContainer, x - width, y + height);
                var point = mapToItem(null, x - width, y + height);
                point.x = point.x - 200;
                modalDialogManager.openDialog(popupMenu, {"x": point.x, "y": point.y, "model": contextMenuModel});
            }
        }
    }

    Component {
        id: popupMenu;

        PopupMenuDialog {
            id: popupMenuDialog;

            onFinished: {
                console.log("CollectionView PopupMenuDialog", commandId);
                this.root.closeDialog();

                if (commandId == "Logout"){
                    panelDelegate.logout();
                }
                else if (commandId == "ChangePassword"){
                    panelDelegate.changePassword();
                }
            }

            function onBackgroundClicked(){
                popupMenuDialog.root.closeDialog();
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
            contextMenuModel.append({"Id": "ChangePassword", "Name": qsTr("Change Password"), "IconSource": ""});
            contextMenuModel.append({"Id": "Separator", "Name": "", "IconSource": ""});
            contextMenuModel.append({"Id": "Logout", "Name": qsTr("Logout"), "IconSource": ""});
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

