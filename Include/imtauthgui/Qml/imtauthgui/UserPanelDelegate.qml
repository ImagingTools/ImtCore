import QtQuick 2.12
import Acf 1.0
import imtguigql 1.0
import imtgui 1.0

Item {
    id: userPanelDelegate;

    property string userId;
    property TreeItemModel userModel: TreeItemModel {}

    function logout(){
        Events.sendEvent("Logout");
    }

    function changePassword(){
        modalDialogManager.openDialog(changePasswordComp, {});
    }

    Component {
        id: changePasswordComp;

        ChangePasswordDialog {
            title: qsTr("Change Password");

            onFinished: {
                if (buttonId === "Ok"){
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
        }

        onError: {
        }
    }

    Component {
        id: savingErrorDialog;

        ErrorDialog {
            title: qsTr("Password changed");
            onFinished: {}
        }
    }
}

