import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import Acf 1.0

Item {
    id: userRolesContainer;

    property TreeItemModel documentModel: TreeItemModel {}
    property UndoRedoManager undoRedoManager: null;

    property bool blockUpdatingModel: false;

    Component.onCompleted: {
        rolesProvider.updateModel();
    }

    onDocumentModelChanged: {
        rolesProvider.updateModel();
    }

    onVisibleChanged: {
        if (userRolesContainer.visible){
            userRolesContainer.updateGui();
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

                    userRolesContainer.updateGui();
                }
            }
        }
    }

    function updateGui(){
        console.log("UserRoles updateGui");
        userRolesContainer.blockUpdatingModel = true;

        let roleIds = [];
        if (userRolesContainer.documentModel.ContainsKey("Roles")){
            let roles = userRolesContainer.documentModel.GetData("Roles")
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

        userRolesContainer.blockUpdatingModel = false;
    }

    function updateModel(){
        console.log("UserRoles updateModel");
        if (userRolesContainer.blockUpdatingModel){
            return;
        }

        if (userRolesContainer.undoRedoManager){
            userRolesContainer.undoRedoManager.beginChanges();
        }

        let selectedRoleIds = []
        let indexes = rolesTable.checkedIndexes;
        for (let index of indexes){
            let id = rolesTable.elements.GetData("Id", index);
            selectedRoleIds.push(id);
        }

        let result = selectedRoleIds.join(';');
        userRolesContainer.documentModel.SetData("Roles", result);

        if (userRolesContainer.undoRedoManager){
            userRolesContainer.undoRedoManager.endChanges();
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

//    AuxButton {
//        id: addProduct;

//        anchors.top: parent.top;
//        anchors.left: parent.left;

//        height: 22;
//        width: height;

//        iconSource: "../../../Icons/" + Style.theme + "/Add_On_Normal.svg";

//        gradient: Gradient {
//            GradientStop { position: 0.0; color: Style.imagingToolsGradient1; }
//            GradientStop { position: 0.97; color: Style.imagingToolsGradient2; }
//            GradientStop { position: 0.98; color: Style.imagingToolsGradient3; }
//            GradientStop { position: 1.0; color: Style.imagingToolsGradient4; }
//        }
//        onClicked: {

////            documentManager.
//        }
//    }

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
            if (userRolesContainer.blockUpdatingModel){
                return;
            }

            let indexes = rolesTable.checkedIndexes;
            let roles = userRolesContainer.documentModel.GetData("Roles");
            let roleIDs = [];
            for (let index of indexes){
                let groupId = rolesTable.elements.GetData("Id", index);
                roleIDs.push(groupId);
            }

            let newRoles = roleIDs.join(';');
            if (roles !== newRoles){
                userRolesContainer.documentModel.SetData("Roles", roleIDs.join(';'));
                undoRedoManager.makeChanges();
            }
        }
    }
}//Container
