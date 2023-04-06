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

    Component.onCompleted: {
        nameInput.focus = true;

        userCollectionProvider.updateModel();
    }

    Component.onDestruction: {
        userCollectionProvider.collectionModel.modelChanged.disconnect(userEditorContainer.usersModelChanged);
    }

    onDocumentModelChanged: {
        console.log("UserEditor onDocumentModelChanged", userEditorContainer.documentModel);
    }

    onBlockUpdatingModelChanged: {
        Events.sendEvent("DocumentUpdating", userEditorContainer.blockUpdatingModel);
    }

    CollectionDataProvider {
        id: userCollectionProvider;

        commandId: "Users";

        fields: ["UserId", "Name"];

        onModelUpdated: {

            for (let i = 0; i < userCollectionProvider.collectionModel.GetItemsCount(); i++){
                let userId = userCollectionProvider.collectionModel.GetData("UserId", i);
                let userName = userCollectionProvider.collectionModel.GetData("Name", i);

                userCollectionProvider.collectionModel.SetData("CheckedState", Qt.Unchecked, i);
            }

            usersTable.elements = userCollectionProvider.collectionModel;
            userEditorContainer.updateGui();

            userCollectionProvider.collectionModel.modelChanged.connect(userEditorContainer.usersModelChanged);
        }
    }

    function usersModelChanged(){
        console.log("usersModelChanged");
        userEditorContainer.updateModel();
    }

    function updateGui(){
        console.log("UserEditor updateGui");

        userEditorContainer.blockUpdatingModel = true;

        if (userEditorContainer.documentModel.ContainsKey("Name")){
            nameInput.text = userEditorContainer.documentModel.GetData("Name");
        }

        if (userEditorContainer.documentModel.ContainsKey("Description")){
            descriptionInput.text = userEditorContainer.documentModel.GetData("Description");
        }

        if (userEditorContainer.documentModel.ContainsKey("Users")){
            let users = userEditorContainer.documentModel.GetData("Users");
            if (users !== ""){
                let userIds = users.split(';');
                for (let i = 0; i < usersTable.elements.GetItemsCount(); i++){
                    let id = usersTable.elements.GetData("Id", i);
                    if (userIds.includes(id)){
                        usersTable.elements.SetData("CheckedState", Qt.Checked, i);
                    }
                }
            }
        }

        userEditorContainer.blockUpdatingModel = false;
    }

    function updateModel(){
        console.log("UserEditor updateModel");
        if (userEditorContainer.blockUpdatingModel){
            return;
        }

        userEditorContainer.undoRedoManager.beginChanges();

        userEditorContainer.documentModel.SetData("Description", descriptionInput.text);
        userEditorContainer.documentModel.SetData("Name", nameInput.text);

        let selectedUserIds = []
        for (let i = 0; i < usersTable.elements.GetItemsCount(); i++){
            let id = usersTable.elements.GetData("Id", i);
            let state = usersTable.elements.GetData("CheckedState", i);
            if (state === Qt.Checked){
                selectedUserIds.push(id)
            }
        }

        let result = selectedUserIds.join(';');
        userEditorContainer.documentModel.SetData("Users", result);

        userEditorContainer.undoRedoManager.endChanges();
        console.log("End updateModel");
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

                Text {
                    id: titleName;

                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;

                    text: qsTr("Group Name");
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
                    id: titleDescription;

                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;

                    text: qsTr("Description");
                }

                CustomTextField {
                    id: descriptionInput;

                    width: parent.width;
                    height: 30;

                    placeHolderText: qsTr("Enter the description");

                    onEditingFinished: {
                        let oldText = userEditorContainer.documentModel.GetData("Description");
                        if (oldText && oldText !== descriptionInput.text || !oldText && descriptionInput.text !== ""){
                            userEditorContainer.updateModel();
                        }
                    }

                    KeyNavigation.tab: nameInput;

                    Loader{
                        id: inputDecoratorLoader4;

                        sourceComponent: Style.textFieldDecorator !==undefined ? Style.textFieldDecorator: emptyDecorator;
                        onLoaded: {
                            if(inputDecoratorLoader4.item){
                                inputDecoratorLoader4.item.rootItem = nameInput;
                            }
                        }
                    }
                }

                TreeItemModel {
                    id: headersModel;

                    Component.onCompleted: {
                        headersModel.InsertNewItem();

                        headersModel.SetData("Id", "Name");
                        headersModel.SetData("Name", "Username");

                        usersTable.headers = headersModel;
                    }
                }

                AuxTable {
                    id: usersTable;

                    width: bodyColumn.width;
                    height: 300;

                    checkable: true;
                }
            }//Column bodyColumn

        }//columnContainer
        //
    }

}//Container
