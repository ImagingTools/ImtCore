import QtQuick 2.12
import Acf 1.0
import imtguigql 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcontrols 1.0
import imtauthUsersSdl 1.0

Rectangle {
    id: root;

    anchors.fill: parent;

    color: Style.backgroundColor;

    property UserData userData: UserData {}
    property string userId: "su";

    signal beforeSetted();
    signal passwordSetted();
    signal failed(string message);

    Component.onCompleted: {
        userData.m_username = userId;
        userData.m_name = "superuser";
    }

    function setSuperuserPassword(password){
        root.beforeSetted();
        userData.m_password = password;
        documentController.documentModel = root.userData;
        documentController.insertDocument();
    }

    GqlRequestDocumentDataController{
        id: documentController;
        documentId: root.userId;
        gqlGetCommandId: ImtauthUsersSdlCommandIds.s_userItem;
        gqlUpdateCommandId: ImtauthUsersSdlCommandIds.s_userUpdate;
        gqlAddCommandId: ImtauthUsersSdlCommandIds.s_userAdd;

        Component.onCompleted: {
            getRequestInputParam.InsertField(UserItemInputTypeMetaInfo.s_productId, AuthorizationController.productId);
            addRequestInputParam.InsertField(UserItemInputTypeMetaInfo.s_productId, AuthorizationController.productId);
            updateRequestInputParam.InsertField(UserItemInputTypeMetaInfo.s_productId, AuthorizationController.productId);
        }

        onSaved: {
            root.passwordSetted();
        }

        onError: {
            root.failed(message);
        }
    }

    function checkPassword(){
        let ok = true;
        if (passwordTextInput.text === ""){
            errorText.text = qsTr("Please enter a non-empty password");

            ok = false;
        }
        else if (passwordTextInput.text !== confirmPasswordInput.text){
            errorText.text = qsTr("Passwords don't match");

            ok = false;
        }

        okButton.enabled = ok;
        errorText.visible = !ok;
    }

    Component{
        id: emptyDecorator;
        Item{
            property Item rootItem: null;
        }
    }

    Column {
        id: bodyColumn;

        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.verticalCenter: parent.verticalCenter;
        spacing: Style.size_mainMargin;
        width: 300;

        Item {
            width: bodyColumn.width;
            height: 30;

            BaseText {
                id: title;
                font.family: Style.fontFamilyBold;
                text: qsTr("Please enter the password for system administrator");
            }
        }

        BaseText {
            id: titlePassword;

            text: qsTr("Password");
        }

        CustomTextField {
            id: passwordTextInput;

            width: bodyColumn.width;
            height: 30;

            placeHolderText: qsTr("Enter the password");
            echoMode: TextInput.Password;
            KeyNavigation.tab: confirmPasswordInput;

            Loader{
                id: inputDecoratorLoader1;

                sourceComponent: Style.textFieldDecorator !==undefined ? Style.textFieldDecorator: emptyDecorator;
                onLoaded: {}
            }

            onTextChanged: {
                root.checkPassword();
            }
        }

        BaseText {
            id: titleConfirmPassword;
            text: qsTr("Confirm Password");
        }

        CustomTextField {
            id: confirmPasswordInput;

            width: bodyColumn.width;
            height: 30;

            placeHolderText: qsTr("Confirm password");
            echoMode: TextInput.Password;
            KeyNavigation.tab: passwordTextInput;

            Loader{
                id: inputDecoratorLoader2;

                sourceComponent: Style.textFieldDecorator !==undefined ? Style.textFieldDecorator: emptyDecorator;
                onLoaded: {}
            }

            onTextChanged: {
                root.checkPassword();
            }
        }

        Item {
            width: bodyColumn.width;
            height: 30;

            BaseText {
                id: errorText;

                anchors.left: parent.left;
                anchors.verticalCenter: parent.verticalCenter;

                color: Style.errorTextColor;

                visible: false;

                text: qsTr("Passwords don't match");
            }

            Button {
                id: okButton;

                anchors.right: parent.right;
                anchors.verticalCenter: parent.verticalCenter;

                enabled: false;

                text: qsTr("OK");

                onClicked: {
                    root.setSuperuserPassword(passwordTextInput.text);
                }

                decorator: Component{ButtonDecorator{
                    width: 70;
                    height: 23;

                }}
            }
        }
    }
}
