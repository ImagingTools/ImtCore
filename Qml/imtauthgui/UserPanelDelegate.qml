import QtQuick 2.12
import Acf 1.0
import imtguigql 1.0
import imtgui 1.0
import imtcontrols 1.0

Item {
    id: userPanelDelegate;

    property string userId;
    property string login;
    property string passwordHash;
    property TreeItemModel userModel: TreeItemModel {}

    signal userUpdated();

    property var dialogManager: ModalDialogManager;

    function logout(){
        Events.sendEvent("Logout");
    }

    function changePassword(){
        userPanelDelegate.dialogManager.openDialog(changePasswordComp, {});
    }

    property QtObject cacheData : QtObject{
        property string login;
        property string password;
    }

    function onResult(id, name){
        userPanelDelegate.dialogManager.openDialog(savingErrorDialog, {"message" : qsTr("Password changed successfully")});

        userPanelDelegate.userUpdated();
    }

    Component {
        id: changePasswordComp;

        ChangePasswordDialog {
            title: qsTr("Change Password");
            userPasswordHash: userPanelDelegate.passwordHash;
            login: userPanelDelegate.login;

            onFinished: {
                if (buttonId == Enums.ok){
                    userPanelDelegate.cacheData.login = this.login;
                    userPanelDelegate.cacheData.password = this.password;

                    userPanelDelegate.userModel.setData("Password", this.password)
                    userPanelDelegate.userModel.setData("Username", this.login)

                    documentController.documentId = userPanelDelegate.userId;
                    documentController.documentModel = userPanelDelegate.userModel;
                    documentController.saveDocument();
                }
            }
        }
    }

    GqlDocumentDataController{
        id: documentController;

        gqlGetCommandId: "UserItem";
        gqlAddCommandId: "UserAdd";
        gqlUpdateCommandId: "UserUpdate";

        onSaved: {
            userPanelDelegate.onResult(id, name);
        }
    }

    Component {
        id: savingErrorDialog;

        ErrorDialog {
            title: qsTr("Password changed");
            onFinished: {
            }
        }
    }
}

