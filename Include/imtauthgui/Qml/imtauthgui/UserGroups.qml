import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import Acf 1.0

Item {
    id: userGroupsContainer;

    property TreeItemModel documentModel: TreeItemModel {}
    property UndoRedoManager undoRedoManager: null;

    property bool blockUpdatingModel: false;

    onDocumentModelChanged: {
        console.log("UserRoles onDocumentModelChanged", documentModel);

        groupsProvider.updateModel();
    }

    CollectionDataProvider {
        id: groupsProvider;

        commandId: "Groups";

        fields: ["Id", "Name", "Description", "Roles"];

        onModelUpdated: {
            if (groupsProvider.collectionModel != null){
                for (let i = 0; i < groupsProvider.collectionModel.GetItemsCount(); i++){
                    groupsProvider.collectionModel.SetData("CheckedState", Qt.Unchecked, i);
                }

                groupsTable.elements = groupsProvider.collectionModel;

                userGroupsContainer.updateGui();
                groupsProvider.collectionModel.dataChanged.connect(userGroupsContainer.updateModel);
            }
        }
    }

    function updateGui(){
        console.log("UserGroups updateGui");
        userGroupsContainer.blockUpdatingModel = true;

        let groupIds = []
        if (userGroupsContainer.documentModel.ContainsKey("Groups")){
            let parentGroups = userGroupsContainer.documentModel.GetData("Groups");
            if (parentGroups !== ""){
                groupIds = parentGroups.split(';')
            }
        }

        for (let i = 0; i < groupsTable.elements.GetItemsCount(); i++){
            let id = groupsTable.elements.GetData("Id", i);
            if (groupIds.includes(id)){
                groupsTable.elements.SetData("CheckedState", Qt.Checked, i);
            }
            else{
                groupsTable.elements.SetData("CheckedState", Qt.Unchecked, i);
            }
        }

        userGroupsContainer.blockUpdatingModel = false;
    }

    function updateModel(){
        console.log("UserRoles updateModel");
        if (userGroupsContainer.blockUpdatingModel){
            return;
        }

        if (userGroupsContainer.undoRedoManager){
            userGroupsContainer.undoRedoManager.beginChanges();
        }

        let selectedRoleIds = []
        let selectedGroupIds = []
        if  (groupsTable.elements){
            for (let i = 0; i < groupsTable.elements.GetItemsCount(); i++){
                let id = groupsTable.elements.GetData("Id", i);
                let state = groupsTable.elements.GetData("CheckedState", i);
                if (state === Qt.Checked){
                    if (groupsTable.elements.ContainsKey("Roles", i)){
                        let roles = groupsTable.elements.GetData("Roles", i);
                        let roleIds = roles.split(';')
                        for (let j = 0; j < roleIds.length; j++){
                            if (!selectedRoleIds.includes(roleIds[j])){
                                selectedRoleIds.push(roleIds[j])
                            }
                        }
                    }

                    selectedGroupIds.push(id)
                }
            }
        }

        userGroupsContainer.documentModel.SetData("Groups", selectedGroupIds.join(';'));

        let ok = true;
        if (userGroupsContainer.documentModel.ContainsKey("Roles")){
            let userRoles = userGroupsContainer.documentModel.GetData("Roles");
            if (userRoles !== ""){
                ok = false;

                let userRoleIds = userRoles.split(';');
                for (let j = 0; j < selectedRoleIds.length; j++){
                    if (!userRoleIds.includes(selectedRoleIds[j])){
                        userRoleIds.push(selectedRoleIds[j])
                    }
                }
               // userGroupsContainer.documentModel.SetData("Roles", userRoleIds.join(';'));
            }
        }

        if (ok){
           // userGroupsContainer.documentModel.SetData("Roles", selectedRoleIds.join(';'));
        }

        if (userGroupsContainer.undoRedoManager){
            userGroupsContainer.undoRedoManager.endChanges();
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

    Text {
        id: titleRoles;

        anchors.top: parent.top;
        anchors.left: parent.left;
        //        anchors.leftMargin: 10;

        text: qsTr("Groups");
        color: Style.textColor;
        font.family: Style.fontFamily;
        font.pixelSize: Style.fontSize_common;
    }

    TreeItemModel {
        id: groupsHeadersModel;

        Component.onCompleted: {
            let index = groupsHeadersModel.InsertNewItem();
            groupsHeadersModel.SetData("Id", "Name", index);
            groupsHeadersModel.SetData("Name", "Group Name", index);

            index = groupsHeadersModel.InsertNewItem();

            groupsHeadersModel.SetData("Id", "Description", index);
            groupsHeadersModel.SetData("Name", "Description", index);

            groupsTable.headers = groupsHeadersModel;
        }
    }

    AuxTable {
        id: groupsTable;

        anchors.top: titleRoles.bottom;
        anchors.topMargin: 7;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 10;
        anchors.left: parent.left;
        //        anchors.leftMargin: 10;
        width: 400;

        checkable: true;

        radius: 0;
    }

}//Container
