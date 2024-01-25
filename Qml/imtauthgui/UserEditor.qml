import QtQuick 2.15
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

Item {
    id: userEditorContainer;

    property TreeItemModel documentModel: TreeItemModel {}
    property Item documentPtr: null;

    property int mainMargin: 0;
    property int panelWidth: 400;
    property int radius: 3;

    function blockEditing(){
        usernameInput.readOnly = true;
        nameInput.readOnly = true;
        mailInput.readOnly = true;
        passwordInput.readOnly = true;
    }

    function updateGui(){
        if (userEditorContainer.documentModel.ContainsKey("Username")){
            usernameInput.text = userEditorContainer.documentModel.GetData("Username");
        }
        else{
            usernameInput.text = "";
        }

        if (userEditorContainer.documentModel.ContainsKey("Name")){
            nameInput.text = userEditorContainer.documentModel.GetData("Name");
        }
        else{
            nameInput.text = "";
        }

        if (userEditorContainer.documentModel.ContainsKey("Email")){
            mailInput.text = userEditorContainer.documentModel.GetData("Email");
        }
        else{
            mailInput.text = "";
        }

        if (userEditorContainer.documentModel.ContainsKey("Password")){
            passwordInput.text = userEditorContainer.documentModel.GetData("Password");
        }
        else{
            passwordInput.text = "";
        }
    }

    function updateModel(){
        userEditorContainer.documentModel.SetData("Username", usernameInput.text);
        userEditorContainer.documentModel.SetData("Name", nameInput.text);
        userEditorContainer.documentModel.SetData("Email", mailInput.text);
        userEditorContainer.documentModel.SetData("Password", passwordInput.text);
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

                    onEditingFinished: {
                        if (userEditorContainer.documentPtr){
                            let oldText = userEditorContainer.documentModel.GetData("Username");
                            if (oldText && oldText !== usernameInput.text || !oldText && usernameInput.text !== ""){
                                userEditorContainer.documentPtr.doUpdateModel();
                            }
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

                    onEditingFinished: {
                        if (userEditorContainer.documentPtr){
                            let oldText = userEditorContainer.documentModel.GetData("Password");
                            if (oldText && oldText !== passwordInput.text || !oldText && passwordInput.text !== ""){
                                userEditorContainer.documentPtr.doUpdateModel();
                            }
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

                    onEditingFinished: {
                        if (userEditorContainer.documentPtr){
                            let oldText = userEditorContainer.documentModel.GetData("Name");
                            if (oldText && oldText !== nameInput.text || !oldText && nameInput.text !== ""){
                                userEditorContainer.documentPtr.doUpdateModel();
                            }
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

                    onEditingFinished: {
                        if (userEditorContainer.documentPtr){
                            userEditorContainer.documentPtr.doUpdateModel();
                        }
                    }

                    KeyNavigation.tab: usernameInput;
                }
            }//Column bodyColumn
        }//columnContainer
    }
}//Container
