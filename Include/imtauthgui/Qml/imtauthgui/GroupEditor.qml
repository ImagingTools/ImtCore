import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import imtlicgui 1.0
import Acf 1.0

Item {
    id: userEditorContainer;

    property TreeItemModel documentModel: TreeItemModel {}
    property UndoRedoManager undoRedoManager: null;

    property TreeItemModel rolesModel: TreeItemModel {}

    property bool blockUpdatingModel: false;

    property int mainMargin: 0;
    property int panelWidth: 400;

    Component.onCompleted: {
        nameInput.focus = true;
    }

    Component.onDestruction: {
        userCollectionProvider.collectionModel.modelChanged.disconnect(userEditorContainer.usersModelChanged);
    }

    onDocumentModelChanged: {
        console.log("UserEditor onDocumentModelChanged", userEditorContainer.documentModel);

        rolesProvider.updateModel();
    }

    onBlockUpdatingModelChanged: {
        Events.sendEvent("DocumentUpdating", userEditorContainer.blockUpdatingModel);
    }

    CollectionDataProvider {
        id: rolesProvider;

        commandId: "Roles";

        fields: ["Id", "Name"];

        onModelUpdated: {
            if (rolesProvider.collectionModel != null){
                if (rolesProvider.collectionModel.ContainsKey("Roles")){
                    let rolesModel = rolesProvider.collectionModel.GetData("Roles");
                    for (let i = 0; i < rolesModel.GetItemsCount(); i++){
                        rolesModel.SetData("CheckedState", Qt.Unchecked, i);
                    }

                    userEditorContainer.rolesModel = rolesModel;
                }
            }

            userCollectionProvider.updateModel();
        }
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

    TreeItemModelConverter {
        id: converter;
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

        let userIds = []
        if (userEditorContainer.documentModel.ContainsKey("Users")){
            let users = userEditorContainer.documentModel.GetData("Users");
            if (users !== ""){
                userIds = users.split(';');
            }
        }

        for (let i = 0; i < usersTable.elements.GetItemsCount(); i++){
            let id = usersTable.elements.GetData("Id", i);
            if (userIds.includes(id)){
                usersTable.elements.SetData("CheckedState", Qt.Checked, i);
            }
            else{
                usersTable.elements.SetData("CheckedState", Qt.Unchecked, i);
            }
        }

        if (userEditorContainer.documentModel.ContainsKey("ChildModel")){
            let childrenModel = userEditorContainer.documentModel.GetData("ChildModel");

            let listModel = converter.convertToListModel(childrenModel);
            childrenGroups.rowModel = listModel;
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

        let childrenModel = converter.convertFromListModel(childrenGroups.rowModel);
        userEditorContainer.documentModel.SetData("ChildModel", childrenModel);

        userEditorContainer.undoRedoManager.endChanges();
        console.log("End updateModel");
    }

    UuidGenerator{
        id: uuidGenerator;
    }

    function onInsert(){
        modalDialogManager.openDialog(addDialog, {});
    }

    function onDelete(){
        let selectedIndex = childrenGroups.selectedIndex;
        if (selectedIndex != null){
            let indexes = selectedIndex.getIndexes();

            childrenGroups.removeRow(indexes);
        }
    }

    function onEdit(){
        let selectedIndex = childrenGroups.selectedIndex;
        if (selectedIndex != null){
            let indexes = selectedIndex.getIndexes();

            let currentRow = childrenGroups.getRow(indexes);
            modalDialogManager.openDialog(editDialog, {"groupName": currentRow["Name"], "groupDescription": currentRow["Description"], "activeRoleIds": currentRow["Roles"]});
        }
    }

    Component {
        id: editDialog;

        EditDialog {
            id: dialog;

            onStarted: {
                dialog.rolesModel.Copy(userEditorContainer.rolesModel);

                dialog.bodyItem.started();
            }

            onFinished: {
                if (buttonId === "Ok"){
                    let selectedIndex = childrenGroups.selectedIndex;
                    if (selectedIndex != null){
                        let indexes = selectedIndex.getIndexes();

                        let newGroupName = dialog.groupName;
                        let newGroupDescription = dialog.groupDescription;

                        let oldRow = childrenGroups.getRow(indexes);

                        childrenGroups.setRow(indexes, {"Id": oldRow["Id"], "Name": newGroupName, "Description": newGroupDescription, "Roles": dialog.bodyItem.activeRoleIds});

                        updateModel();
                        updateGui();
                    }
                }
            }
        }
    }

    Component {
        id: addDialog;

        EditDialog {
            id: dialog;

            onFinished: {
                if (buttonId === "Ok"){
                    let selectedIndex = childrenGroups.selectedIndex;
                    let uuid = uuidGenerator.generateUUID();

                    let newGroupName = dialog.groupName;
                    let newGroupDescription = dialog.groupDescription;

                    let rowObj = {"Id": uuid, "Name": newGroupName, "Description": newGroupDescription, "Roles": []};
                    if (selectedIndex != null){
                        let indexes = selectedIndex.getIndexes();

                        let childrenIndexes = selectedIndex.childModel;
                        indexes.push(childrenIndexes.length);

                        childrenGroups.insertRow(indexes, rowObj);
                    }
                    else{
                        childrenGroups.addRow(rowObj);
                    }

                    updateModel();
                    updateGui();
                }
            }
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

        Item{
            id: columnContainer;

            width: userEditorContainer.panelWidth;
            height: bodyColumn.height + 2*bodyColumn.anchors.topMargin;

            Loader{
                id: mainPanelFrameLoader;

                anchors.fill: parent;

                sourceComponent: Style.frame !==undefined ? Style.frame: emptyDecorator;
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

                Text {
                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;

                    text: qsTr("Children Groups");
                }

                TreeItemModel {
                    id: commandsModel;

                    Component.onCompleted: {
                        let index = commandsModel.InsertNewItem();

                        commandsModel.SetData("Id", "Insert", index);
                        commandsModel.SetData("Name", "Insert", index);
                        commandsModel.SetData("IsEnabled", true, index);
                        commandsModel.SetData("Icon", "Add", index);

                        index = commandsModel.InsertNewItem();

                        commandsModel.SetData("Id", "Delete", index);
                        commandsModel.SetData("Name", "Delete", index);
                        commandsModel.SetData("IsEnabled", false, index);
                        commandsModel.SetData("Icon", "Delete", index);

                        index = commandsModel.InsertNewItem();

                        commandsModel.SetData("Id", "Edit", index);
                        commandsModel.SetData("Name", "Edit", index);
                        commandsModel.SetData("IsEnabled", false, index);
                        commandsModel.SetData("Icon", "Edit", index);

                        commands.commandModel = commandsModel;
                    }
                }

                Item{
                    width: parent.width;
                    height: rect.height + childrenGroups.height;

                    Rectangle {
                        id: rect;

                        width: parent.width;
                        height: 25;

                        color: Style.alternateBaseColor;

                        SimpleCommandsDecorator {
                            id: commands;

                            anchors.horizontalCenter: parent.horizontalCenter;
                            anchors.verticalCenter: parent.verticalCenter;

                            height: 20;

                            color: parent.color;

                            onCommandActivated: {
                                if (commandId == "Insert"){
                                    userEditorContainer.onInsert();
                                }
                                else if (commandId == "Delete"){
                                    userEditorContainer.onDelete();
                                }
                                else if (commandId == "Edit"){
                                    userEditorContainer.onEdit();
                                }
                            }
                        }
                    }

                    BasicTreeView {
                        id: childrenGroups;

                        anchors.top: rect.bottom;

                        width: bodyColumn.width;
                        height: 300;

                        Component.onCompleted: {
                            childrenGroups.addColumn({"Id" : "Name", "Name": "Group Name"})
                            childrenGroups.addColumn({"Id" : "Description", "Name": "Description"})
                        }

                        onSelectedIndexChanged: {
                            let ok = childrenGroups.selectedIndex != null;

                            commandsModel.SetData("IsEnabled", ok, 1);
                            commandsModel.SetData("IsEnabled", ok, 2);
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

                Text {
                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;

                    text: qsTr("Users");

                    visible: false;
                }

                AuxTable {
                    id: usersTable;

                    width: bodyColumn.width;
                    height: 300;

                    checkable: true;

                    visible: false;

                    radius: 0;
                }
            }//Column bodyColumn
        }//columnContainer
        //
    }

}//Container
