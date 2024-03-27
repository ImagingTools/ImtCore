import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

ViewBase {
    id: userRolesContainer;

    property int radius: 3;

    property TreeItemModel rolesModel: TreeItemModel {}

    Component.onCompleted: {
        Events.subscribeEvent("OnLocalizationChanged", userRolesContainer.onLocalizationChanged);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("OnLocalizationChanged", userRolesContainer.onLocalizationChanged);
    }

    function onLocalizationChanged(language){
        headersModel.updateHeaders();
    }

    function updateGui(){
        let roleIds = [];
        if (userRolesContainer.model.ContainsKey("Roles")){
            let roles = userRolesContainer.model.GetData("Roles")
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
        let selectedRoleIds = []
        let indexes = rolesTable.getCheckedItems();
        for (let index of indexes){
            let id = rolesTable.elements.GetData("Id", index);
            selectedRoleIds.push(id);
        }

        let result = selectedRoleIds.join(';');
        userRolesContainer.model.SetData("Roles", result);
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

        text: qsTr("Roles");
        color: Style.textColor;
        font.family: Style.fontFamily;
        font.pixelSize: Style.fontSize_common;
    }

    TreeItemModel {
        id: headersModel;

        function updateHeaders(){
            headersModel.Clear();

            headersModel.InsertNewItem();

            headersModel.SetData("Id", "Name");
            headersModel.SetData("Name", qsTr("Role Name"));

            headersModel.Refresh();

            rolesTable.headers = headersModel;
        }

        Component.onCompleted: {
            updateHeaders();
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

        elements: userRolesContainer.rolesModel;

        checkable: true;
        radius: userRolesContainer.radius;

        readOnly: userRolesContainer.readOnly;

        onCheckedItemsChanged: {
            userRolesContainer.doUpdateModel();
        }
    }
}//Container
