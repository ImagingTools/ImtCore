import QtQuick 2.15
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

ViewBase {
    id: container;

    anchors.fill: parent;

    property TreeItemModel rolesModel: TreeItemModel {}
    property TreeItemModel groupsModel: TreeItemModel {}

    function updateGui(){
        generalGroup.updateGui();
        rolesGroup.updateGui();
        groupsBlock.updateGui();
        systemInfoGroup.updateGui();
    }

    function updateModel(){
        generalGroup.updateModel();
        rolesGroup.updateModel();
        groupsBlock.updateModel();
        systemInfoGroup.updateModel();
    }

    onModelChanged: {
        let systemInfosModel = container.model.getData("SystemInfos");
        systemInfoTable.table.elements = systemInfosModel;

        if (systemInfoTable.table.elementsCount <= 1){
            headerSystemInfoGroup.visible = false;
            systemInfoGroup.visible = false;
        }

        for (let i = 0; i < systemInfosModel.getItemsCount(); i++){
            let systemId = systemInfosModel.getData("Id", i);
            if (systemId !== ""){
                usernameInput.readOnly = true;
                break;
            }
        }

        systemInfoTableConn.enabled = true;
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
                    id: usernameInput;

                    name: qsTr("Username");
                    placeHolderText: qsTr("Enter the user name");
                    description: qsTr();

                    readOnly: container.readOnly;

                    onEditingFinished: {
                        let oldText = container.model.getData("Username");
                        if (oldText && oldText !== usernameInput.text || !oldText && usernameInput.text !== ""){

                            container.doUpdateModel();
                        }
                    }

                    KeyNavigation.tab: passwordInput;
                    KeyNavigation.backtab: groupsTable;
                }

                TextInputElementView {
                    id: passwordInput;

                    name: qsTr("Password");
                    placeHolderText: qsTr("Enter the password");

                    echoMode: TextInput.Password;

                    readOnly: container.readOnly;

                    onEditingFinished: {
                        let oldText = container.model.getData("Password");
                        if (oldText && oldText !== passwordInput.text || !oldText && passwordInput.text !== ""){
                            container.doUpdateModel();
                        }
                    }

                    KeyNavigation.tab: nameInput;
                    KeyNavigation.backtab: usernameInput;
                }

                TextInputElementView {
                    id: nameInput;

                    name: qsTr("Name");
                    placeHolderText: qsTr("Enter the name");
                    readOnly: container.readOnly;

                    onEditingFinished: {
                        let oldText = container.model.getData("Name");
                        if (oldText && oldText !== nameInput.text || !oldText && nameInput.text !== ""){
                            container.doUpdateModel();
                        }
                    }

                    KeyNavigation.tab: mailInput;
                    KeyNavigation.backtab: passwordInput;
                }

                RegularExpressionValidator {
                    id: mailValid;

                    regularExpression: /\w+([-+.']\w+)*@\w+([-.]\w+)*\.\w+([-.]\w+)*/;
                }

                TextInputElementView {
                    id: mailInput;

                    name: qsTr("Email Address");
                    textInputValidator: mailValid;

                    placeHolderText: qsTr("Enter the email");
                    readOnly: container.readOnly;

                    onEditingFinished: {
                        container.doUpdateModel();
                    }

                    KeyNavigation.tab: usernameInput;
                    KeyNavigation.backtab: nameInput;
                }

                function updateGui(){
                    if (container.model.containsKey("Username")){
                        usernameInput.text = container.model.getData("Username");
                    }
                    else{
                        usernameInput.text = "";
                    }

                    if (container.model.containsKey("Name")){
                        nameInput.text = container.model.getData("Name");
                    }
                    else{
                        nameInput.text = "";
                    }

                    if (container.model.containsKey("Email")){
                        mailInput.text = container.model.getData("Email");
                    }
                    else{
                        mailInput.text = "";
                    }

                    if (container.model.containsKey("Password")){
                        passwordInput.text = container.model.getData("Password");
                    }
                    else{
                        passwordInput.text = "";
                    }
                }

                function updateModel(){
                    container.model.setData("Username", usernameInput.text);
                    container.model.setData("Name", nameInput.text);
                    container.model.setData("Email", mailInput.text);
                    container.model.setData("Password", passwordInput.text);
                }
            }

            GroupHeaderView {
                id: headerSystemInfoGroup;
                width: parent.width;

                title: qsTr("System Information");
                groupView: systemInfoGroup;
            }

            GroupElementView {
                id: systemInfoGroup;
                width: parent.width;

                TableElementView {
                    id: systemInfoTable;
                    TreeItemModel {
                        id: headersModel2;

                        Component.onCompleted: {
                            updateModel();
                        }

                        function updateModel(){
                            headersModel2.clear();

                            let index = headersModel2.insertNewItem();
                            headersModel2.setData("Id", "Name", index)
                            headersModel2.setData("Name", qsTr("System Name"), index)

                            systemInfoTable.table.headers = headersModel2;
                        }
                    }

                    onTableChanged: {
                        if (table){
                            table.checkable = true;
                        }
                    }

                    Connections {
                        id: systemInfoTableConn;
                        target: systemInfoTable.table;
                        enabled: false;

                        function onCheckedItemsChanged(){
                            container.doUpdateModel();
                        }
                    }
                }

                function updateGui(){
                    systemInfoTable.table.uncheckAll();
                    let systemInfosModel = container.model.getData("SystemInfos");
                    if (systemInfosModel){
                        for (let i = 0; i < systemInfosModel.getItemsCount(); i++){
                            let enabled = systemInfosModel.getData("Enabled", i);

                            if (enabled){
                                systemInfoTable.table.checkItem(i);
                            }
                        }
                    }
                }

                function updateModel(){
                    let systemInfosModel = container.model.getData("SystemInfos");
                    if (systemInfosModel){
                        let indexes = systemInfoTable.table.getCheckedItems();

                        for (let i = 0; i < systemInfosModel.getItemsCount(); i++){
                            systemInfosModel.setData("Enabled", indexes.includes(i), i);
                        }
                    }
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

                    KeyNavigation.tab: groupsTable;
                    KeyNavigation.backtab: mailInput;

                    Connections {
                        target: rolesTable.table;

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

                        headersModel.setData("Id", "Name");
                        headersModel.setData("Name", qsTr("Role Name"));

                        headersModel.refresh();

                        rolesTable.table.checkable = true;
                        rolesTable.table.headers = headersModel;
                        rolesTable.table.elements = container.rolesModel;
                    }

                    Component.onCompleted: {
                        updateHeaders();
                    }
                }

                function updateGui(){
                    let roleIds = [];
                    if (container.model.containsKey("Roles")){
                        let roles = container.model.getData("Roles")
                        if (roles !== ""){
                            roleIds = roles.split(';');
                        }
                    }

                    rolesTable.table.uncheckAll();

                    if (rolesTable.table.elements){
                        for (let i = 0; i < rolesTable.table.elements.getItemsCount(); i++){
                            let id = rolesTable.table.elements.getData("Id", i);
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
                        let id = rolesTable.table.elements.getData("Id", index);
                        selectedRoleIds.push(id);
                    }

                    selectedRoleIds.sort();

                    let result = selectedRoleIds.join(';');
                    container.model.setData("Roles", result);
                }
            }

            GroupHeaderView {
                width: parent.width;

                title: qsTr("Groups");
                groupView: groupsBlock;
            }

            GroupElementView {
                id: groupsBlock;

                width: parent.width;

                TableElementView {
                    id: groupsTable;

                    KeyNavigation.tab: usernameInput;
                    KeyNavigation.backtab: rolesTable;

                    Component.onCompleted: {
                        groupsTable.table.readOnly = container.readOnly;
                    }

                    Connections {
                        target: groupsTable.table;

                        function onCheckedItemsChanged(){
                            container.doUpdateModel();
                        }
                    }
                }

                TreeItemModel {
                    id: groupsHeadersModel;

                    function updateHeaders(){
                        groupsHeadersModel.clear();

                        let index = groupsHeadersModel.insertNewItem();
                        groupsHeadersModel.setData("Id", "Name", index);
                        groupsHeadersModel.setData("Name", qsTr("Group Name"), index);

                        index = groupsHeadersModel.insertNewItem();

                        groupsHeadersModel.setData("Id", "Description", index);
                        groupsHeadersModel.setData("Name", qsTr("Description"), index);

                        groupsHeadersModel.refresh();

                        groupsTable.table.checkable = true;
                        groupsTable.table.headers = groupsHeadersModel;
                        groupsTable.table.elements = container.groupsModel;
                    }

                    Component.onCompleted: {
                        updateHeaders();
                    }
                }

                function updateGui(){
                    let groupIds = []
                    if (container.model.containsKey("Groups")){
                        let parentGroups = container.model.getData("Groups");
                        if (parentGroups !== ""){
                            groupIds = parentGroups.split(';')
                        }
                    }

                    groupsTable.table.uncheckAll();
                    if (groupsTable.table.elements){
                        for (let i = 0; i < groupsTable.table.elements.getItemsCount(); i++){
                            let id = groupsTable.table.elements.getData("Id", i);
                            if (groupIds.includes(id)){
                                groupsTable.table.checkItem(i);
                            }
                        }
                    }
                }

                function updateModel(){
                    let selectedGroupIds = []

                    let indexes = groupsTable.table.getCheckedItems();
                    for (let index of indexes){
                        let id = groupsTable.table.elements.getData("Id", index);
                        selectedGroupIds.push(id);
                    }

                    selectedGroupIds.sort();

                    container.model.setData("Groups", selectedGroupIds.join(';'));
                }
            }
        }
    }
}
