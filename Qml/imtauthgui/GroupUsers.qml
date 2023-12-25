import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtqml 1.0
import imtcolqml 1.0

Item {
    id: groupUsersContainer;

    property TreeItemModel documentModel: TreeItemModel {}
    property Item documentPtr: null;

    property int radius: 3;

    property bool completed: usersProvider.completed;

    Component.onCompleted: {
        usersProvider.updateModel();
    }

    function blockEditing(){
        usersTable.readOnly = true;
    }

    function updateGui(){
        console.log("GroupUsers updateGui");

        let userIds = [];
        if (groupUsersContainer.documentModel.ContainsKey("Users")){
            let roles = groupUsersContainer.documentModel.GetData("Users")
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

        let result = selectedUserIds.join(';');
        groupUsersContainer.documentModel.SetData("Users", result);
    }

    Component{
        id: emptyDecorator;
        Item{
            property Item rootItem: null;
        }
    }

    CollectionDataProvider {
        id: usersProvider;

        commandId: "Users";

        fields: ["Id", "Name"];

        onModelUpdated: {
            if (usersProvider.collectionModel != null){
                usersTable.elements = usersProvider.collectionModel;
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

        text: qsTr("Users");
        color: Style.textColor;
        font.family: Style.fontFamily;
        font.pixelSize: Style.fontSize_common;
    }

    TreeItemModel {
        id: headersModel;

        Component.onCompleted: {
            headersModel.InsertNewItem();

            headersModel.SetData("Id", "Name");
            headersModel.SetData("Name", "User Name");

            usersTable.headers = headersModel;
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

        checkable: true;
        radius: groupUsersContainer.radius;

        onCheckedItemsChanged: {
            if (groupUsersContainer.documentPtr){
                groupUsersContainer.documentPtr.doUpdateModel();
            }
        }
    }
}//Container
