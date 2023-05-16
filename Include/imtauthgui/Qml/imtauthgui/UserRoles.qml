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
                    for (let i = 0; i < rolesModel.GetItemsCount(); i++){
                        rolesModel.SetData("CheckedState", Qt.Unchecked, i);
                    }

                    rolesTable.elements = rolesModel;

                    userRolesContainer.updateGui();

                    rolesProvider.collectionModel.dataChanged.connect(userRolesContainer.updateModel);
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

        for (let i = 0; i < rolesTable.elements.GetItemsCount(); i++){
            let id = rolesTable.elements.GetData("Id", i);
            if (roleIds.includes(id)){
                rolesTable.elements.SetData("CheckedState", Qt.Checked, i);
            }
            else{
                rolesTable.elements.SetData("CheckedState", Qt.Unchecked, i);
            }
        }

        userRolesContainer.blockUpdatingModel = false;
    }

    function updateModel(){
        console.log("UserRoles updateModel");
        if (userRolesContainer.blockUpdatingModel){
            return;
        }

        userRolesContainer.undoRedoManager.beginChanges();

        let selectedRoleIds = []
        for (let i = 0; i < rolesTable.elements.GetItemsCount(); i++){
            let id = rolesTable.elements.GetData("Id", i);
            let state = rolesTable.elements.GetData("CheckedState", i);
            if (state === Qt.Checked){
                selectedRoleIds.push(id)
            }
        }

        let result = selectedRoleIds.join(';');
        userRolesContainer.documentModel.SetData("Roles", result);

        userRolesContainer.undoRedoManager.endChanges();
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
    }
}//Container
