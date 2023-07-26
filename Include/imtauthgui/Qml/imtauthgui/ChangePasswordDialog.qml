import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

Dialog {
    id: root;

    width: 600;

    property string password: contentItem.password;

    property string userPasswordHash;
    property string login;

    Component.onCompleted: {
        console.log("InputDialog onCompleted", root);

        root.buttons.addButton({"Id":"Ok", "Name":qsTr("OK"), "Enabled": false, "Active": true});
        root.buttons.addButton({"Id":"Cancel", "Name":qsTr("Cancel"), "Enabled": true, "Active": true});
    }

    onUserPasswordHashChanged: {
        console.log("onUserPasswordHashChanged", userPasswordHash);
    }

    Keys.onPressed: {
        if (event.key === Qt.Key_Return){
            let enabled = root.buttons.getButtonState("Ok");
            if (enabled){
                root.finished("Ok")
            }
        }
    }

    contentComp: Component { Item {
        id: contentItem;

        height: columnBodyMain.height + 30;

        property alias password: columnBody.password;

        onFocusChanged: {
            console.log("InputBody onFocusChanged", focus);

            if (contentItem.focus){
                oldPassword.focus = contentItem.focus;
            }
        }

        Column {
            id: columnBodyMain;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.right: parent.right;
            anchors.left: parent.left;
            anchors.rightMargin: 10;
            anchors.leftMargin: 10;

            width: parent.width;

            spacing: 10;

            function checkPassword(){
                console.log("checkPassword");

                console.log("root.login", root.login);
                console.log("roldPassword.text", oldPassword.text);

                if (oldPassword.text != ""){
                    let passwordHash = Qt.md5(root.login + oldPassword.text);
                    console.log("passwordHash", passwordHash);
                    console.log("userPasswordHash", userPasswordHash);
                    if (passwordHash == root.userPasswordHash){
                        columnBody.errorText.visible = false;
                        columnBody.readOnly = false;

                        return;
                    }
                }

                columnBody.readOnly = true;
                columnBody.errorText.visible = true;
                columnBody.errorText.text = qsTr("Invalid user password");
            }

            TextFieldWithTitle {
                id: oldPassword;

                width: parent.width;

                title: qsTr("Old Password");

                placeHolderText: qsTr("Enter the password");

                echoMode: TextInput.Password;

                titleFontFamily: Style.fontFamilyBold;

                Component.onCompleted: {
                    columnBodyMain.checkPassword();
                }

                onTextChanged: {
                    console.log("onTextChanged", oldPassword.text);
                    columnBodyMain.checkPassword();
                }

                KeyNavigation.tab: columnBody.passwordInput;
            }

            BaseText{
                id: newPasswordTitle;

                text: qsTr("New Password");

                font.family: Style.fontFamilyBold;
            }

            Rectangle {
                id: newPasswordBlock;

                width: parent.width;
                height: columnBody.height + 25;

                color: "transparent";

                border.width: 1;
                border.color: Style.borderColor;

                PasswordInput {
                    id: columnBody;

                    anchors.centerIn: parent;

                    width: parent.width - 20;

                    spacing: 7;

                    onAcceptedChanged: {
                        root.buttons.setButtonState("Ok", columnBody.accepted)
                    }
                }
            }
        }
    } }

    Component{
        id: emptyDecorator;
        Item{
            property Item rootItem: null;
        }
    }

    Loader{
        id: messageDecoratorLoader;

        sourceComponent: Style.messageDecorator !==undefined ? Style.messageDecorator: emptyDecorator;
        onLoaded: {
            if(messageDecoratorLoader.item){
                messageDecoratorLoader.item.rootItem = root;
            }

            root.width = 300;
        }
    }
}
