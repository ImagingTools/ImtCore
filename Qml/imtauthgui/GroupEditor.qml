import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtcolgui 1.0
import imtlicgui 1.0
import imtcontrols 1.0

ViewBase {
    id: userGroupEditorContainer;

    property int mainMargin: 0;
    property int panelWidth: 400;

    property int radius: 3;

    property TreeItemModel groupsModel: TreeItemModel {}
    property TreeItemModel copiedGroupsModel: TreeItemModel {}

    Component.onCompleted: {
        Events.subscribeEvent("OnLocalizationChanged", userGroupEditorContainer.onLocalizationChanged);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("OnLocalizationChanged", userGroupEditorContainer.onLocalizationChanged);
    }

    function onLocalizationChanged(language){
        groupsHeadersModel.updateHeaders();
    }

    function updateGroupsModel(){
        copiedGroupsModel.copy(groupsModel);

        let objectId = userGroupEditorContainer.model.getData("Id");
        let removedIndexes = []

        let childrenIds = []
        userGroupEditorContainer.getAllChildrenGroups(objectId, childrenIds);

        // Get all parent ID-s
        let parentIds = []
        if (userGroupEditorContainer.model.containsKey("ParentGroups")){
            let parentGroups = userGroupEditorContainer.model.getData("ParentGroups")
            let parentGroupIds = parentGroups.split(';')
            for (let j = 0; j < parentGroupIds.length; j++){
                userGroupEditorContainer.getAllParentGroupIds(parentGroupIds[j], parentIds);
            }
        }

        for (let i = 0; i < userGroupEditorContainer.copiedGroupsModel.getItemsCount(); i++){
            let id = userGroupEditorContainer.copiedGroupsModel.getData("Id", i);
            if (id === objectId || childrenIds.includes(id)){
                removedIndexes.push(i);
            }
        }

        let removedCount = 0
        for (let i = 0; i < removedIndexes.length; i++){
            userGroupEditorContainer.copiedGroupsModel.removeItem(removedIndexes[i] - removedCount);
            removedCount++;
        }


        parentGroupsTable.elements = userGroupEditorContainer.copiedGroupsModel;
    }

    function getAllChildrenGroups(groupId, retVal){
        for (let i = 0; i < userGroupEditorContainer.copiedGroupsModel.getItemsCount(); i++){
            let id = userGroupEditorContainer.copiedGroupsModel.getData("Id", i);
            let parentGroups = userGroupEditorContainer.copiedGroupsModel.getData("ParentGroups", i);
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
        for (let i = 0; i < userGroupEditorContainer.copiedGroupsModel.getItemsCount(); i++){
            let id = userGroupEditorContainer.copiedGroupsModel.getData("Id", i);
            if (id === groupId){
                let parentGroups = userGroupEditorContainer.copiedGroupsModel.getData("ParentGroups", i);
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

    function updateGui(){
        console.log("GroupEditor updateGui");

        if (userGroupEditorContainer.model.containsKey("Name")){
            nameInput.text = userGroupEditorContainer.model.getData("Name");
        }
        else{
            nameInput.text = "";
        }

        if (userGroupEditorContainer.model.containsKey("Description")){
            descriptionInput.text = userGroupEditorContainer.model.getData("Description");
        }
        else{
            descriptionInput.text = "";
        }

        let parentGroupIds = []
        if (userGroupEditorContainer.model.containsKey("ParentGroups")){
            let parentGroups = userGroupEditorContainer.model.getData("ParentGroups");
            if (parentGroups !== ""){
                parentGroupIds = parentGroups.split(';')
            }
        }

        if (!parentGroupsTable.elements){
            updateGroupsModel();
        }

        parentGroupsTable.uncheckAll();
        if (parentGroupsTable.elements){
            for (let i = 0; i < parentGroupsTable.elements.getItemsCount(); i++){
                let id = parentGroupsTable.elements.getData("Id", i);
                if (parentGroupIds.includes(id)){
                    parentGroupsTable.checkItem(i);
                }
            }
        }
    }

    function updateModel(){
        userGroupEditorContainer.model.setData("Description", descriptionInput.text);
        userGroupEditorContainer.model.setData("Name", nameInput.text);

        let selectedGroupIds = []
        let indexes = parentGroupsTable.getCheckedItems();
        for (let index of indexes){
            let id = parentGroupsTable.elements.getData("Id", index);
            selectedGroupIds.push(id);
        }

        selectedGroupIds.sort();

        userGroupEditorContainer.model.setData("ParentGroups", selectedGroupIds.join(';'));

        if (!parentGroupsTable.elements){
            updateGroupsModel();
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
                    userGroupEditorContainer.doUpdateModel();
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
                    userGroupEditorContainer.doUpdateModel();
                }

                KeyNavigation.tab: nameInput;
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

        function updateHeaders(){
            groupsHeadersModel.clear();

            let index = groupsHeadersModel.insertNewItem();

            groupsHeadersModel.setData("Id", "Name");
            groupsHeadersModel.setData("Name", qsTr("Group Name"));

            groupsHeadersModel.refresh();

            parentGroupsTable.headers = groupsHeadersModel;
        }

        Component.onCompleted: {
            updateHeaders();
        }
    }

    Table {
        id: parentGroupsTable;

        anchors.top: title.bottom;
        anchors.topMargin: 10;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 10;

        width: 400;

        checkable: true;

        radius: userGroupEditorContainer.radius;

        onCheckedItemsChanged: {
            userGroupEditorContainer.doUpdateModel();
        }
    }
}//Container
