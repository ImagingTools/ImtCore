import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0

ViewBase {
    id: groupUsersContainer;

    property int radius: 3;

    property TreeItemModel usersModel: TreeItemModel {}

    Component.onCompleted: {
        Events.subscribeEvent("OnLocalizationChanged", groupUsersContainer.onLocalizationChanged);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("OnLocalizationChanged", groupUsersContainer.onLocalizationChanged);
    }

    function onLocalizationChanged(language){
        headersModel.updateHeaders();
    }

    function updateGui(){
        console.log("GroupUsers updateGui");

        let userIds = [];
        if (groupUsersContainer.model.ContainsKey("Users")){
            let roles = groupUsersContainer.model.GetData("Users")
            if (roles !== ""){
                userIds = roles.split(';');
            }
        }

        usersTable.uncheckAll();
        if (usersTable.elements){
            for (let i = 0; i < usersTable.elements.GetItemsCount(); i++){
                let id = usersTable.elements.GetData("Id", i);
                if (userIds.includes(id)){
                    usersTable.checkItem(i);
                }
            }
        }

    }

    function updateModel(){
        let selectedUserIds = []

        let indexes = usersTable.getCheckedItems();
        for (let index of indexes){
            let id = usersTable.elements.GetData("Id", index);
            selectedUserIds.push(id);
        }

        selectedUserIds.sort()

        let result = selectedUserIds.join(';');
        groupUsersContainer.model.SetData("Users", result);
    }

    Component{
        id: emptyDecorator;
        Item{
            property Item rootItem: null;
        }
    }

//    CollectionDataProvider {
//        id: usersProvider;

//        commandId: "Users";

//        fields: ["Id", "Name"];

//        onModelUpdated: {
//            if (usersProvider.collectionModel != null){
//                usersTable.elements = usersProvider.collectionModel;

//                groupUsersContainer.doUpdateGui();
//            }
//        }
//    }

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

        text: qsTr("Users");
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
            headersModel.SetData("Name", qsTr("User Name"));

            headersModel.Refresh();

            usersTable.headers = headersModel;
        }

        Component.onCompleted: {
            updateHeaders();
        }
    }

    AuxTable {
        id: usersTable;

        anchors.top: titleRoles.bottom;
        anchors.topMargin: 7;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 10;
        anchors.left: parent.left;

        width: 400;

        elements: groupUsersContainer.usersModel;

        checkable: true;
        radius: groupUsersContainer.radius;

        readOnly: groupUsersContainer.readOnly;

        onCheckedItemsChanged: {
            groupUsersContainer.doUpdateModel();
        }
    }
}//Container
