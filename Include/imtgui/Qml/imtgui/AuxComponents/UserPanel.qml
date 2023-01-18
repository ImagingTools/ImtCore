import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

Item {
    id: root;

    height: 30;
    width: 50;

    property string username: authorizationPage.tokenProvider.login;
    property bool enabled: authorizationPage.state == "authorized";
    property alias iconSource: loginButton.iconSource;
    property bool isExitButton: false;

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

        width: 24;
        height: 24;

        highlighted: Style.highlightedButtons !==undefined ? Style.highlightedButtons : containsMouse;

        iconSource: "../../../Icons/" + Style.theme + "/Account.svg";

        enabled: root.enabled;

        onClicked: {
            if(root.isExitButton){
                panelDelegate.logout();
            }
            else{
                var point = mapToItem(thumbnailDecoratorContainer, x - width, y + height);
                point.x = point.x - 200;
                modalDialogManager.openDialog(popupMenu, {"x": point.x, "y": point.y, "model": contextMenuModel});
            }
        }
    }

    Component {
        id: popupMenu;

        PopupMenuDialog {
            onFinished: {
                console.log("CollectionView PopupMenuDialog", commandId);
                if (commandId == "Logout"){
                    panelDelegate.logout();
                }
            }
        }
    }

    ListModel {
        id: contextMenuModel;

        Component.onCompleted: {
            contextMenuModel.append({"Id": "Logout", "Name": qsTr("Logout"), "IconSource": ""});
        }
    }

    UserPanelDelegate {
        id: panelDelegate;
    }
}

