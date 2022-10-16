import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import Acf 1.0

Item {
    id: userEditorContainer;

    function updateGui(){
        console.log("UserEditor updateGui");
        usernameInput.text = documentModel.GetData("Username");
        nameInput.text = documentModel.GetData("Name");
        mailInput.text = documentModel.GetData("Email");
        passwordInput.text = documentModel.GetData("Password");
    }

    Rectangle {
        anchors.fill: parent;

        color: Style.backgroundColor;
    }

    Flickable {
        anchors.fill: parent;
        anchors.leftMargin: 20;

        contentWidth: bodyColumn.width;
        contentHeight: bodyColumn.height + 50;

        boundsBehavior: Flickable.StopAtBounds;

        Column {
            id: bodyColumn;

            width: 400;

            spacing: 7;

            Text {
                id: titleUsername;

                text: qsTr("Username");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            CustomTextField {
                id: usernameInput;

                width: parent.width;
                height: 30;

                placeHolderText: qsTr("Enter the Username");

                onTextChanged: {
                    documentModel.SetData("Username", usernameInput.text);
                }
            }

            Text {
                id: titlePassword;

                text: qsTr("Password");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            CustomTextField {
                id: passwordInput;

                width: parent.width;
                height: 30;

                placeHolderText: qsTr("Enter the password");

                echoMode: TextInput.Password;

                onTextChanged: {
                    documentModel.SetData("Password", passwordInput.text);
                }
            }

            Text {
                id: titleName;

                text: qsTr("Name");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            CustomTextField {
                id: nameInput;

                width: parent.width;
                height: 30;

                placeHolderText: qsTr("Enter the Name");

                onTextChanged: {
                    documentModel.SetData("Name", nameInput.text);
                }
            }

            Text {
                id: titleMail;

                text: qsTr("Email address");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            CustomTextField {
                id: mailInput;

                width: parent.width;
                height: 30;

                placeHolderText: qsTr("Enter the Email");

                onTextChanged: {
                    documentModel.SetData("Email", mailInput.text);
                }
            }

        }//Column bodyColumn
    }//Flickable
}//Container
