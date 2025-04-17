import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthGroupsSdl 1.0
import imtdocgui 1.0

ViewBase {
    id: container;

    property TreeItemModel groupsModel: TreeItemModel {}
    property TreeItemModel rolesModel: TreeItemModel {}
    property TreeItemModel usersModel: TreeItemModel {}

    property TreeItemModel copiedGroupsModel: TreeItemModel {}

    property GroupData groupData: model;

    property string productId;

    function updateGui(){
        generalGroup.updateGui();
        usersGroup.updateGui();
        rolesGroup.updateGui();
    }

    function updateModel(){
        generalGroup.updateModel();
        usersGroup.updateModel();
        rolesGroup.updateModel();

        groupData.m_productId = productId;
    }

	DocumentHistoryPanel {
		id: historyPanel;
		documentId: container.groupData ? container.groupData.m_id : "";
		collectionId: "Groups";
		editorFlickable: flickable;
	}

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
        anchors.topMargin: Style.sizeLargeMargin;

        anchors.bottom: parent.bottom;
        anchors.bottomMargin: Style.sizeLargeMargin;

        anchors.left: parent.left;
        anchors.leftMargin: Style.sizeLargeMargin;

        anchors.right: scrollbar.left;
        anchors.rightMargin: Style.sizeLargeMargin;

        contentWidth: bodyColumn.width;
		contentHeight: Math.max(bodyColumn.height + 2 * Style.sizeLargeMargin, historyPanel.contentHeight + 2 * Style.sizeLargeMargin);

        boundsBehavior: Flickable.StopAtBounds;

        clip: true;

        Column {
            id: bodyColumn;

            width: 700;

            spacing: Style.sizeLargeMargin;

            GroupHeaderView {
                width: parent.width;

                title: qsTr("General");
                groupView: generalGroup;
            }

            GroupElementView {
                id: generalGroup;

                width: parent.width;

                TextInputElementView {
                    id: nameInput;

                    name: qsTr("Group Name");
                    placeHolderText: qsTr("Enter the name");

                    onEditingFinished: {
                        container.doUpdateModel();
                    }

                    KeyNavigation.tab: descriptionInput;
                    KeyNavigation.backtab: rolesTable;
                }

                TextInputElementView {
                    id: descriptionInput;

                    name: qsTr("Description");
                    placeHolderText: qsTr("Enter the description");

                    onEditingFinished: {
                        container.doUpdateModel();
                    }

                    KeyNavigation.tab: parentGroupsTable;
                    KeyNavigation.backtab: nameInput;
                }

                TableElementView {
                    id: parentGroupsTable;

                    name: qsTr("Parent Groups");

                    KeyNavigation.tab: usersTable;
                    KeyNavigation.backtab: descriptionInput;

                    Connections {
                        target: parentGroupsTable.table;

                        function onCheckedItemsChanged(){
                            container.doUpdateModel();
                        }
                    }

                    TreeItemModel {
                        id: groupsHeadersModel;

                        function updateHeaders(){
                            groupsHeadersModel.clear();

                            let index = groupsHeadersModel.insertNewItem();

                            groupsHeadersModel.setData("id", "name");
                            groupsHeadersModel.setData("name", qsTr("Group Name"));

                            groupsHeadersModel.refresh();

                            parentGroupsTable.table.checkable = true;
                            parentGroupsTable.table.headers = groupsHeadersModel;
                        }

                        Component.onCompleted: {
                            updateHeaders();
                        }
                    }
                }

                function updateGroupsModel(){
                    container.copiedGroupsModel.copy(container.groupsModel);

                    let objectId = container.groupData.m_id;
                    let removedIndexes = []

                    let childrenIds = []
                    getAllChildrenGroups(objectId, childrenIds);

                    // Get all parent ID-s
                    let parentIds = []
                    let parentGroups = container.groupData.m_parentGroups;
                    let parentGroupIds = parentGroups.split(';')
                    for (let j = 0; j < parentGroupIds.length; j++){
                        getAllParentGroupIds(parentGroupIds[j], parentIds);
                    }

                    for (let i = 0; i < container.copiedGroupsModel.getItemsCount(); i++){
                        let id = container.copiedGroupsModel.getData("id", i);
                        if (id === objectId || childrenIds.includes(id)){
                            removedIndexes.push(i);
                        }
                    }

                    let removedCount = 0
                    for (let i = 0; i < removedIndexes.length; i++){
                        container.copiedGroupsModel.removeItem(removedIndexes[i] - removedCount);
                        removedCount++;
                    }

                    parentGroupsTable.table.checkable = true;
                    parentGroupsTable.table.elements = container.copiedGroupsModel;
                }

                function getAllChildrenGroups(groupId, retVal){
                    for (let i = 0; i < container.copiedGroupsModel.getItemsCount(); i++){
                        let id = container.copiedGroupsModel.getData("id", i);
                        let parentGroups = container.copiedGroupsModel.getData("ParentGroups", i);
                        if (parentGroups !== ""){
                            let parentGroupIds = parentGroups.split(';')
                            if (parentGroupIds.includes(groupId)){
                                retVal.push(id);

                                getAllChildrenGroups(id, retVal);
                            }
                        }
                    }
                }

                function getAllParentGroupIds(groupId, retVal){
                    for (let i = 0; i < container.copiedGroupsModel.getItemsCount(); i++){
                        let id = container.copiedGroupsModel.getData("id", i);
                        if (id === groupId){
                            let parentGroups = container.copiedGroupsModel.getData("ParentGroups", i);
                            if (parentGroups !== ""){
                                let parentGroupsIds = parentGroups.split(';');
                                for (let j = 0; j < parentGroupsIds.length; j++){
                                    retVal.push(parentGroupsIds[j])
                                    getAllParentGroupIds(parentGroupsIds[j], retVal);
                                }
                            }
                        }
                    }
                }

                function updateGui(){
                    nameInput.text = container.groupData.m_name;
                    descriptionInput.text = container.groupData.m_description;

                    let parentGroupIds = []
                    let parentGroups = container.groupData.m_parentGroups;
                    if (parentGroups !== ""){
                        parentGroupIds = parentGroups.split(';')
                    }

                    if (!parentGroupsTable.table.elements){
                        updateGroupsModel();
                    }

                    parentGroupsTable.table.uncheckAll();
                    if (parentGroupsTable.table.elements){
                        for (let i = 0; i < parentGroupsTable.table.elements.getItemsCount(); i++){
                            let id = parentGroupsTable.table.elements.getData("id", i);
                            if (parentGroupIds.includes(id)){
                                parentGroupsTable.table.checkItem(i);
                            }
                        }
                    }
                }

                function updateModel(){
                    container.groupData.m_description = descriptionInput.text;
                    container.groupData.m_name = nameInput.text;

                    let selectedGroupIds = []
                    let indexes = parentGroupsTable.table.getCheckedItems();
                    for (let index of indexes){
                        let id = parentGroupsTable.table.elements.getData("id", index);
                        selectedGroupIds.push(id);
                    }

                    selectedGroupIds.sort();

                    container.groupData.m_parentGroups = selectedGroupIds.join(';');

                    if (!parentGroupsTable.table.elements){
                        updateGroupsModel();
                    }
                }
            }

            GroupHeaderView {
                width: parent.width;

                title: qsTr("Users");
                groupView: usersGroup;
            }

            GroupElementView {
                id: usersGroup;

                width: parent.width;

                TableElementView {
                    id: usersTable;

                    KeyNavigation.tab: rolesTable;
                    KeyNavigation.backtab: parentGroupsTable;

                    Component.onCompleted: {
                        usersTable.table.readOnly = container.readOnly;
                    }

                    Connections {
                        target: usersTable.table;

                        function onCheckedItemsChanged(){
                            container.doUpdateModel();
                        }
                    }
                }

                TreeItemModel {
                    id: headersModel;

                    function updateHeaders(){
                        headersModel.clear();

                        headersModel.insertNewItem();

                        headersModel.setData("id", "name");
                        headersModel.setData("name", qsTr("User Name"));

                        headersModel.refresh();

                        usersTable.table.checkable = true;
                        usersTable.table.headers = headersModel;
                        usersTable.table.elements = container.usersModel;
                    }

                    Component.onCompleted: {
                        updateHeaders();
                    }
                }

                function updateGui(){
                    let userIds = [];
                    let roles = container.groupData.m_users;
                    if (roles !== ""){
                        userIds = roles.split(';');
                    }

                    usersTable.table.uncheckAll();
                    if (usersTable.table.elements){
                        for (let i = 0; i < usersTable.table.elements.getItemsCount(); i++){
                            let id = usersTable.table.elements.getData("id", i);
                            if (userIds.includes(id)){
                                usersTable.table.checkItem(i);
                            }
                        }
                    }
                }

                function updateModel(){
                    let selectedUserIds = []

                    let indexes = usersTable.table.getCheckedItems();
                    for (let index of indexes){
                        let id = usersTable.table.elements.getData("id", index);
                        selectedUserIds.push(id);
                    }

                    selectedUserIds.sort()

                    let result = selectedUserIds.join(';');
                    container.groupData.m_users = result;
                }
            }

            GroupHeaderView {
                width: parent.width;

                title: qsTr("Roles");
                groupView: rolesGroup;
            }

            GroupElementView {
                id: rolesGroup;

                width: parent.width;

                TableElementView {
                    id: rolesTable;

                    KeyNavigation.tab: nameInput;
                    KeyNavigation.backtab: usersTable;

                    Component.onCompleted: {
                        rolesTable.table.readOnly = container.readOnly;
                    }

                    Connections {
                        target: rolesTable.table;

                        function onCheckedItemsChanged(){
                            container.doUpdateModel();
                        }
                    }
                }

                TreeItemModel {
                    id: rolesHeadersModel;

                    function updateHeaders(){
                        rolesHeadersModel.clear();

                        rolesHeadersModel.insertNewItem();

                        rolesHeadersModel.setData("id", "roleName");
                        rolesHeadersModel.setData("name", qsTr("Role Name"));

                        rolesHeadersModel.refresh();

                        rolesTable.table.checkable = true;;
                        rolesTable.table.elements = container.rolesModel;
                        rolesTable.table.headers = rolesHeadersModel;
                    }

                    Component.onCompleted: {
                        updateHeaders();
                    }
                }

                function updateGui(){
                    let roleIds = [];
                    let roles = container.groupData.m_roles;
                    if (roles !== ""){
                        roleIds = roles.split(';');
                    }

                    rolesTable.table.uncheckAll();
                    if (rolesTable.table.elements){
                        for (let i = 0; i < rolesTable.table.elements.getItemsCount(); i++){
                            let id = rolesTable.table.elements.getData("id", i);
                            if (roleIds.includes(id)){
                                rolesTable.table.checkItem(i);
                            }
                        }
                    }
                }

                function updateModel(){
                    let selectedRoleIds = []
                    let indexes = rolesTable.table.getCheckedItems();
                    for (let index of indexes){
                        let id = rolesTable.table.elements.getData("id", index);
                        selectedRoleIds.push(id);
                    }

                    selectedRoleIds.sort();

                    let result = selectedRoleIds.join(';');
                    container.groupData.m_roles = result;
                }
            }
        }
    }
}


