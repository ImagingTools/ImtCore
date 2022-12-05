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

            spacing: 10;

            Text {
                id: titleUsername;

                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;

                text: qsTr("Username");

                Loader{
                    id: titleDecoratorLoader1;

                    sourceComponent: Style.inputTitleDecorator !==undefined ? Style.inputTitleDecorator: emptyDecorator;
                    onLoaded: {
                        if(titleDecoratorLoader1.item){
                            titleDecoratorLoader1.item.rootItem = titleUsername;
                        }
                    }
                }
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

                Loader{
                    id: inputDecoratorLoader1;

                    sourceComponent: Style.textFieldDecorator !==undefined ? Style.textFieldDecorator: emptyDecorator;
                    onLoaded: {
                        if(inputDecoratorLoader1.item){
                            inputDecoratorLoader1.item.rootItem = usernameInput;
                        }
                    }
                }
            }

            Text {
                id: titlePassword;

                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;

                text: qsTr("Password");

                Loader{
                    id: titleDecoratorLoader2;

                    sourceComponent: Style.inputTitleDecorator !==undefined ? Style.inputTitleDecorator: emptyDecorator;
                    onLoaded: {
                        if(titleDecoratorLoader2.item){
                            titleDecoratorLoader2.item.rootItem = titlePassword;
                        }
                    }
                }
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

                Loader{
                    id: inputDecoratorLoader2;

                    sourceComponent: Style.textFieldDecorator !==undefined ? Style.textFieldDecorator: emptyDecorator;
                    onLoaded: {
                        if(inputDecoratorLoader2.item){
                            inputDecoratorLoader2.item.rootItem = passwordInput;
                        }
                    }
                }
            }

            Text {
                id: titleName;

                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;

                text: qsTr("Name");

                Loader{
                    id: titleDecoratorLoader3;

                    sourceComponent: Style.inputTitleDecorator !==undefined ? Style.inputTitleDecorator: emptyDecorator;
                    onLoaded: {
                        if(titleDecoratorLoader3.item){
                            titleDecoratorLoader3.item.rootItem = titleName;
                        }
                    }
                }
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

                Loader{
                    id: inputDecoratorLoader3;

                    sourceComponent: Style.textFieldDecorator !==undefined ? Style.textFieldDecorator: emptyDecorator;
                    onLoaded: {
                        if(inputDecoratorLoader3.item){
                            inputDecoratorLoader3.item.rootItem = nameInput;
                        }
                    }
                }
            }

            Text {
                id: titleMail;

                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;

                text: qsTr("Email address");

                Loader{
                    id: titleDecoratorLoader4;

                    sourceComponent: Style.inputTitleDecorator !==undefined ? Style.inputTitleDecorator: emptyDecorator;
                    onLoaded: {
                        if(titleDecoratorLoader4.item){
                            titleDecoratorLoader4.item.rootItem = titleMail;
                        }
                    }
                }
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

                Loader{
                    id: inputDecoratorLoader4;

                    sourceComponent: Style.textFieldDecorator !==undefined ? Style.textFieldDecorator: emptyDecorator;
                    onLoaded: {
                        if(inputDecoratorLoader4.item){
                            inputDecoratorLoader4.item.rootItem = mailInput;
                        }
                    }
                }
            }
        }//Column bodyColumn
    }//Flickable
}//Container
