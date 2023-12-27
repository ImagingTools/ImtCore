import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtqml 1.0
import imtcolgui 1.0
import imtlicgui 1.0

Item {
    id: userGroupEditorContainer;

    property TreeItemModel documentModel: TreeItemModel {}
    property Item documentPtr: null;

    property int mainMargin: 0;
    property int panelWidth: 400;

    property int radius: 3;

    property bool completed: groupsProvider.compl;

    Component.onCompleted: {
        groupsProvider.updateModel();
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

        property bool compl: false;

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

                for (let i = 0; i < groupsProvider.collectionModel.GetItemsCount(); i++){
                    let id = groupsProvider.collectionModel.GetData("Id", i);
                    if (id === documentId || childrenIds.includes(id)){
                        removedIndexes.push(i);
                    }
                }

                let removedCount = 0
                for (let i = 0; i < removedIndexes.length; i++){
                    groupsProvider.collectionModel.RemoveItem(removedIndexes[i] - removedCount);
                    removedCount++;
                }

                parentGroupsTable.elements = groupsProvider.collectionModel;

                compl = true;
            }
        }
    }

    function updateGui(){
        console.log("GroupEditor updateGui");

        if (userGroupEditorContainer.documentModel.ContainsKey("Name")){
            nameInput.text = userGroupEditorContainer.documentModel.GetData("Name");
        }
        else{
            nameInput.text = "";
        }

        if (userGroupEditorContainer.documentModel.ContainsKey("Description")){
            descriptionInput.text = userGroupEditorContainer.documentModel.GetData("Description");
        }
        else{
            descriptionInput.text = "";
        }

        let parentGroupIds = []
        if (userGroupEditorContainer.documentModel.ContainsKey("ParentGroups")){
            let parentGroups = userGroupEditorContainer.documentModel.GetData("ParentGroups");
            if (parentGroups !== ""){
                parentGroupIds = parentGroups.split(';')
            }
        }

        parentGroupsTable.uncheckAll();
        if (parentGroupsTable.elements){
            for (let i = 0; i < parentGroupsTable.elements.GetItemsCount(); i++){
                let id = parentGroupsTable.elements.GetData("Id", i);
                if (parentGroupIds.includes(id)){
                    parentGroupsTable.checkItem(i);
                }
            }
        }
    }

    function updateModel(){
        console.log("GroupEditor updateModel", userGroupEditorContainer.documentModel.toJSON());

        userGroupEditorContainer.documentModel.SetData("Description", descriptionInput.text);
        userGroupEditorContainer.documentModel.SetData("Name", nameInput.text);

        let selectedGroupIds = []
        let indexes = parentGroupsTable.getCheckedItems();
        for (let index of indexes){
            let id = parentGroupsTable.elements.GetData("Id", index);
            selectedGroupIds.push(id);
        }

        userGroupEditorContainer.documentModel.SetData("ParentGroups", selectedGroupIds.join(';'));
        console.log("GroupEditor end updateModel", userGroupEditorContainer.documentModel.toJSON());
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

            Item{
                width: parent.width;
                height: 1;
            }

            Text {
                id: titleName;

                anchors.left: parent.left;

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
                    if (userGroupEditorContainer.documentPtr){
                        userGroupEditorContainer.documentPtr.doUpdateModel();
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

                anchors.left: parent.left;

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
                    if (userGroupEditorContainer.documentPtr){
                        userGroupEditorContainer.documentPtr.doUpdateModel();
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

        radius:userGroupEditorContainer.radius;

        onCheckedItemsChanged: {
            if (userGroupEditorContainer.documentPtr){
                userGroupEditorContainer.documentPtr.doUpdateModel();
            }
        }
    }
}//Container
