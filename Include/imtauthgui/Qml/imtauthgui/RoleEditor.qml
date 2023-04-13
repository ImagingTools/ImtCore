import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import Acf 1.0

Item {
    id: roleEditorContainer;

    property TreeItemModel documentModel: TreeItemModel {}
    property UndoRedoManager undoRedoManager: null;
    property Item documentBase: null;

    property bool blockUpdatingModel: false;

    property int mainMargin: 0;
    property int panelWidth: 400;

    onBlockUpdatingModelChanged: {
        Events.sendEvent("DocumentUpdating", roleEditorContainer.blockUpdatingModel);
    }

    onDocumentModelChanged: {
        rolesProvider.updateModel();
    }

    CollectionDataProvider {
        id: rolesProvider;

        commandId: "Roles";
        fields: ["Id", "Name", "Description"];

        Component.onDestruction: {
            rolesProvider.collectionModel.modelChanged.disconnect(roleEditorContainer.updateModel);
        }

        onModelUpdated: {
            if (rolesProvider.collectionModel != null){
                if (rolesProvider.collectionModel.ContainsKey("Roles")){
                    let rolesModel = rolesProvider.collectionModel.GetData("Roles")

                    for (let i = 0; i < rolesModel.GetItemsCount(); i++){
                        rolesModel.SetData("CheckedState", Qt.Unchecked, i);
                    }

                    parentRolesTable.elements = rolesModel;

                    roleEditorContainer.updateGui();
                    rolesModel.modelChanged.connect(roleEditorContainer.updateModel);

                    roleNameInput.focus = true;
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

    function updateGui(){
        console.log("RoleEditor updateGui", documentModel.toJSON());

        roleEditorContainer.blockUpdatingModel = true;

        if (roleEditorContainer.documentModel.ContainsKey("RoleId")){
            roleIdInput.text = documentModel.GetData("RoleId");
        }

        if (roleEditorContainer.documentModel.ContainsKey("Name")){
            roleNameInput.text = documentModel.GetData("Name");
        }

        if (roleEditorContainer.documentModel.ContainsKey("Description")){
            descriptionInput.text = documentModel.GetData("Description");
        }

        let parentRolesIds = []
        if (roleEditorContainer.documentModel.ContainsKey("ParentRoles")){
            let parentGroups = roleEditorContainer.documentModel.GetData("ParentRoles");
            if (parentGroups !== ""){
                parentRolesIds = parentGroups.split(';')
            }
        }

        if (parentRolesTable.elements){
            for (let i = 0; i < parentRolesTable.elements.GetItemsCount(); i++){
                let id = parentRolesTable.elements.GetData("Id", i);
                if (parentRolesIds.includes(id)){
                    parentRolesTable.elements.SetData("CheckedState", Qt.Checked, i);
                }
                else{
                    parentRolesTable.elements.SetData("CheckedState", Qt.Unchecked, i);
                }
            }
        }

        roleEditorContainer.blockUpdatingModel = false;
    }

    function updateModel(){
        console.log("RoleEditor updateModel");
        if (roleEditorContainer.blockUpdatingModel){
            return;
        }

        roleEditorContainer.undoRedoManager.beginChanges();

        roleEditorContainer.documentModel.SetData("RoleId", roleIdInput.text);
        roleEditorContainer.documentModel.SetData("Name", roleNameInput.text);
        roleEditorContainer.documentModel.SetData("Description", descriptionInput.text);

        let selectedRoleIds = []
        for (let i = 0; i < parentRolesTable.elements.GetItemsCount(); i++){
            let id = parentRolesTable.elements.GetData("Id", i);
            let state = parentRolesTable.elements.GetData("CheckedState", i);
            if (state === Qt.Checked){
                selectedRoleIds.push(id)
            }
        }
        roleEditorContainer.documentModel.SetData("ParentRoles", selectedRoleIds.join(';'));

        roleEditorContainer.undoRedoManager.endChanges();
    }

    Flickable {
        anchors.fill: parent;
        anchors.leftMargin: 0;

        contentWidth: columnContainer.width + columnContainer.anchors.leftMargin;
        contentHeight: columnContainer.height + columnContainer.anchors.topMargin;

        boundsBehavior: Flickable.StopAtBounds;

        clip: true;

        Item{
            id: columnContainer;

            anchors.top: parent.top;
            anchors.left: parent.left;
            anchors.leftMargin: 2;
            anchors.topMargin: 2;

            width: roleEditorContainer.panelWidth;
            height: bodyColumn.height + 2*bodyColumn.anchors.topMargin;
            Loader{
                id: mainPanelFrameLoader;
                anchors.fill: parent;
                sourceComponent: Style.frame !==undefined ? Style.frame: emptyDecorator;
            }
            Column {
                id: bodyColumn;

                anchors.top: parent.top;
                anchors.left: parent.left;
                anchors.topMargin: roleEditorContainer.mainMargin;
                anchors.leftMargin: roleEditorContainer.mainMargin;

                width: roleEditorContainer.panelWidth - 2*anchors.leftMargin;

                spacing: 10;


                Text {
                    id: titleRoleName;

                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;

                    text: qsTr("Role Name");

                    Loader{
                        id: titleDecoratorLoader1;

                        sourceComponent: Style.inputTitleDecorator !==undefined ? Style.inputTitleDecorator: emptyDecorator;
                        onLoaded: {
                            if(titleDecoratorLoader1.item){
                                titleDecoratorLoader1.item.rootItem = titleRoleName;
                            }
                        }
                    }
                }

                CustomTextField {
                    id: roleNameInput;

                    width: parent.width;
                    height: 30;

                    placeHolderText: qsTr("Enter the role name");

                    onEditingFinished: {
                        console.log("roleNameInput onEditingFinished");
                        let oldText = roleEditorContainer.documentModel.GetData("Name");
                        if (oldText && oldText !== roleNameInput.text || !oldText && roleNameInput.text !== ""){
                            roleIdInput.text = roleNameInput.text.replace(/\s+/g, '');
                            roleEditorContainer.updateModel();
                        }
                    }

                    KeyNavigation.tab: roleIdInput;

                    Loader{
                        id: inputDecoratorLoader1;

                        sourceComponent: Style.textFieldDecorator !==undefined ? Style.textFieldDecorator: emptyDecorator;
                        onLoaded: {
                            if(inputDecoratorLoader1.item){
                                inputDecoratorLoader1.item.rootItem = roleNameInput;
                            }
                        }
                    }
                }

                Text {
                    id: titleRoleId;

                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;

                    text: qsTr("Role-ID");

                    Loader{
                        id: titleDecoratorLoader2;

                        sourceComponent: Style.inputTitleDecorator !==undefined ? Style.inputTitleDecorator: emptyDecorator;
                        onLoaded: {
                            if(titleDecoratorLoader2.item){
                                titleDecoratorLoader2.item.rootItem = titleRoleId;
                            }
                        }
                    }
                }

                CustomTextField {
                    id: roleIdInput;

                    width: parent.width;
                    height: 30;
                    readOnly: true;

                    KeyNavigation.tab: descriptionInput;

                    Loader{
                        id: inputDecoratorLoader2;

                        sourceComponent: Style.textFieldDecorator !==undefined ? Style.textFieldDecorator: emptyDecorator;
                        onLoaded: {
                            if(inputDecoratorLoader2.item){
                                inputDecoratorLoader2.item.rootItem = roleIdInput;
                            }
                        }
                    }
                }

                Text {
                    id: titleDescription;

                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;

                    text: qsTr("Description");

                    Loader{
                        id: titleDecoratorLoader3;

                        sourceComponent: Style.inputTitleDecorator !==undefined ? Style.inputTitleDecorator: emptyDecorator;
                        onLoaded: {
                            if(titleDecoratorLoader3.item){
                                titleDecoratorLoader3.item.rootItem = titleDescription;
                            }
                        }
                    }
                }

                CustomTextField {
                    id: descriptionInput;

                    width: parent.width;
                    height: 30;

                    placeHolderText: qsTr("Enter the description");

                    onEditingFinished: {
                        let oldText = roleEditorContainer.documentModel.GetData("Description");
                        if (oldText && oldText !== descriptionInput.text || !oldText && descriptionInput.text !== ""){
                            roleEditorContainer.updateModel();
                        }
                    }

                    KeyNavigation.tab: roleNameInput;

                    Loader{
                        id: inputDecoratorLoader3;

                        sourceComponent: Style.textFieldDecorator !==undefined ? Style.textFieldDecorator: emptyDecorator;
                        onLoaded: {
                            if(inputDecoratorLoader3.item){
                                inputDecoratorLoader3.item.rootItem = descriptionInput;
                            }
                        }
                    }
                }

                Text {
                    id: titleIncludes;

                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;

                    text: qsTr("Parent Roles");

                    Loader{
                        id: titleDecoratorLoader4;

                        sourceComponent: Style.inputTitleDecorator !==undefined ? Style.inputTitleDecorator: emptyDecorator;
                        onLoaded: {
                            if(titleDecoratorLoader4.item){
                                titleDecoratorLoader4.item.rootItem = titleIncludes;
                            }
                        }
                    }
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

                AuxTable {
                    id: parentRolesTable;

                    width: parent.width;
                    height: 300;

                    elements: TreeItemModel {}

                    checkable: true;

                    radius: 0;
                }
            }//Column bodyColumn
        }
    }//Flickable

}//Container
