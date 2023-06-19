import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import Acf 1.0

Item {
    id: groupRolesContainer;

    property TreeItemModel documentModel: TreeItemModel {}
    property UndoRedoManager undoRedoManager: null;

    property bool blockUpdatingModel: false;

    onDocumentModelChanged: {
        console.log("UserRoles onDocumentModelChanged", documentModel);

//        rolesProvider.updateModel({"ProductId" : window.productId});
        rolesProvider.updateModel();
    }

    onVisibleChanged: {
        if (groupRolesContainer.visible){
            groupRolesContainer.updateGui();
        }
    }

    function blockEditing(){
        rolesTable.readOnly = true;
    }

    function updateGui(){
        console.log("UserRoles updateGui");

        groupRolesContainer.blockUpdatingModel = true;

        let roleIds = [];
        if (groupRolesContainer.documentModel.ContainsKey("Roles")){
            let roles = groupRolesContainer.documentModel.GetData("Roles")
            if (roles !== ""){
                roleIds = roles.split(';');
            }
        }

        rolesTable.uncheckAll();
        if (rolesTable.elements){
            for (let i = 0; i < rolesTable.elements.GetItemsCount(); i++){
                let id = rolesTable.elements.GetData("Id", i);
                if (roleIds.includes(id)){
                    rolesTable.checkItem(i);
                }
            }
        }

        groupRolesContainer.blockUpdatingModel = false;
    }

    function updateModel(){
        if (groupRolesContainer.blockUpdatingModel){
            return;
        }

        if (groupRolesContainer.undoRedoManager){
            groupRolesContainer.undoRedoManager.beginChanges();
        }

        let selectedRoleIds = []
        let indexes = rolesTable.getCheckedItems();
        for (let index of indexes){
            let id = rolesTable.elements.GetData("Id", index);
            selectedRoleIds.push(id);
        }

        let result = selectedRoleIds.join(';');
        groupRolesContainer.documentModel.SetData("Roles", result);

        if (groupRolesContainer.undoRedoManager){
            groupRolesContainer.undoRedoManager.endChanges();
        }
    }

    Component{
        id: emptyDecorator;
        Item{
            property Item rootItem: null;
        }
    }

    CollectionDataProvider {
        id: rolesProvider;

        commandId: "Roles";

        fields: ["Id", "Name"];

        onModelUpdated: {
            if (rolesProvider.collectionModel != null){
                if (rolesProvider.collectionModel.ContainsKey("Roles")){
                    let rolesModel = rolesProvider.collectionModel.GetData("Roles");
                    rolesTable.elements = rolesModel;

                    groupRolesContainer.updateGui();
                }
            }
        }
    }

    Rectangle {
        id: background;
        anchors.fill: parent;
        color: Style.backgroundColor;

        Loader {
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

        text: qsTr("Roles");
        color: Style.textColor;
        font.family: Style.fontFamily;
        font.pixelSize: Style.fontSize_common;
    }

    TreeItemModel {
        id: headersModel;

        Component.onCompleted: {
            headersModel.InsertNewItem();

            headersModel.SetData("Id", "Name");
            headersModel.SetData("Name", "Role Name");

            rolesTable.headers = headersModel;
        }
    }

    AuxTable {
        id: rolesTable;

        anchors.top: titleRoles.bottom;
        anchors.topMargin: 7;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 10;
        anchors.left: parent.left;
//        anchors.leftMargin: 10;

        width: 400;

        elements: TreeItemModel {}

        checkable: true;
        radius: 0;

        onCheckedItemsChanged: {
            if (groupRolesContainer.blockUpdatingModel){
                return;
            }

            let indexes = rolesTable.getCheckedItems();
            let roles = groupRolesContainer.documentModel.GetData("Roles");
            let roleIDs = [];
            for (let index of indexes){
                let roleId = rolesTable.elements.GetData("Id", index);
                roleIDs.push(roleId);
            }

            let newGroups = roleIDs.join(';');
            if (roles !== newGroups){
                groupRolesContainer.documentModel.SetData("Roles", roleIDs.join(';'));
                undoRedoManager.makeChanges();
            }
        }
    }
}//Container
