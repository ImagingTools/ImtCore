import QtQuick 2.12
import Acf 1.0
import imtguigql 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthUsersSdl 1.0

Item {
    id: userPanelDelegate;

    property string userId;
    property string login;
    property string passwordHash;
    property UserData userModel: UserData {}

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

                    userPanelDelegate.userModel.m_password = this.password;
                    userPanelDelegate.userModel.m_username = this.login;

                    documentController.documentId = userPanelDelegate.userId;
                    documentController.documentModel = userPanelDelegate.userModel;
                    documentController.saveDocument();
                }
            }
        }
    }

    GqlRequestDocumentDataController{
        id: documentController;

        gqlGetCommandId: ImtauthUsersSdlCommandIds.s_userItem;
        gqlUpdateCommandId: ImtauthUsersSdlCommandIds.s_userUpdate;
        gqlAddCommandId: ImtauthUsersSdlCommandIds.s_userAdd;

        Component.onCompleted: {
            getRequestInputParam.InsertField(UserItemInputTypeMetaInfo.s_productId, AuthorizationController.productId);
            addRequestInputParam.InsertField(UserItemInputTypeMetaInfo.s_productId, AuthorizationController.productId);
            updateRequestInputParam.InsertField(UserItemInputTypeMetaInfo.s_productId, AuthorizationController.productId);
        }

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

