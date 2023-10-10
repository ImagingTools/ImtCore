import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import Acf 1.0

Item {
    id: userEditorContainer;

    property TreeItemModel documentModel: TreeItemModel {}
    property UndoRedoManager undoRedoManager: null;

    property bool blockUpdatingModel: false;

    property int mainMargin: 0;
    property int panelWidth: 400;
    property int radius: 3;

    onDocumentModelChanged: {
        console.log("UserEditor onDocumentModelChanged", userEditorContainer.documentModel);
        userEditorContainer.updateGui();
        usernameInput.focus = true;
    }

    onBlockUpdatingModelChanged: {
        Events.sendEvent("DocumentUpdating", userEditorContainer.blockUpdatingModel);
    }

    onUndoRedoManagerChanged: {
        console.log("UserEditor onUndoRedoManagerChanged", undoRedoManager);
    }

    function blockEditing(){
        usernameInput.readOnly = true;
        nameInput.readOnly = true;
        mailInput.readOnly = true;
        passwordInput.readOnly = true;
    }

    function updateGui(){
        console.log("UserEditor updateGui");

        userEditorContainer.blockUpdatingModel = true;

        if (userEditorContainer.documentModel.ContainsKey("Username")){
            usernameInput.text = userEditorContainer.documentModel.GetData("Username");
        }

        if (userEditorContainer.documentModel.ContainsKey("Name")){
            nameInput.text = userEditorContainer.documentModel.GetData("Name");
        }

        if (userEditorContainer.documentModel.ContainsKey("Email")){
            mailInput.text = userEditorContainer.documentModel.GetData("Email");
        }

        if (userEditorContainer.documentModel.ContainsKey("Password")){
            passwordInput.text = userEditorContainer.documentModel.GetData("Password");
        }

        userEditorContainer.blockUpdatingModel = false;
    }

    function updateModel(){
        console.log("UserEditor updateModel");

        if (userEditorContainer.undoRedoManager){
            userEditorContainer.undoRedoManager.beginChanges();
        }

        userEditorContainer.documentModel.SetData("Username", usernameInput.text);
        userEditorContainer.documentModel.SetData("Name", nameInput.text);
        userEditorContainer.documentModel.SetData("Email", mailInput.text);

        let ok = true;
        if (userEditorContainer.documentModel.ContainsKey("Password")){
            let oldPassword = userEditorContainer.documentModel.GetData("Password");
            console.log("oldPassword", oldPassword);
            if (passwordInput.text != oldPassword){
                ok = false;
            }
        }

        if (!ok){
            let hash = passwordInput.text;
            userEditorContainer.documentModel.SetData("Password", hash);
        }

        if (userEditorContainer.undoRedoManager){
            userEditorContainer.undoRedoManager.endChanges();
        }
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
        Loader{
            id: backgroundDecoratorLoader;

            sourceComponent: Style.backGroundDecorator !==undefined ? Style.backGroundDecorator: emptyDecorator;
            onLoaded: {
                if(backgroundDecoratorLoader.item){
                    backgroundDecoratorLoader.item.rootItem = background;
                }
            }
        }

        //
        Item{
            id: columnContainer;

            width: userEditorContainer.panelWidth;
            height: bodyColumn.height + 2*bodyColumn.anchors.topMargin;
            Loader{
                id: mainPanelFrameLoader;

                anchors.fill: parent;

                sourceComponent: Style.frame !==undefined ? Style.frame: emptyDecorator;

                onLoaded: {
                    if(mainPanelFrameLoader.item){
                        // userEditorContainer.mainMargin = mainPanelFrameLoader.item.mainMargin;
                    }
                }
            }//Loader

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
//                    anchors.leftMargin: 5;

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
                        let oldText = userEditorContainer.documentModel.GetData("Username");
                        if (oldText && oldText !== usernameInput.text || !oldText && usernameInput.text !== ""){
                            userEditorContainer.updateModel();
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

                    anchors.left: parent.left;
//                    anchors.leftMargin: 5;

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
                        let oldText = userEditorContainer.documentModel.GetData("Password");
                        if (oldText && oldText !== passwordInput.text || !oldText && passwordInput.text !== ""){
                            userEditorContainer.updateModel();
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

                    anchors.left: parent.left;
//                    anchors.leftMargin: 5;

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
                        let oldText = userEditorContainer.documentModel.GetData("Name");
                        if (oldText && oldText !== nameInput.text || !oldText && nameInput.text !== ""){
                            userEditorContainer.updateModel();
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

                    anchors.left: parent.left;
//                    anchors.leftMargin: 5;

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

                RegExpValidator {
                    id: mailValid;

                    regExp: /\w+([-+.']\w+)*@\w+([-.]\w+)*\.\w+([-.]\w+)*/;
                }

                CustomTextField {
                    id: mailInput;

                    width: parent.width;
                    height: 30;

                    textInputValidator: mailValid;

                    placeHolderText: qsTr("Enter the email");

                    onEditingFinished: {
                        let oldText = userEditorContainer.documentModel.GetData("Email");
                        if (oldText && oldText !== mailInput.text || !oldText && mailInput.text !== ""){
                            userEditorContainer.updateModel();
                        }
                    }

                    onTextEdited: {
                        console.log("onTextEdited");
                    }

                    onTextChanged: {
                        console.log("onTextChanged");
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

        }//columnContainer
        //
    }

}//Container
