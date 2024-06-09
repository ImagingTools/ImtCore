import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

ViewBase {
    id: container;

    anchors.fill: parent;

    property TreeItemModel permissionsModel: TreeItemModel {};
    property TreeItemModel rolesModel: TreeItemModel {};

    property string productId: "";

    function updateGui(){
        generalGroup.updateGui();
        permissionsGroup.updateGui()
    }

    function updateModel(){
        generalGroup.updateModel();
        permissionsGroup.updateModel()
    }

    property TreeItemModel copiedRolesModel: TreeItemModel {}

    CustomScrollbar {
        id: scrollbar;

        z: parent.z + 1;

        anchors.right: parent.right;
        anchors.top: flickable.top;
        anchors.bottom: flickable.bottom;

        secondSize: 10;
        targetItem: flickable;
    }

    CustomScrollbar{
        id: scrollHoriz;

        z: parent.z + 1;

        anchors.left: flickable.left;
        anchors.right: flickable.right;
        anchors.bottom: flickable.bottom;

        secondSize: 10;

        vertical: false;
        targetItem: flickable;
    }

    Flickable {
        id: flickable;

        anchors.top: parent.top;
        anchors.topMargin: Style.size_largeMargin;

        anchors.bottom: parent.bottom;
        anchors.bottomMargin: Style.size_largeMargin;

        anchors.left: parent.left;
        anchors.leftMargin: Style.size_largeMargin;

        anchors.right: scrollbar.left;
        anchors.rightMargin: Style.size_largeMargin;

        contentWidth: bodyColumn.width;
        contentHeight: bodyColumn.height + 2 * Style.size_largeMargin;

        boundsBehavior: Flickable.StopAtBounds;

        clip: true;

        Column {
            id: bodyColumn;

            width: 700;

            spacing: Style.size_largeMargin;

            GroupHeaderView {
                width: parent.width;

                title: qsTr("General");
                groupView: generalGroup;
            }

            GroupElementView {
                id: generalGroup;

                width: parent.width;

                TextInputElementView {
                    id: roleNameInput;

                    name: qsTr("Role Name");
                    placeHolderText: qsTr("Enter the role name");

                    onEditingFinished: {
                        let oldText = container.model.GetData("Name");
                        if (oldText && oldText !== roleNameInput.text || !oldText && roleNameInput.text !== ""){
                            roleIdInput.text = roleNameInput.text.replace(/\s+/g, '');
                            container.doUpdateModel();
                        }
                    }

                    KeyNavigation.tab: roleIdInput;
                    KeyNavigation.backtab: permissionsGroup;
                }

                TextInputElementView {
                    id: roleIdInput;

                    readOnly: true;

                    name: qsTr("Role-ID");

                    KeyNavigation.tab: descriptionInput;
                    KeyNavigation.backtab: roleNameInput;
                }

                TextInputElementView {
                    id: descriptionInput;

                    name: qsTr("Description");
                    placeHolderText: qsTr("Enter the description");

                    onEditingFinished: {
                        let oldText = container.model.GetData("Description");
                        if (oldText && oldText !== descriptionInput.text || !oldText && descriptionInput.text !== ""){
                            container.doUpdateModel();
                        }
                    }

                    KeyNavigation.tab: parentRolesTable;
                    KeyNavigation.backtab: roleIdInput;
                }

                TableElementView {
                    id: parentRolesTable;

                    name: qsTr("Parent Roles");

                    KeyNavigation.tab: permissionsGroup;
                    KeyNavigation.backtab: descriptionInput;

                    Component.onCompleted: {
                        parentRolesTable.table.checkable = true;
                    }

                    Connections {
                        target: parentRolesTable.table;

                        function onCheckedItemsChanged(){
                            container.doUpdateModel();
                        }
                    }
                }

                TreeItemModel {
                    id: rolesHeadersModel;

                    function updateHeaders(){
                        rolesHeadersModel.Clear();

                        let index = rolesHeadersModel.InsertNewItem();

                        rolesHeadersModel.SetData("Id", "Name");
                        rolesHeadersModel.SetData("Name", qsTr("Role Name"));

                        rolesHeadersModel.Refresh();

                        parentRolesTable.table.headers = rolesHeadersModel;
                    }

                    Component.onCompleted: {
                        updateHeaders();
                    }
                }

                function updateGui(){
                    if (container.model.ContainsKey("RoleId")){
                        roleIdInput.text = container.model.GetData("RoleId");
                    }
                    else{
                        roleIdInput.text = "";
                    }

                    if (container.model.ContainsKey("Name")){
                        roleNameInput.text = container.model.GetData("Name");
                    }
                    else{
                        roleNameInput.text = "";
                    }

                    if (container.model.ContainsKey("Description")){
                        descriptionInput.text = container.model.GetData("Description");
                    }
                    else{
                        descriptionInput.text = "";
                    }

                    let parentRolesIds = []
                    if (container.model.ContainsKey("ParentRoles")){
                        let parentGroups = container.model.GetData("ParentRoles");
                        if (parentGroups !== ""){
                            parentRolesIds = parentGroups.split(';')
                        }
                    }

                    parentRolesIds.sort();

                    if (!parentRolesTable.table.elements){
                        updateRolesModel();
                    }

                    parentRolesTable.table.uncheckAll();

                    if (parentRolesTable.table.elements){
                        for (let i = 0; i < parentRolesTable.table.elements.GetItemsCount(); i++){
                            let id = parentRolesTable.table.elements.GetData("Id", i);
                            if (parentRolesIds.includes(id)){
                                parentRolesTable.table.checkItem(i);
                            }
                        }
                    }
                }

                function updateModel(){
                    container.model.SetData("RoleId", roleIdInput.text);
                    container.model.SetData("Name", roleNameInput.text);
                    container.model.SetData("Description", descriptionInput.text);

                    let selectedRoleIds = []
                    let indexes = parentRolesTable.table.getCheckedItems();
                    for (let index of indexes){
                        let id = parentRolesTable.table.elements.GetData("Id", index);
                        selectedRoleIds.push(id);
                    }

                    selectedRoleIds.sort();

                    container.model.SetData("ParentRoles", selectedRoleIds.join(';'));

                    if (!parentRolesTable.table.elements){
                        updateRolesModel();
                    }
                }

                function updateRolesModel(){
                    container.copiedRolesModel.Copy(container.rolesModel);

                    let documentId = container.model.GetData("Id");

                    // Get all children ID-s
                    let childrenIds = []
                    getAllChildrenRoleIds(documentId, container.copiedRolesModel, childrenIds);

                    // Get all parent ID-s
                    let parentIds = []
                    if (container.model.ContainsKey("ParentRoles")){
                        let parentRoles = container.model.GetData("ParentRoles")
                        let parentRolesIds = parentRoles.split(';')
                        for (let j = 0; j < parentRolesIds.length; j++){
                            getAllParentRoleIds(parentRolesIds[j], container.copiedRolesModel, parentIds);
                        }
                    }

                    // Indexes for deleting
                    let removedIndexes = []
                    for (let i = 0; i < container.copiedRolesModel.GetItemsCount(); i++){
                        let id = container.copiedRolesModel.GetData("Id", i);
                        console.log("id", id);

                        if (id === documentId || childrenIds.includes(id)){
                            removedIndexes.push(i);
                        }
                    }

                    let removedCount = 0
                    for (let i = 0; i < removedIndexes.length; i++){
                        container.copiedRolesModel.RemoveItem(removedIndexes[i] - removedCount);
                        removedCount++;
                    }

                    parentRolesTable.table.elements = container.copiedRolesModel;
                }

                function getAllParentRoleIds(roleId, rolesModel, retVal){
                    for (let i = 0; i < rolesModel.GetItemsCount(); i++){
                        let id = rolesModel.GetData("Id", i);
                        if (id === roleId){
                            let parentRoles = rolesModel.GetData("ParentRoles", i);
                            let parentRolesIds = parentRoles.split(';');
                            for (let j = 0; j < parentRolesIds.length; j++){
                                retVal.push(parentRolesIds[j])
                                getAllParentRoleIds(parentRolesIds[j], rolesModel, retVal);
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

                                getAllChildrenRoleIds(id, rolesModel, retVal);
                            }
                        }
                    }
                }
            }

            GroupHeaderView {
                width: parent.width;

                title: qsTr("Permissions");
                groupView: group;
            }

            GroupElementView {
                id: group;

                width: parent.width;

                TreeViewElementView {
                    id: permissionsGroup;

                    KeyNavigation.tab: roleNameInput;
                    KeyNavigation.backtab: parentRolesTable;

                    Component.onCompleted: {
                        permissionsGroup.treeView.tristate = true;
                    }

                    function updateGui(){
                        let selectedPermissionsIds = [];
                        if (container.model.ContainsKey("Permissions")){
                            let selectedPermissions = container.model.GetData("Permissions");
                            if (selectedPermissions !== ""){
                                selectedPermissionsIds = selectedPermissions.split(';');
                            }
                        }

                        selectedPermissionsIds.sort();

                        permissionsGroup.treeView.uncheckAll();

                        let itemsList = permissionsGroup.treeView.getItemsDataAsList();
                        for (let i = 0; i < itemsList.length; i++){
                            let delegateItem = itemsList[i];
                            if (!delegateItem.hasChild){
                                let itemData = delegateItem.getItemData();
                                let id = itemData.FeatureId;

                                if (selectedPermissionsIds.includes(id)){
                                    delegateItem.isOpened = true;
                                    permissionsGroup.treeView.checkItem(delegateItem);
                                }
                                else{
                                    delegateItem.isOpened = false;
                                }
                            }
                        }
                    }

                    function updateModel(){
                        let selectedPermissionIds = []
                        let itemsList = permissionsGroup.treeView.getCheckedItems();
                        for (let delegate of itemsList){
                            if (!delegate.hasChild){
                                let itemData = delegate.getItemData();
                                let id = itemData.FeatureId;
                                selectedPermissionIds.push(id)
                            }
                        }

                        selectedPermissionIds.sort();

                        let permissions = container.model.GetData("Permissions");
                        container.model.SetData("Permissions", selectedPermissionIds.join(';'));
                    }

                    Connections {
                        target: permissionsGroup.treeView;

                        function onCheckedItemsChanged(){
                            container.doUpdateModel();
                        }
                    }

                    TreeItemModel {
                        id: permissionHeaders;

                        function updateHeaders(){
                            permissionHeaders.Clear();

                            let index = permissionHeaders.InsertNewItem();
                            permissionHeaders.SetData("Id", "FeatureName", index)
                            permissionHeaders.SetData("Name", qsTr("Permission"), index)

                            permissionHeaders.Refresh();

                            permissionsGroup.treeView.columnModel = permissionHeaders;
                            permissionsGroup.treeView.rowModel = container.permissionsModel;
                        }

                        Component.onCompleted: {
                            updateHeaders();
                        }
                    }
                }
            }
        }
    }
}
