import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import imtlicgui 1.0
import Acf 1.0

Item {
    id: userGroupEditorContainer;

    property TreeItemModel documentModel: TreeItemModel {}
    property UndoRedoManager undoRedoManager: null;

    property bool blockUpdatingModel: false;

    property int mainMargin: 0;
    property int panelWidth: 400;

    onDocumentModelChanged: {
        console.log("UserEditor onDocumentModelChanged", userGroupEditorContainer.documentModel);

        groupsProvider.updateModel();
    }

    onBlockUpdatingModelChanged: {
        Events.sendEvent("DocumentUpdating", userGroupEditorContainer.blockUpdatingModel);
    }

    function getAllChildrenGroups(groupId, retVal){
        for (let i = 0; i < groupsProvider.collectionModel.GetItemsCount(); i++){
            let id = groupsProvider.collectionModel.GetData("Id", i);
            let parentGroups = groupsProvider.collectionModel.GetData("ParentGroups", i);
            if (parentGroups !== ""){
                let parentGroupIds = parentGroups.split(';')
                if (parentGroupIds.includes(groupId)){
                    retVal.push(id);

                    userGroupEditorContainer.getAllChildrenGroups(id, retVal);
                }
            }
        }
    }

    function getAllParentGroupIds(groupId, retVal){
        for (let i = 0; i < groupsProvider.collectionModel.GetItemsCount(); i++){
            let id = groupsProvider.collectionModel.GetData("Id", i);
            if (id === groupId){
                let parentGroups = groupsProvider.collectionModel.GetData("ParentGroups", i);
                if (parentGroups !== ""){
                    let parentGroupsIds = parentGroups.split(';');
                    for (let j = 0; j < parentGroupsIds.length; j++){
                        retVal.push(parentGroupsIds[j])
                        userGroupEditorContainer.getAllParentGroupIds(parentGroupsIds[j], retVal);
                    }
                }
            }
        }
    }

    CollectionDataProvider {
        id: groupsProvider;

        commandId: "Groups";
        fields: ["Id", "Name", "Description", "ParentGroups", "Roles"];

        Component.onDestruction: {
            groupsProvider.collectionModel.modelChanged.disconnect(userGroupEditorContainer.updateModel);
        }

        onModelUpdated: {
            if (groupsProvider.collectionModel != null){
                let documentId = userGroupEditorContainer.documentModel.GetData("Id");
                let removedIndexes = []

                let childrenIds = []
                userGroupEditorContainer.getAllChildrenGroups(documentId, childrenIds);

                // Get all parent ID-s
                let parentIds = []
                if (userGroupEditorContainer.documentModel.ContainsKey("ParentGroups")){
                    let parentGroups = userGroupEditorContainer.documentModel.GetData("ParentGroups")
                    let parentGroupIds = parentGroups.split(';')
                    for (let j = 0; j < parentGroupIds.length; j++){
                        userGroupEditorContainer.getAllParentGroupIds(parentGroupIds[j], parentIds);
                    }
                }

                console.log("parentIds", parentIds);
                console.log("childrenIds", childrenIds);

                for (let i = 0; i < groupsProvider.collectionModel.GetItemsCount(); i++){
                    let id = groupsProvider.collectionModel.GetData("Id", i);
                    if (id === documentId || childrenIds.includes(id)){
                        removedIndexes.push(i);
                    }

                    groupsProvider.collectionModel.SetData("CheckedState", Qt.Unchecked, i);
                }

                console.log("removedIndexes", removedIndexes);

                let removedCount = 0
                for (let i = 0; i < removedIndexes.length; i++){
                    groupsProvider.collectionModel.RemoveItem(removedIndexes[i] - removedCount);
                    removedCount++;
                }

                parentGroupsTable.elements = groupsProvider.collectionModel;

                userGroupEditorContainer.updateGui();
                groupsProvider.collectionModel.modelChanged.connect(userGroupEditorContainer.updateModel);

                nameInput.focus = true;
            }
        }
    }

    function updateGui(){
        console.log("GroupEditor updateGui");

        userGroupEditorContainer.blockUpdatingModel = true;

        if (userGroupEditorContainer.documentModel.ContainsKey("Name")){
            nameInput.text = userGroupEditorContainer.documentModel.GetData("Name");
        }

        if (userGroupEditorContainer.documentModel.ContainsKey("Description")){
            descriptionInput.text = userGroupEditorContainer.documentModel.GetData("Description");
        }

        let parentGroupIds = []
        if (userGroupEditorContainer.documentModel.ContainsKey("ParentGroups")){
            let parentGroups = userGroupEditorContainer.documentModel.GetData("ParentGroups");
            if (parentGroups !== ""){
                parentGroupIds = parentGroups.split(';')
            }
        }

        for (let i = 0; i < parentGroupsTable.elements.GetItemsCount(); i++){
            let id = parentGroupsTable.elements.GetData("Id", i);
            if (parentGroupIds.includes(id)){
                parentGroupsTable.elements.SetData("CheckedState", Qt.Checked, i);
            }
            else{
                parentGroupsTable.elements.SetData("CheckedState", Qt.Unchecked, i);
            }
        }

        userGroupEditorContainer.blockUpdatingModel = false;
    }

    function updateModel(){
        console.log("GroupEditor updateModel");
        if (userGroupEditorContainer.blockUpdatingModel){
            return;
        }

        userGroupEditorContainer.undoRedoManager.beginChanges();

        userGroupEditorContainer.documentModel.SetData("Description", descriptionInput.text);
        userGroupEditorContainer.documentModel.SetData("Name", nameInput.text);

        let selectedRoleIds = []
        let selectedGroupIds = []
        for (let i = 0; i < parentGroupsTable.elements.GetItemsCount(); i++){
            let id = parentGroupsTable.elements.GetData("Id", i);
            let state = parentGroupsTable.elements.GetData("CheckedState", i);
            if (state === Qt.Checked){
                selectedGroupIds.push(id);
                let roles = parentGroupsTable.elements.GetData("Roles", i);
                let roleIds = roles.split(';')
                for (let j = 0; j < roleIds.length; j++){
                    if (!selectedRoleIds.includes(roleIds[j])){
                        selectedRoleIds.push(roleIds[j])
                    }
                }
            }
        }
        userGroupEditorContainer.documentModel.SetData("ParentGroups", selectedGroupIds.join(';'));

        let ok = true;
        if (userGroupEditorContainer.documentModel.ContainsKey("Roles")){
            let userRoles = userGroupEditorContainer.documentModel.GetData("Roles");
            if (userRoles !== ""){
                ok = false;

                let userRoleIds = userRoles.split(';');
                for (let j = 0; j < selectedRoleIds.length; j++){
                    if (!userRoleIds.includes(selectedRoleIds[j])){
                        userRoleIds.push(selectedRoleIds[j])
                    }
                }
                //  userGroupEditorContainer.documentModel.SetData("Roles", userRoleIds.join(';'));
            }
        }

        if (ok){
            // userGroupEditorContainer.documentModel.SetData("Roles", selectedRoleIds.join(';'));
        }

        userGroupEditorContainer.undoRedoManager.endChanges();
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
    }

    Item{
        id: columnContainer;

        anchors.top: parent.top;
        anchors.left: parent.left;

        width: userGroupEditorContainer.panelWidth;
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
            anchors.topMargin: userGroupEditorContainer.mainMargin;
            anchors.leftMargin: userGroupEditorContainer.mainMargin;

            width: userGroupEditorContainer.panelWidth - 2*anchors.leftMargin;

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
                    let oldText = userGroupEditorContainer.documentModel.GetData("Name");
                    if (oldText && oldText !== nameInput.text || !oldText && nameInput.text !== ""){
                        userGroupEditorContainer.updateModel();
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

                KeyNavigation.tab: descriptionInput;
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
                    let oldText = userGroupEditorContainer.documentModel.GetData("Description");
                    if (oldText && oldText !== descriptionInput.text || !oldText && descriptionInput.text !== ""){
                        userGroupEditorContainer.updateModel();
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
        }//Column bodyColumn
    }//columnContainer

    Text {
        id: title;
        anchors.top: columnContainer.bottom;
        anchors.topMargin: 10;

        color: Style.textColor;
        font.family: Style.fontFamily;
        font.pixelSize: Style.fontSize_common;

        text: qsTr("Parent Groups");
    }

    TreeItemModel {
        id: groupsHeadersModel;

        Component.onCompleted: {
            let index = groupsHeadersModel.InsertNewItem();

            groupsHeadersModel.SetData("Id", "Name");
            groupsHeadersModel.SetData("Name", "Group Name");

            parentGroupsTable.headers = groupsHeadersModel;
        }
    }

    AuxTable {
        id: parentGroupsTable;

        anchors.top: title.bottom;
        anchors.topMargin: 10;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 10;

        width: 400;

        elements: TreeItemModel {}
        checkable: true;
        radius: 0;
    }

    //    Rectangle {
    //        id: background;

    //        anchors.fill: parent;

    //        color: Style.backgroundColor;
    //        Loader{
    //            id: backgroundDecoratorLoader;

    //            sourceComponent: Style.backGroundDecorator !==undefined ? Style.backGroundDecorator: emptyDecorator;
    //            onLoaded: {
    //                if(backgroundDecoratorLoader.item){
    //                    backgroundDecoratorLoader.item.rootItem = background;
    //                }
    //            }
    //        }

    //        Item{
    //            id: columnContainer;

    //            width: userGroupEditorContainer.panelWidth;
    //            height: bodyColumn.height + 2*bodyColumn.anchors.topMargin;

    //            Loader{
    //                id: mainPanelFrameLoader;

    //                anchors.fill: parent;

    //                sourceComponent: Style.frame !==undefined ? Style.frame: emptyDecorator;
    //            }//Loader

    //            Column {
    //                id: bodyColumn;

    //                anchors.top: parent.top;
    //                anchors.left: parent.left;
    //                anchors.topMargin: userGroupEditorContainer.mainMargin;
    //                anchors.leftMargin: userGroupEditorContainer.mainMargin;

    //                width: userGroupEditorContainer.panelWidth - 2*anchors.leftMargin;

    //                spacing: 10;

    //                Text {
    //                    id: titleName;

    //                    color: Style.textColor;
    //                    font.family: Style.fontFamily;
    //                    font.pixelSize: Style.fontSize_common;

    //                    text: qsTr("Group Name");
    //                }

    //                CustomTextField {
    //                    id: nameInput;

    //                    width: parent.width;
    //                    height: 30;

    //                    placeHolderText: qsTr("Enter the name");

    //                    onEditingFinished: {
    //                        let oldText = userGroupEditorContainer.documentModel.GetData("Name");
    //                        if (oldText && oldText !== nameInput.text || !oldText && nameInput.text !== ""){
    //                            userGroupEditorContainer.updateModel();
    //                        }
    //                    }

    //                    Loader{
    //                        id: inputDecoratorLoader3;

    //                        sourceComponent: Style.textFieldDecorator !==undefined ? Style.textFieldDecorator: emptyDecorator;
    //                        onLoaded: {
    //                            if(inputDecoratorLoader3.item){
    //                                inputDecoratorLoader3.item.rootItem = nameInput;
    //                            }
    //                        }
    //                    }

    //                    KeyNavigation.tab: descriptionInput;
    //                }

    //                Text {
    //                    id: titleDescription;

    //                    color: Style.textColor;
    //                    font.family: Style.fontFamily;
    //                    font.pixelSize: Style.fontSize_common;

    //                    text: qsTr("Description");
    //                }

    //                CustomTextField {
    //                    id: descriptionInput;

    //                    width: parent.width;
    //                    height: 30;

    //                    placeHolderText: qsTr("Enter the description");

    //                    onEditingFinished: {
    //                        let oldText = userGroupEditorContainer.documentModel.GetData("Description");
    //                        if (oldText && oldText !== descriptionInput.text || !oldText && descriptionInput.text !== ""){
    //                            userGroupEditorContainer.updateModel();
    //                        }
    //                    }

    //                    KeyNavigation.tab: nameInput;

    //                    Loader{
    //                        id: inputDecoratorLoader4;

    //                        sourceComponent: Style.textFieldDecorator !==undefined ? Style.textFieldDecorator: emptyDecorator;
    //                        onLoaded: {
    //                            if(inputDecoratorLoader4.item){
    //                                inputDecoratorLoader4.item.rootItem = nameInput;
    //                            }
    //                        }
    //                    }
    //                }

    //                Text {
    //                    color: Style.textColor;
    //                    font.family: Style.fontFamily;
    //                    font.pixelSize: Style.fontSize_common;

    //                    text: qsTr("Parent Groups");
    //                }

    //                TreeItemModel {
    //                    id: groupsHeadersModel;

    //                    Component.onCompleted: {
    //                        let index = groupsHeadersModel.InsertNewItem();

    //                        groupsHeadersModel.SetData("Id", "Name");
    //                        groupsHeadersModel.SetData("Name", "Group Name");

    //                        parentGroupsTable.headers = groupsHeadersModel;
    //                    }
    //                }

    //                AuxTable {
    //                    id: parentGroupsTable;

    //                    width: parent.width;
    //                    height: 300;

    //                    checkable: true;

    //                    radius: 0;
    //                }
    //            }//Column bodyColumn
    //        }//columnContainer
    //        //
    //    }

}//Container
