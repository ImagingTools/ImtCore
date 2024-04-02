import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0

ViewBase {
    id: roleEditorContainer;

    property int mainMargin: 0;
    property int panelWidth: 400;

    property int radius: 3;

    property TreeItemModel rolesModel: TreeItemModel {}
    property TreeItemModel copiedRolesModel: TreeItemModel {}

    Component.onCompleted: {
        Events.subscribeEvent("OnLocalizationChanged", roleEditorContainer.onLocalizationChanged);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("OnLocalizationChanged", roleEditorContainer.onLocalizationChanged);
    }

    function onLocalizationChanged(language){
        rolesHeadersModel.updateHeaders();
    }

    Rectangle{
        anchors.fill: parent;
        color: Style.backgroundColor;
    }

    function updateRolesModel(){
        copiedRolesModel.Copy(rolesModel);

        let documentId = roleEditorContainer.model.GetData("Id");

        // Get all children ID-s
        let childrenIds = []
        roleEditorContainer.getAllChildrenRoleIds(documentId, copiedRolesModel, childrenIds);

        // Get all parent ID-s
        let parentIds = []
        if (roleEditorContainer.model.ContainsKey("ParentRoles")){
            let parentRoles = roleEditorContainer.model.GetData("ParentRoles")
            let parentRolesIds = parentRoles.split(';')
            for (let j = 0; j < parentRolesIds.length; j++){
                roleEditorContainer.getAllParentRoleIds(parentRolesIds[j], copiedRolesModel, parentIds);
            }
        }

        // Indexes for deleting
        let removedIndexes = []
        for (let i = 0; i < copiedRolesModel.GetItemsCount(); i++){
            let id = copiedRolesModel.GetData("Id", i);
            console.log("id", id);

            if (id === documentId || childrenIds.includes(id)){
                removedIndexes.push(i);
            }
        }

        let removedCount = 0
        for (let i = 0; i < removedIndexes.length; i++){
            copiedRolesModel.RemoveItem(removedIndexes[i] - removedCount);
            removedCount++;
        }

        parentRolesTable.elements = copiedRolesModel;
    }

    function getAllParentRoleIds(roleId, rolesModel, retVal){
        for (let i = 0; i < rolesModel.GetItemsCount(); i++){
            let id = rolesModel.GetData("Id", i);
            if (id === roleId){
                let parentRoles = rolesModel.GetData("ParentRoles", i);
                let parentRolesIds = parentRoles.split(';');
                for (let j = 0; j < parentRolesIds.length; j++){
                    retVal.push(parentRolesIds[j])
                    roleEditorContainer.getAllParentRoleIds(parentRolesIds[j], rolesModel, retVal);
                }
            }
        }
    }

    function getAllChildrenRoleIds(roleId, rolesModel, retVal){
        for (let i = 0; i < rolesModel.GetItemsCount(); i++){
            let id = rolesModel.GetData("Id", i);

            let parentRoles = rolesModel.GetData("ParentRoles", i);
            if (parentRoles !== ""){
                let parentRolesIds = parentRoles.split(';');
                if (parentRolesIds.includes(roleId)){
                    retVal.push(id);

                    roleEditorContainer.getAllChildrenRoleIds(id, rolesModel, retVal);
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
    }

    function updateGui(){
        console.log("RoleEditor updateGui");

        if (roleEditorContainer.model.ContainsKey("RoleId")){
            roleIdInput.text = model.GetData("RoleId");
        }
        else{
            roleIdInput.text = "";
        }

        if (roleEditorContainer.model.ContainsKey("Name")){
            roleNameInput.text = model.GetData("Name");
        }
        else{
            roleNameInput.text = "";
        }

        if (roleEditorContainer.model.ContainsKey("Description")){
            descriptionInput.text = model.GetData("Description");
        }
        else{
            descriptionInput.text = "";
        }

        let parentRolesIds = []
        if (roleEditorContainer.model.ContainsKey("ParentRoles")){
            let parentGroups = roleEditorContainer.model.GetData("ParentRoles");
            if (parentGroups !== ""){
                parentRolesIds = parentGroups.split(';')
            }
        }

        parentRolesIds.sort();

        if (!parentRolesTable.elements){
            updateRolesModel();
        }

        parentRolesTable.uncheckAll();

        if (parentRolesTable.elements){
            for (let i = 0; i < parentRolesTable.elements.GetItemsCount(); i++){
                let id = parentRolesTable.elements.GetData("Id", i);
                if (parentRolesIds.includes(id)){
                    parentRolesTable.checkItem(i);
                }
            }
        }
    }

    function updateModel(){
        roleEditorContainer.model.SetData("RoleId", roleIdInput.text);
        roleEditorContainer.model.SetData("Name", roleNameInput.text);
        roleEditorContainer.model.SetData("Description", descriptionInput.text);

        let selectedRoleIds = []
        let indexes = parentRolesTable.getCheckedItems();
        for (let index of indexes){
            let id = parentRolesTable.elements.GetData("Id", index);
            selectedRoleIds.push(id);
        }

        selectedRoleIds.sort();

        roleEditorContainer.model.SetData("ParentRoles", selectedRoleIds.join(';'));

        if (!parentRolesTable.elements){
            updateRolesModel();
        }
    }

    Item{
        id: columnContainer;

        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.leftMargin: 10;

        width: roleEditorContainer.panelWidth;
        height: bodyColumn.height + 2*bodyColumn.anchors.topMargin;

        Column {
            id: bodyColumn;

            anchors.top: parent.top;
            anchors.left: parent.left;
            anchors.topMargin: roleEditorContainer.mainMargin;
            anchors.leftMargin: roleEditorContainer.mainMargin;

            width: roleEditorContainer.panelWidth - 2*anchors.leftMargin;

            spacing: 10;

            Item{
                width: parent.width;
                height: 1;
            }

            Text {
                id: titleRoleName;

                anchors.left: parent.left;

                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;

                text: qsTr("Role Name");
            }

            CustomTextField {
                id: roleNameInput;

                width: parent.width;
                height: 30;

                placeHolderText: qsTr("Enter the role name");

                onEditingFinished: {
                    let oldText = roleEditorContainer.model.GetData("Name");
                    if (oldText && oldText !== roleNameInput.text || !oldText && roleNameInput.text !== ""){
                        roleIdInput.text = roleNameInput.text.replace(/\s+/g, '');
                        roleEditorContainer.doUpdateModel();
                    }
                }

                KeyNavigation.tab: roleIdInput;
            }

            Text {
                id: titleRoleId;

                anchors.left: parent.left;
                //                anchors.leftMargin: 5;

                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;

                text: qsTr("Role-ID");
            }

            CustomTextField {
                id: roleIdInput;

                width: parent.width;
                height: 30;
                readOnly: true;

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
                    let oldText = roleEditorContainer.model.GetData("Description");
                    if (oldText && oldText !== descriptionInput.text || !oldText && descriptionInput.text !== ""){
                        roleEditorContainer.doUpdateModel();
                    }
                }

                KeyNavigation.tab: roleNameInput;
            }
        }//Column bodyColumn
    }

    TreeItemModel {
        id: rolesHeadersModel;

        function updateHeaders(){
            rolesHeadersModel.Clear();

            let index = rolesHeadersModel.InsertNewItem();

            rolesHeadersModel.SetData("Id", "Name");
            rolesHeadersModel.SetData("Name", qsTr("Role Name"));

            rolesHeadersModel.Refresh();

            parentRolesTable.headers = rolesHeadersModel;
        }

        Component.onCompleted: {
            updateHeaders();
        }
    }

    Text {
        id: titleIncludes;

        anchors.top: columnContainer.bottom;
        anchors.topMargin: 10;
        anchors.left: parent.left;
        anchors.leftMargin: 10;

        color: Style.textColor;
        font.family: Style.fontFamily;
        font.pixelSize: Style.fontSize_common;

        text: qsTr("Parent Roles");
    }

    AuxTable {
        id: parentRolesTable;

        anchors.top: titleIncludes.bottom;
        anchors.topMargin: 10;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 10;
        anchors.left: parent.left;
        anchors.leftMargin: 10;

        width: 400;
        checkable: true;
        radius: roleEditorContainer.radius;

        onCheckedItemsChanged: {
            roleEditorContainer.doUpdateModel();
        }
    }
}//Container
