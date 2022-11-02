import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

Item {
    id: root;

    height: 30;
    width: 50;

    property string username: authorizationPage.login;

    Text {
        id: usernameText;

        anchors.verticalCenter: root.verticalCenter;
        anchors.right: loginButton.left;
        anchors.rightMargin: 5;

        text: root.username;

        color: Style.textColor;
        font.family: Style.fontFamily;
        font.pixelSize: Style.fontSize_common;
    }

    AuxButton {
        id: loginButton;

        anchors.right: parent.right;
        anchors.verticalCenter: root.verticalCenter;

        width: 24;
        height: 24;

        iconSource: "../../../Icons/" + Style.theme + "/Account.svg";

        onClicked: {
            var point = mapToItem(thumbnailDecoratorContainer, x - width, y + height);
            point.x = point.x - 200;
            modalDialogManager.openDialog(popupMenu, {"x": point.x, "y": point.y, "model": contextMenuModel});
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

