import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0

ViewBase {
    id: groupRolesContainer;

    property int radius: 3;

    property TreeItemModel rolesModel: TreeItemModel {}

    Component.onCompleted: {
        Events.subscribeEvent("OnLocalizationChanged", groupRolesContainer.onLocalizationChanged);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("OnLocalizationChanged", groupRolesContainer.onLocalizationChanged);
    }

    function onLocalizationChanged(language){
        headersModel.updateHeaders();
    }

    function updateGui(){
        console.log("UserRoles updateGui");

        let roleIds = [];
        if (groupRolesContainer.model.containsKey("Roles")){
            let roles = groupRolesContainer.model.getData("Roles")
            if (roles !== ""){
                roleIds = roles.split(';');
            }
        }

        rolesTable.uncheckAll();
        if (rolesTable.elements){
            for (let i = 0; i < rolesTable.elements.getItemsCount(); i++){
                let id = rolesTable.elements.getData("Id", i);
                if (roleIds.includes(id)){
                    rolesTable.checkItem(i);
                }
            }
        }
    }

    function updateModel(){
        let selectedRoleIds = []
        let indexes = rolesTable.getCheckedItems();
        for (let index of indexes){
            let id = rolesTable.elements.getData("Id", index);
            selectedRoleIds.push(id);
        }

        selectedRoleIds.sort();

        let result = selectedRoleIds.join(';');
        groupRolesContainer.model.setData("Roles", result);
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

        function updateHeaders(){
            headersModel.clear();

            headersModel.insertNewItem();

            headersModel.setData("Id", "Name");
            headersModel.setData("Name", qsTr("Role Name"));

            headersModel.refresh();

            rolesTable.headers = headersModel;
        }

        Component.onCompleted: {
            updateHeaders();
        }
    }

    Table {
        id: rolesTable;

        anchors.top: titleRoles.bottom;
        anchors.topMargin: 7;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 10;
        anchors.left: parent.left;

        width: 400;

        elements: groupRolesContainer.rolesModel;

        checkable: true;
        radius: groupRolesContainer.radius;
        readOnly: groupRolesContainer.readOnly;

        onCheckedItemsChanged: {
            groupRolesContainer.doUpdateModel();
        }
    }
}//Container
