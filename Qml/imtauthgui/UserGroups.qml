import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

ViewBase {
    id: userGroupsContainer;

    property int radius: 3;

    property TreeItemModel groupsModel: TreeItemModel {}

    Component.onCompleted: {
        Events.subscribeEvent("OnLocalizationChanged", userGroupsContainer.onLocalizationChanged);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("OnLocalizationChanged", userGroupsContainer.onLocalizationChanged);
    }

    function onLocalizationChanged(language){
        groupsHeadersModel.updateHeaders();
    }

    function updateGui(){
        console.log("UserGroups updateGui");

        let groupIds = []
        if (userGroupsContainer.model.containsKey("Groups")){
            let parentGroups = userGroupsContainer.model.getData("Groups");
            if (parentGroups !== ""){
                groupIds = parentGroups.split(';')
            }
        }

        groupsTable.uncheckAll();
        if (groupsTable.elements){
            for (let i = 0; i < groupsTable.elements.getItemsCount(); i++){
                let id = groupsTable.elements.getData("Id", i);
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
            let id = groupsTable.elements.getData("Id", index);
            selectedGroupIds.push(id);
        }

        selectedGroupIds.sort();

        userGroupsContainer.model.setData("Groups", selectedGroupIds.join(';'));
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

        function updateHeaders(){
            groupsHeadersModel.clear();

            let index = groupsHeadersModel.insertNewItem();
            groupsHeadersModel.setData("Id", "Name", index);
            groupsHeadersModel.setData("Name", qsTr("Group Name"), index);

            index = groupsHeadersModel.insertNewItem();

            groupsHeadersModel.setData("Id", "Description", index);
            groupsHeadersModel.setData("Name", qsTr("Description"), index);

            groupsHeadersModel.refresh();

            groupsTable.headers = groupsHeadersModel;
        }

        Component.onCompleted: {
            updateHeaders();
        }
    }

    Table {
        id: groupsTable;

        anchors.top: titleRoles.bottom;
        anchors.topMargin: 7;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 10;
        anchors.left: parent.left;
        width: 400;

        checkable: true;
        radius: userGroupsContainer.radius;

        elements: userGroupsContainer.groupsModel;

        onCheckedItemsChanged: {
            userGroupsContainer.doUpdateModel();
        }
    }

}//Container
