import QtQuick 2.12
import Acf 1.0
import imtguigql 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthUsersSdl 1.0

Item {
    id: userPanelDelegate;

    property UserData userModel: UserData {}

    signal userUpdated();

    property var dialogManager: ModalDialogManager;

    function logout(){
        Events.sendEvent("Logout");
    }

    function changePassword(){
        userPanelDelegate.dialogManager.openDialog(changePasswordComp, {});
    }

    function onResult(id, name){
        userPanelDelegate.dialogManager.openDialog(savingErrorDialog, {"message" : qsTr("Password changed successfully")});

        userPanelDelegate.userUpdated();
    }

    Component {
        id: changePasswordComp;

        ChangePasswordDialog {
            title: qsTr("Change Password");

            onFinished: {
                if (buttonId == Enums.save){
                    AuthorizationController.changePassword(AuthorizationController.userTokenProvider.userId, contentItem.oldPassword, contentItem.newPassword);
                }
            }
        }
    }
}

