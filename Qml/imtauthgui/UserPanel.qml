import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtgui 1.0

Item {
    id: root;

    width: 50;
    height: 30;

    property bool enabled: false;

    property alias iconSource: loginButton.iconSource;
    property bool isExitButton: false;

    Component.onCompleted: {
        Events.subscribeEvent("SetUserPanelEnabled", root.setUserPanelEnabled);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("SetUserPanelEnabled", root.setUserPanelEnabled);
    }

	LocalizationEvent {
		onLocalizationChanged: {
			contextMenuModel.fillModel();
		}
	}

    Connections {
        target: AuthorizationController;

		function onLoggedIn(){
            root.enabled = true;
        }

		function onLoggedOut(){
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
        font.pixelSize: Style.fontSizeNormal;
        text: AuthorizationController.userTokenProvider.login;
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
				AuthorizationController.logout();
            }
            else{
                contextMenuModel.fillModel();

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
					AuthorizationController.logout();
                }
                else if (commandId == "Profile"){
					ModalDialogManager.openDialog(profileViewComp, {});
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
            contextMenuModel.append({"Id": "Profile", "Name": qsTr("Profile"), "Icon": "Icons/Account", "IsEnabled": true});
            contextMenuModel.append({"Id": "", "Name": "", "Icon": ""});
            contextMenuModel.append({"Id": "Logout", "Name": qsTr("Logout"), "Icon": "Icons/Exit", "IsEnabled": true});
        }
    }

    UserPanelDelegate {
        id: panelDelegate;
    }

    Component {
        id: profileViewComp;

        Dialog {
            id: dialog;
            width: 760;
            height: ModalDialogManager.activeView.height - 100;
            title: qsTr("Profile");
            canMove: false;
            contentComp: Component {
                ProfileView {
                    width: dialog.width;
                    height: dialog.height - 40;
                }
            }
        }
    }
}

