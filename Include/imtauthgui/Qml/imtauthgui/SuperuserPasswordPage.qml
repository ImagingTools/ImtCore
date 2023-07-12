import QtQuick 2.12
import Acf 1.0
import imtguigql 1.0
import imtgui 1.0

Rectangle {
    id: root;

    anchors.fill: parent;

    color: Style.backgroundColor;

    property TreeItemModel userModel: TreeItemModel {}

    signal beforeSetted();
    signal passwordSetted();
    signal failed(string message);

    Component.onCompleted: {
        root.userModel.SetData("Username", "su");
        root.userModel.SetData("Name", "superuser");
    }

    function setSuperuserPassword(password){
        root.beforeSetted();

        root.userModel.SetData("Password", password);

        documentController.setData("su", root.userModel);
    }

    GqlDocumentDataController{
        id: documentController;

        documentTypeId: "User";

        onDocumentAdded: {
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

        spacing: 10;

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

            BaseButton {
                id: okButton;

                anchors.right: parent.right;
                anchors.verticalCenter: parent.verticalCenter;

                enabled: false;

                text: qsTr("OK");

                onClicked: {
                    root.setSuperuserPassword(passwordTextInput.text);
                }
            }
        }
    }
}
