import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0

Item {
    id: roleEditorContainer;

    property TreeItemModel documentModel: TreeItemModel {}
    property Item documentPtr: null;

    property int mainMargin: 0;
    property int panelWidth: 400;

    property int radius: 3;

    Rectangle{
        anchors.fill: parent;
        color: Style.backgroundColor;
    }

    property bool completed: rolesProvider.compl;

    Component.onCompleted: {
        rolesProvider.updateModel();
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

    function blockEditing(){
        roleIdInput.readOnly = true;
        roleNameInput.readOnly = true;
        descriptionInput.readOnly = true;
        parentRolesTable.readOnly = true;
    }

    CollectionDataProvider {
        id: rolesProvider;

        commandId: "Roles";
        fields: ["Id", "Name", "Description"];

        property bool compl: false;

        onModelUpdated: {
            if (rolesProvider.collectionModel != null){
                if (rolesProvider.collectionModel.ContainsKey("Roles")){
                    let documentId = roleEditorContainer.documentModel.GetData("Id");
                    let rolesModel = rolesProvider.collectionModel.GetData("Roles")

                    // Get all children ID-s
                    let childrenIds = []
                    roleEditorContainer.getAllChildrenRoleIds(documentId, rolesModel, childrenIds);

                    // Get all parent ID-s
                    let parentIds = []
                    if (roleEditorContainer.documentModel.ContainsKey("ParentRoles")){
                        let parentRoles = roleEditorContainer.documentModel.GetData("ParentRoles")
                        let parentRolesIds = parentRoles.split(';')
                        for (let j = 0; j < parentRolesIds.length; j++){
                            roleEditorContainer.getAllParentRoleIds(parentRolesIds[j], rolesModel, parentIds);
                        }
                    }

                    // Indexes for deleting
                    let removedIndexes = []
                    for (let i = 0; i < rolesModel.GetItemsCount(); i++){
                        let id = rolesModel.GetData("Id", i);
                        if (id === documentId || childrenIds.includes(id)){
                            removedIndexes.push(i);
                        }
                    }

                    let removedCount = 0
                    for (let i = 0; i < removedIndexes.length; i++){
                        rolesModel.RemoveItem(removedIndexes[i] - removedCount);
                        removedCount++;
                    }

                    parentRolesTable.elements = rolesModel;

                    rolesProvider.compl = true;
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
        console.log("RoleEditor start updateGui");

        if (roleEditorContainer.documentModel.ContainsKey("RoleId")){
            roleIdInput.text = documentModel.GetData("RoleId");
        }
        else{
            roleIdInput.text = "";
        }

        if (roleEditorContainer.documentModel.ContainsKey("Name")){
            roleNameInput.text = documentModel.GetData("Name");
        }
        else{
            roleNameInput.text = "";
        }

        if (roleEditorContainer.documentModel.ContainsKey("Description")){
            descriptionInput.text = documentModel.GetData("Description");
        }
        else{
            descriptionInput.text = "";
        }

        let parentRolesIds = []
        if (roleEditorContainer.documentModel.ContainsKey("ParentRoles")){
            let parentGroups = roleEditorContainer.documentModel.GetData("ParentRoles");
            if (parentGroups !== ""){
                parentRolesIds = parentGroups.split(';')
            }
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

        console.log("RoleEditor finish updateGui");
    }

    function updateModel(){
        roleEditorContainer.documentModel.SetData("RoleId", roleIdInput.text);
        roleEditorContainer.documentModel.SetData("Name", roleNameInput.text);
        roleEditorContainer.documentModel.SetData("Description", descriptionInput.text);

        let selectedRoleIds = []
        let indexes = parentRolesTable.getCheckedItems();
        for (let index of indexes){
            let id = parentRolesTable.elements.GetData("Id", index);
            selectedRoleIds.push(id);
        }

        roleEditorContainer.documentModel.SetData("ParentRoles", selectedRoleIds.join(';'));
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
                //                anchors.leftMargin: 5;

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
                    if (roleEditorContainer.documentPtr){
                        let oldText = roleEditorContainer.documentModel.GetData("Name");
                        if (oldText && oldText !== roleNameInput.text || !oldText && roleNameInput.text !== ""){
                            roleIdInput.text = roleNameInput.text.replace(/\s+/g, '');
                            roleEditorContainer.documentPtr.doUpdateModel();
                        }
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
                //                anchors.leftMargin: 5;

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
                    if (roleEditorContainer.documentPtr){
                        let oldText = roleEditorContainer.documentModel.GetData("Description");
                        if (oldText && oldText !== descriptionInput.text || !oldText && descriptionInput.text !== ""){
                            roleEditorContainer.documentPtr.doUpdateModel();
                        }
                    }
                }

                KeyNavigation.tab: roleNameInput;
            }
        }//Column bodyColumn
    }

    TreeItemModel {
        id: rolesHeadersModel;

        Component.onCompleted: {
            let index = rolesHeadersModel.InsertNewItem();

            rolesHeadersModel.SetData("Id", "Name");
            rolesHeadersModel.SetData("Name", "Role Name");

            parentRolesTable.headers = rolesHeadersModel;
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
            if (roleEditorContainer.documentPtr){
                roleEditorContainer.documentPtr.doUpdateModel();
            }
        }
    }
}//Container
