import QtQuick 2.15
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthUsersSdl 1.0
import imtdocgui 1.0

ViewBase {
    id: container;

    anchors.fill: parent;

    property TreeItemModel rolesModel: TreeItemModel {}
    property TreeItemModel groupsModel: TreeItemModel {}

    property UserData userData: model;
    property string productId;

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

        userData.m_productId = container.productId;
    }

    onUserDataChanged: {
        if (!userData){
            return;
        }

        setBlockingUpdateModel(true);
        let systemInfosModel = userData.m_systemInfos;
        if (systemInfoTable.table){
            systemInfoTable.table.elements = systemInfosModel;

            if (systemInfoTable.table.elementsCount <= 1){
                headerSystemInfoGroup.visible = false;
                systemInfoGroup.visible = false;
            }
        }

        checkChangePasswordLogic();

        checkSystemId();

        setBlockingUpdateModel(false);
    }

    function checkChangePasswordLogic(){
        passwordInput.visible = userData.m_id == "";
        changePasswordButton.visible = userData.m_id != "";
    }

    function checkSystemId(){
        if (!userData){
            console.error("Unable to check system ID for the user. Error: UserData is invalid");
            return;
        }

        usernameInput.readOnly = false;
        passwordInput.readOnly = false;

        for (let i = 0; i < userData.m_systemInfos.count; i++){
            let systemId = userData.m_systemInfos.get(i).item.m_id;
            if (systemId !== ""){
                changePasswordButton.visible = false;
                usernameInput.readOnly = true;
            }

            let enabled = userData.m_systemInfos.get(i).item.m_enabled;
            if (enabled && systemId !== ""){
                usernameInput.readOnly = true;
                passwordInput.readOnly = true;
                changePasswordButton.visible = false;
            }
            else if (enabled && systemId === ""){
                passwordInput.readOnly = false;
            }
        }
    }

	DocumentHistoryPanel {
		id: historyPanel;
		documentId: container.userData ? container.userData.m_id : "";
		collectionId: "Users";
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
        anchors.topMargin: Style.size_largeMargin;

        anchors.bottom: parent.bottom;
        anchors.bottomMargin: Style.size_largeMargin;

        anchors.left: parent.left;
        anchors.leftMargin: Style.size_largeMargin;

        anchors.right: scrollbar.left;
        anchors.rightMargin: Style.size_largeMargin;

        contentWidth: bodyColumn.width;
		contentHeight: Math.max(bodyColumn.height + 2 * Style.size_largeMargin, historyPanel.contentHeight + 2 * Style.size_largeMargin);

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
                        let oldText = container.userData.m_username;
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
                    visible: false;

                    onEditingFinished: {
                        let oldText = container.userData.m_password;
                        if (oldText && oldText !== passwordInput.text || !oldText && passwordInput.text !== ""){
                            container.doUpdateModel();
                        }
                    }

                    KeyNavigation.tab: nameInput;
                    KeyNavigation.backtab: usernameInput;
                }

                ElementView {
                    id: changePasswordButton;
                    name: qsTr("Change password");
                    visible: false;

                    controlComp: Component {
                        Button {
                            width: 100;
                            height: 30;
                            text: qsTr("Change");
                            onClicked: {
                                ModalDialogManager.openDialog(changePasswordComp, {});
                            }
                        }
                    }

                    Component {
                        id: changePasswordComp;
                        ChangePasswordDialog {
                            title: qsTr("Change Password");
                            onFinished: {
                                if (buttonId == Enums.save){
                                    AuthorizationController.changePassword(container.userData.m_id, contentItem.oldPassword, contentItem.newPassword);
                                }
                            }
                        }
                    }
                }

                TextInputElementView {
                    id: nameInput;

                    name: qsTr("Name");
                    placeHolderText: qsTr("Enter the name");
                    readOnly: container.readOnly;

                    onEditingFinished: {
                        let oldText = container.userData.m_name;
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
                    usernameInput.text = container.userData.m_username;
                    nameInput.text = container.userData.m_name;
                    mailInput.text = container.userData.m_email;
                    passwordInput.text = container.userData.m_password;
                }

                function updateModel(){
                    container.userData.m_username = usernameInput.text;
                    container.userData.m_name = nameInput.text;
                    container.userData.m_email = mailInput.text;
                    container.userData.m_password = passwordInput.text;
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
                            headersModel2.setData("Id", "m_name", index)
                            headersModel2.setData("Name", qsTr("System Name"), index)

                            if (systemInfoTable.table){
                                systemInfoTable.table.headers = headersModel2;
                            }
                        }
                    }

                    onTableChanged: {
                        if (table){
                            table.checkable = true;
                            table.isMultiCheckable = false;
                        }
                    }

                    Connections {
                        id: systemInfoTableConn;
                        target: systemInfoTable.table;

                        function onCheckedItemsChanged(){
                            if (systemInfoGroup.block){
                                return;
                            }

                            let indexes = systemInfoTable.table.getCheckedItems();
                            if (indexes.length === 0){
                                systemInfoGroup.block = true;
                                systemInfoTable.table.checkItem(0);
                                systemInfoGroup.block = false;
                                // systemInfoGroup.updateGui()
                            }

                            container.doUpdateModel();
                            container.checkSystemId();
                        }
                    }
                }

                property bool block: false;

                function updateGui(){
                    if (!container.userData){
                        return;
                    }

                    if (systemInfoTable.table){
                        systemInfoTable.table.uncheckAll();
                        let systemInfosModel = container.userData.m_systemInfos;
                        if (systemInfosModel){
                            for (let i = 0; i < systemInfosModel.count; i++){
                                let enabled = systemInfosModel.get(i).item.m_enabled;
                                if (enabled){
                                    systemInfoTable.table.checkItem(i);
                                }
                            }
                        }
                    }
                }

                function updateModel(){
                    if (!container.userData){
                        return;
                    }

                    let indexes = systemInfoTable.table.getCheckedItems();

                    if (container.userData.m_systemInfos){
                        for (let i = 0; i < container.userData.m_systemInfos.count; i++){
                            container.userData.m_systemInfos.get(i).item.m_enabled = indexes.includes(i)
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

                        headersModel.setData("Id", "RoleName");
                        headersModel.setData("Name", qsTr("Role Name"));

                        headersModel.refresh();

                        if (rolesTable.table){
                            rolesTable.table.checkable = true;
                            rolesTable.table.headers = headersModel;
                            rolesTable.table.elements = container.rolesModel;
                        }
                    }

                    Component.onCompleted: {
                        updateHeaders();
                    }
                }

                function updateGui(){
                    let roleIds = [];
                    let roles = container.userData.m_roles;
                    if (roles !== ""){
                        roleIds = roles.split(';');
                    }

                    if (rolesTable.table){
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
                }

                function updateModel(){
                    if (rolesTable.table){
                        let selectedRoleIds = []
                        let indexes = rolesTable.table.getCheckedItems();
                        for (let index of indexes){
                            let id = rolesTable.table.elements.getData("Id", index);
                            selectedRoleIds.push(id);
                        }

                        selectedRoleIds.sort();

                        let result = selectedRoleIds.join(';');
                        container.userData.m_roles = result;
                    }
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
                        if (groupsTable.table){
                            groupsTable.table.readOnly = container.readOnly;
                        }
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

                        if (groupsTable.table){
                            groupsTable.table.checkable = true;
                            groupsTable.table.headers = groupsHeadersModel;
                            groupsTable.table.elements = container.groupsModel;
                        }
                    }

                    Component.onCompleted: {
                        updateHeaders();
                    }
                }

                function updateGui(){
                    let groupIds = []
                    let parentGroups = container.userData.m_groups;
                    if (parentGroups !== ""){
                        groupIds = parentGroups.split(';')
                    }

                    if (groupsTable.table){
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
                }

                function updateModel(){
                    let selectedGroupIds = []

                    if (groupsTable.table){
                        let indexes = groupsTable.table.getCheckedItems();
                        for (let index of indexes){
                            let id = groupsTable.table.elements.getData("Id", index);
                            selectedGroupIds.push(id);
                        }

                        selectedGroupIds.sort();
                        container.userData.m_groups = selectedGroupIds.join(';')
                    }
                }
            }
        }
    }
}
