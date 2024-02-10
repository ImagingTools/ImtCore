import QtQuick 2.15
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

ViewBase {
    id: userEditorContainer;

    property int mainMargin: 0;
    property int panelWidth: 400;

    function updateGui(){
        if (userEditorContainer.model.ContainsKey("Username")){
            usernameInput.text = userEditorContainer.model.GetData("Username");
        }
        else{
            usernameInput.text = "";
        }

        if (userEditorContainer.model.ContainsKey("Name")){
            nameInput.text = userEditorContainer.model.GetData("Name");
        }
        else{
            nameInput.text = "";
        }

        if (userEditorContainer.model.ContainsKey("Email")){
            mailInput.text = userEditorContainer.model.GetData("Email");
        }
        else{
            mailInput.text = "";
        }

        if (userEditorContainer.model.ContainsKey("Password")){
            passwordInput.text = userEditorContainer.model.GetData("Password");
        }
        else{
            passwordInput.text = "";
        }
    }

    function updateModel(){
        userEditorContainer.model.SetData("Username", usernameInput.text);
        userEditorContainer.model.SetData("Name", nameInput.text);
        userEditorContainer.model.SetData("Email", mailInput.text);
        userEditorContainer.model.SetData("Password", passwordInput.text);
    }

    Component{
        id: emptyDecorator;
        Item{
            property Item rootItem: null;
        }
    }

    Rectangle {
        id: background;

        anchors.fill: parent;

        color: Style.backgroundColor;

        Item{
            id: columnContainer;

            width: userEditorContainer.panelWidth;
            height: bodyColumn.height + 2*bodyColumn.anchors.topMargin;

            Column {
                id: bodyColumn;

                anchors.top: parent.top;
                anchors.left: parent.left;
                anchors.topMargin: userEditorContainer.mainMargin;
                anchors.leftMargin: userEditorContainer.mainMargin;

                width: userEditorContainer.panelWidth - 2*anchors.leftMargin;

                spacing: 10;

                Item{
                    width: parent.width;
                    height: 1;
                }

                Text {
                    id: titleUsername;

                    anchors.left: parent.left;

                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;

                    text: qsTr("Username");
                }

                CustomTextField {
                    id: usernameInput;

                    width: parent.width;
                    height: 30;

                    placeHolderText: qsTr("Enter the username");

                    readOnly: userEditorContainer.readOnly;

                    onEditingFinished: {
                            let oldText = userEditorContainer.model.GetData("Username");
                            if (oldText && oldText !== usernameInput.text || !oldText && usernameInput.text !== ""){
                                userEditorContainer.doUpdateModel();
                            }
                    }

                    KeyNavigation.tab: passwordInput;
                }

                Text {
                    id: titlePassword;

                    anchors.left: parent.left;

                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;

                    text: qsTr("Password");
                }

                CustomTextField {
                    id: passwordInput;

                    width: parent.width;
                    height: 30;

                    placeHolderText: qsTr("Enter the password");

                    echoMode: TextInput.Password;

                    readOnly: userEditorContainer.readOnly;

                    onEditingFinished: {
                            let oldText = userEditorContainer.model.GetData("Password");
                            if (oldText && oldText !== passwordInput.text || !oldText && passwordInput.text !== ""){
                                userEditorContainer.doUpdateModel();
                            }
                    }

                    KeyNavigation.tab: nameInput;
                }

                Text {
                    id: titleName;

                    anchors.left: parent.left;

                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;

                    text: qsTr("Name");

                }

                CustomTextField {
                    id: nameInput;

                    width: parent.width;
                    height: 30;

                    placeHolderText: qsTr("Enter the name");
                    readOnly: userEditorContainer.readOnly;

                    onEditingFinished: {
                        let oldText = userEditorContainer.model.GetData("Name");
                        if (oldText && oldText !== nameInput.text || !oldText && nameInput.text !== ""){
                            userEditorContainer.doUpdateModel();
                        }
                    }

                    KeyNavigation.tab: mailInput;
                }

                Text {
                    id: titleMail;

                    anchors.left: parent.left;

                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;

                    text: qsTr("Email address");
                }

                RegularExpressionValidator {
                    id: mailValid;

                    regularExpression: /\w+([-+.']\w+)*@\w+([-.]\w+)*\.\w+([-.]\w+)*/;
                }

                CustomTextField {
                    id: mailInput;

                    width: parent.width;
                    height: 30;

                    textInputValidator: mailValid;

                    placeHolderText: qsTr("Enter the email");
                    readOnly: userEditorContainer.readOnly;

                    onEditingFinished: {
                        userEditorContainer.doUpdateModel();
                    }

                    KeyNavigation.tab: usernameInput;
                }
            }//Column bodyColumn
        }//columnContainer
    }
}//Container
