import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtqml 1.0
import imtcolqml 1.0

Item {
    id: userRolesContainer;

    property TreeItemModel documentModel: TreeItemModel {}
    property Item documentPtr: null;

    property int radius: 3;

    property bool completed: rolesProvider.completed;

    Component.onCompleted: {
        rolesProvider.updateModel();
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
                }
            }
        }
    }

    function blockEditing(){
        rolesTable.readOnly = true;
    }

    function updateGui(){
        console.log("UserRoles updateGui");
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
    }

    function updateModel(){
        console.log("UserRoles updateModel");
        let selectedRoleIds = []
        let indexes = rolesTable.getCheckedItems();
        for (let index of indexes){
            let id = rolesTable.elements.GetData("Id", index);
            selectedRoleIds.push(id);
        }

        let result = selectedRoleIds.join(';');
        userRolesContainer.documentModel.SetData("Roles", result);
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

        width: 400;

        elements: TreeItemModel {}

        checkable: true;
        radius: userRolesContainer.radius;

        onCheckedItemsChanged: {
            if (userRolesContainer.documentPtr){
                userRolesContainer.documentPtr.doUpdateModel();
            }
        }
    }
}//Container
