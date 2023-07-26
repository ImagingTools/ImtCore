import QtQuick 2.12
import Acf 1.0
import imtguigql 1.0
import imtgui 1.0

Item {
    id: userPanelDelegate;

    property string userId;
    property string login;
    property string passwordHash;
    property TreeItemModel userModel: TreeItemModel {}

    signal userUpdated();

    function logout(){
        Events.sendEvent("Logout");
    }

    function changePassword(){
        modalDialogManager.openDialog(changePasswordComp, {});
    }

    property QtObject cacheData : QtObject{
        property string login;
        property string password;
    }

    Component {
        id: changePasswordComp;

        ChangePasswordDialog {
            title: qsTr("Change Password");
            userPasswordHash: userPanelDelegate.passwordHash;
            login: userPanelDelegate.login;

            onFinished: {
                if (buttonId === "Ok"){
                    cacheData.login = login;
                    cacheData.password = this.password;

                    userPanelDelegate.userModel.SetData("Password", this.password)
                    documentController.updateData(userPanelDelegate.userId, userPanelDelegate.userModel);
                }
            }
        }
    }

    GqlDocumentDataController{
        id: documentController;

        documentTypeId: "User";

        onDocumentUpdated: {
            modalDialogManager.openDialog(savingErrorDialog, {"message" : qsTr("Password changed successfully")});

            userPanelDelegate.userUpdated();
        }

        onSetModelStateChanged: {
            if (setModelState === "Loading"){
                Events.sendEvent("StartLoading");
            }
            else{
                Events.sendEvent("StopLoading");
            }
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

