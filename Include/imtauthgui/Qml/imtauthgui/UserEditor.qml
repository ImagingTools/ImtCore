import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import Acf 1.0

Item {
    id: userEditorContainer;

    property TreeItemModel documentModel: TreeItemModel {}
    property UndoRedoManager undoRedoManager: null;

    property bool blockUpdatingModel: false;

    Component.onCompleted: {
//        usernameInput.focus = true;
    }

    onDocumentModelChanged: {
        console.log("UserEditor onDocumentModelChanged", documentModel);

        updateGui();
    }

    onUndoRedoManagerChanged: {
        console.log("UserEditor onUndoRedoManagerChanged", undoRedoManager);
    }

    function updateGui(){
        console.log("UserEditor updateGui");

        blockUpdatingModel = true;

        if (documentModel.ContainsKey("Username")){
            usernameInput.text = documentModel.GetData("Username");
        }

        if (documentModel.ContainsKey("Name")){
            nameInput.text = documentModel.GetData("Name");
        }

        if (documentModel.ContainsKey("Email")){
            mailInput.text = documentModel.GetData("Email");
        }

        if (documentModel.ContainsKey("Password")){
            passwordInput.text = documentModel.GetData("Password");
        }

        blockUpdatingModel = false;
    }

    function updateModel(){
        console.log("UserEditor updateModel");

        undoRedoManager.beginChanges();

        documentModel.SetData("Username", usernameInput.text);
        documentModel.SetData("Name", nameInput.text);
        documentModel.SetData("Email", mailInput.text);
        documentModel.SetData("Password", passwordInput.text);

        undoRedoManager.endChanges();
    }

    Component{
        id: emptyDecorator;
        Item{
            property Item rootItem;
        }
    }

    Rectangle {
        id: background;
        anchors.fill: parent;
        color: Style.backgroundColor;
        Loader{
            id: backgroundDecoratorLoader;

            sourceComponent: Style.backGroundDecorator !==undefined ? Style.backGroundDecorator: emptyDecorator;
            onLoaded: {
                if(backgroundDecoratorLoader.item){
                    backgroundDecoratorLoader.item.rootItem = background;
                }
            }
        }
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

                placeHolderText: qsTr("Enter the username");

                onEditingFinished: {
                    console.log("onEditingFinished Username");
                    let oldText = documentModel.GetData("Username");
                    if (oldText != usernameInput.text){
                        updateModel();
                    }
                }

                KeyNavigation.tab: passwordInput;
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

                onEditingFinished: {
                    let oldText = documentModel.GetData("Password");
                    if (oldText != passwordInput.text){
                        updateModel();
                    }
                }

                KeyNavigation.tab: nameInput;
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

                placeHolderText: qsTr("Enter the name");

                onEditingFinished: {
                    let oldText = documentModel.GetData("Name");
                    if (oldText != nameInput.text){
                        updateModel();
                    }
                }

                KeyNavigation.tab: mailInput;
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

                placeHolderText: qsTr("Enter the email");

                onEditingFinished: {
                    let oldText = documentModel.GetData("Email");
                    if (oldText != mailInput.text){
                        updateModel();
                    }
                }

                KeyNavigation.tab: usernameInput;
            }
        }//Column bodyColumn
    }//Flickable
}//Container
