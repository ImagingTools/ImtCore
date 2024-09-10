import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthRolesSdl 1.0

ViewBase {
    id: container;

    anchors.fill: parent;

    property TreeItemModel permissionsModel: TreeItemModel {};
    property TreeItemModel rolesModel: TreeItemModel {};

    property string productId: "";

    property RoleData roleData: model ? model : null;

    function updateGui(){
        generalGroup.updateGui();
        permissionsGroup.updateGui()
    }

    function updateModel(){
        generalGroup.updateModel();
        permissionsGroup.updateModel()

        roleData.m_productId = container.productId;
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
                        let oldText = container.roleData.m_name;
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
                        let oldText = container.roleData.m_description;
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
                    visible: parentRolesTable.table ? parentRolesTable.table.elementsCount > 0 : false;

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
                        rolesHeadersModel.clear();

                        let index = rolesHeadersModel.insertNewItem();

                        rolesHeadersModel.setData("Id", "RoleName");
                        rolesHeadersModel.setData("Name", qsTr("Role Name"));

                        rolesHeadersModel.refresh();

                        parentRolesTable.table.headers = rolesHeadersModel;
                    }

                    Component.onCompleted: {
                        updateHeaders();
                    }
                }

                function updateGui(){
                    roleIdInput.text = container.roleData.m_roleId;
                    roleNameInput.text = container.roleData.m_name;
                    descriptionInput.text = container.roleData.m_description;

                    let parentRolesIds = [];
                    let parentGroups = container.roleData.m_parentRoles;
                    if (parentGroups !== ""){
                        parentRolesIds = parentGroups.split(';')
                    }

                    parentRolesIds.sort();

                    if (!parentRolesTable.table.elements){
                        updateRolesModel();
                    }

                    parentRolesTable.table.uncheckAll();

                    if (parentRolesTable.table.elements){
                        for (let i = 0; i < parentRolesTable.table.elements.getItemsCount(); i++){
                            let id = parentRolesTable.table.elements.getData("Id", i);
                            if (parentRolesIds.includes(id)){
                                parentRolesTable.table.checkItem(i);
                            }
                        }
                    }
                }

                function updateModel(){
                    container.roleData.m_roleId = roleIdInput.text;
                    container.roleData.m_name = roleNameInput.text;
                    container.roleData.m_description = descriptionInput.text;

                    let selectedRoleIds = []
                    let indexes = parentRolesTable.table.getCheckedItems();
                    for (let index of indexes){
                        let id = parentRolesTable.table.elements.getData("Id", index);
                        selectedRoleIds.push(id);
                    }

                    selectedRoleIds.sort();
                    container.roleData.m_parentRoles = selectedRoleIds.join(';')

                    if (!parentRolesTable.table.elements){
                        updateRolesModel();
                    }
                }

                function updateRolesModel(){
                    container.copiedRolesModel.copy(container.rolesModel);

                    let documentId = container.roleData.m_id;

                    // Get all children ID-s
                    let childrenIds = []
                    getAllChildrenRoleIds(documentId, container.copiedRolesModel, childrenIds);

                    // Get all parent ID-s
                    let parentIds = []
                    let parentRoles = container.roleData.m_parentRoles;
                    let parentRolesIds = parentRoles.split(';')
                    for (let j = 0; j < parentRolesIds.length; j++){
                        getAllParentRoleIds(parentRolesIds[j], container.copiedRolesModel, parentIds);
                    }

                    // Indexes for deleting
                    let removedIndexes = []
                    for (let i = 0; i < container.copiedRolesModel.getItemsCount(); i++){
                        let id = container.copiedRolesModel.getData("Id", i);
                        if (id === documentId || childrenIds.includes(id)){
                            removedIndexes.push(i);
                        }
                    }

                    let removedCount = 0
                    for (let i = 0; i < removedIndexes.length; i++){
                        container.copiedRolesModel.removeItem(removedIndexes[i] - removedCount);
                        removedCount++;
                    }

                    parentRolesTable.table.elements = container.copiedRolesModel;
                }

                function getAllParentRoleIds(roleId, rolesModel, retVal){
                    for (let i = 0; i < rolesModel.getItemsCount(); i++){
                        let id = rolesModel.getData("Id", i);
                        if (id === roleId){
                            let parentRoles = rolesModel.getData("ParentRoles", i);
                            let parentRolesIds = parentRoles.split(';');
                            for (let j = 0; j < parentRolesIds.length; j++){
                                retVal.push(parentRolesIds[j])
                                getAllParentRoleIds(parentRolesIds[j], rolesModel, retVal);
                            }
                        }
                    }
                }

                function getAllChildrenRoleIds(roleId, rolesModel, retVal){
                    for (let i = 0; i < rolesModel.getItemsCount(); i++){
                        let id = rolesModel.getData("Id", i);

                        let parentRoles = rolesModel.getData("ParentRoles", i);
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
                        let selectedPermissions = container.roleData.m_permissions;
                        if (selectedPermissions !== ""){
                            selectedPermissionsIds = selectedPermissions.split(';');
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

                        container.roleData.m_permissions = selectedPermissionIds.join(';')
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
                            permissionHeaders.clear();

                            let index = permissionHeaders.insertNewItem();
                            permissionHeaders.setData("Id", "FeatureName", index)
                            permissionHeaders.setData("Name", qsTr("Permission"), index)

                            permissionHeaders.refresh();

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
