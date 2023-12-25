import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtqml 1.0
import imtcolqml 1.0

Item {
    id: userGroupsContainer;

    property TreeItemModel documentModel: TreeItemModel {}
    property Item documentPtr: null;

    property int radius: 3;

    property bool completed: groupsProvider.completed;

    Component.onCompleted: {
        groupsProvider.updateModel();
    }

    CollectionDataProvider {
        id: groupsProvider;

        commandId: "Groups";

        fields: ["Id", "Name", "Description", "Roles"];

        onModelUpdated: {
            if (groupsProvider.collectionModel != null){
                groupsTable.elements = groupsProvider.collectionModel;
            }
        }
    }

    function blockEditing(){
        groupsTable.readOnly = true;
    }

    function updateGui(){
        console.log("UserGroups updateGui");

        let groupIds = []
        if (userGroupsContainer.documentModel.ContainsKey("Groups")){
            let parentGroups = userGroupsContainer.documentModel.GetData("Groups");
            if (parentGroups !== ""){
                groupIds = parentGroups.split(';')
            }
        }

        groupsTable.uncheckAll();
        if (groupsTable.elements){
            for (let i = 0; i < groupsTable.elements.GetItemsCount(); i++){
                let id = groupsTable.elements.GetData("Id", i);
                if (groupIds.includes(id)){
                    groupsTable.checkItem(i);
                }
            }
        }
    }

    function updateModel(){
        console.log("UserRoles updateModel");
        let selectedGroupIds = []

        let indexes = groupsTable.getCheckedItems();
        for (let index of indexes){
            let id = groupsTable.elements.GetData("Id", index);
            selectedGroupIds.push(id);
        }

        userGroupsContainer.documentModel.SetData("Groups", selectedGroupIds.join(';'));
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

        text: qsTr("Groups");
        color: Style.textColor;
        font.family: Style.fontFamily;
        font.pixelSize: Style.fontSize_common;
    }

    TreeItemModel {
        id: groupsHeadersModel;

        Component.onCompleted: {
            let index = groupsHeadersModel.InsertNewItem();
            groupsHeadersModel.SetData("Id", "Name", index);
            groupsHeadersModel.SetData("Name", "Group Name", index);

            index = groupsHeadersModel.InsertNewItem();

            groupsHeadersModel.SetData("Id", "Description", index);
            groupsHeadersModel.SetData("Name", "Description", index);

            groupsTable.headers = groupsHeadersModel;
        }
    }

    AuxTable {
        id: groupsTable;

        anchors.top: titleRoles.bottom;
        anchors.topMargin: 7;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 10;
        anchors.left: parent.left;
        width: 400;

        checkable: true;
        radius: userGroupsContainer.radius;

        onCheckedItemsChanged: {
            if (userGroupsContainer.documentPtr){
                userGroupsContainer.documentPtr.doUpdateModel();
            }
        }
    }

}//Container
