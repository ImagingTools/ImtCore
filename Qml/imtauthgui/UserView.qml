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
        console.log("UserView updateGui")

        generalGroup.updateGui();
        rolesGroup.updateGui();
        groupsBlock.updateGui();
    }

    function updateModel(){
        console.log("UserView updateModel")

        generalGroup.updateModel();
        rolesGroup.updateModel();
        groupsBlock.updateModel();
    }

    CustomScrollbar {
        id: scrollbar;

        anchors.right: parent.right;
        anchors.top: flickable.top;
        anchors.bottom: flickable.bottom;

        secondSize: 10;
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

                    readOnly: container.readOnly;

                    onEditingFinished: {
                        let oldText = container.model.GetData("Username");
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
                        let oldText = container.model.GetData("Password");
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
                        let oldText = container.model.GetData("Name");
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
                    if (container.model.ContainsKey("Username")){
                        usernameInput.text = container.model.GetData("Username");
                    }
                    else{
                        usernameInput.text = "";
                    }

                    if (container.model.ContainsKey("Name")){
                        nameInput.text = container.model.GetData("Name");
                    }
                    else{
                        nameInput.text = "";
                    }

                    if (container.model.ContainsKey("Email")){
                        mailInput.text = container.model.GetData("Email");
                    }
                    else{
                        mailInput.text = "";
                    }

                    if (container.model.ContainsKey("Password")){
                        passwordInput.text = container.model.GetData("Password");
                    }
                    else{
                        passwordInput.text = "";
                    }
                }

                function updateModel(){
                    container.model.SetData("Username", usernameInput.text);
                    container.model.SetData("Name", nameInput.text);
                    container.model.SetData("Email", mailInput.text);
                    container.model.SetData("Password", passwordInput.text);
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

                    Component.onCompleted: {
                        rolesTable.table.readOnly = container.readOnly;
                    }

                    Connections {
                        target: rolesTable.table;

                        function onCheckedItemsChanged(){
                            console.log("rolesTable onCheckedItemsChanged")

                            container.doUpdateModel();
                        }
                    }
                }

                TreeItemModel {
                    id: headersModel;

                    function updateHeaders(){
                        headersModel.Clear();

                        headersModel.InsertNewItem();

                        headersModel.SetData("Id", "Name");
                        headersModel.SetData("Name", qsTr("Role Name"));

                        headersModel.Refresh();

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
                    if (container.model.ContainsKey("Roles")){
                        let roles = container.model.GetData("Roles")
                        if (roles !== ""){
                            roleIds = roles.split(';');
                        }
                    }

                    rolesTable.table.uncheckAll();

                    if (rolesTable.table.elements){
                        for (let i = 0; i < rolesTable.table.elements.GetItemsCount(); i++){
                            let id = rolesTable.table.elements.GetData("Id", i);
                            if (roleIds.includes(id)){
                                rolesTable.table.checkItem(i);
                            }
                        }
                    }
                }

                function updateModel(){
                    console.log("Roles updateModel", container.model.ToJson());
                    let selectedRoleIds = []
                    let indexes = rolesTable.table.getCheckedItems();
                    for (let index of indexes){
                        let id = rolesTable.table.elements.GetData("Id", index);
                        selectedRoleIds.push(id);
                    }

                    selectedRoleIds.sort();
                    console.log("selectedRoleIds", selectedRoleIds);

                    let result = selectedRoleIds.join(';');
                    container.model.SetData("Roles", result);
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
                            console.log("groupsTable onCheckedItemsChanged")

                            container.doUpdateModel();
                        }
                    }
                }

                TreeItemModel {
                    id: groupsHeadersModel;

                    function updateHeaders(){
                        groupsHeadersModel.Clear();

                        let index = groupsHeadersModel.InsertNewItem();
                        groupsHeadersModel.SetData("Id", "Name", index);
                        groupsHeadersModel.SetData("Name", qsTr("Group Name"), index);

                        index = groupsHeadersModel.InsertNewItem();

                        groupsHeadersModel.SetData("Id", "Description", index);
                        groupsHeadersModel.SetData("Name", qsTr("Description"), index);

                        groupsHeadersModel.Refresh();

                        groupsTable.table.checkable = true;
                        groupsTable.table.headers = groupsHeadersModel;
                        groupsTable.table.elements = container.groupsModel;
                    }

                    Component.onCompleted: {
                        updateHeaders();
                    }
                }

                function updateGui(){
                    console.log("UserGroups updateGui");

                    let groupIds = []
                    if (container.model.ContainsKey("Groups")){
                        let parentGroups = container.model.GetData("Groups");
                        if (parentGroups !== ""){
                            groupIds = parentGroups.split(';')
                        }
                    }

                    groupsTable.table.uncheckAll();
                    if (groupsTable.table.elements){
                        for (let i = 0; i < groupsTable.table.elements.GetItemsCount(); i++){
                            let id = groupsTable.table.elements.GetData("Id", i);
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
                        let id = groupsTable.table.elements.GetData("Id", index);
                        selectedGroupIds.push(id);
                    }

                    selectedGroupIds.sort();

                    container.model.SetData("Groups", selectedGroupIds.join(';'));
                }
            }
        }
    }
}
