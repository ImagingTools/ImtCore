import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import Acf 1.0

Item {
    id: groupUsersContainer;

    property TreeItemModel documentModel: TreeItemModel {}
    property UndoRedoManager undoRedoManager: null;

    property bool blockUpdatingModel: false;

    onDocumentModelChanged: {
        console.log("GroupUsers onDocumentModelChanged", documentModel);

        usersProvider.updateModel();
    }

    function blockEditing(){
        usersTable.readOnly = true;
    }

    function updateGui(){
        console.log("GroupUsers updateGui");
        groupUsersContainer.blockUpdatingModel = true;

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

        groupUsersContainer.blockUpdatingModel = false;
    }

    function updateModel(){
        if (groupUsersContainer.blockUpdatingModel){
            return;
        }

        if (groupUsersContainer.undoRedoManager){
            groupUsersContainer.undoRedoManager.beginChanges();
        }

        let selectedUserIds = []

        let indexes = usersTable.getCheckedItems();
        for (let index of indexes){
            let id = usersTable.elements.GetData("Id", index);
            selectedUserIds.push(id);
        }

        let result = selectedUserIds.join(';');
        groupUsersContainer.documentModel.SetData("Users", result);

        if (groupUsersContainer.undoRedoManager){
            groupUsersContainer.undoRedoManager.endChanges();
        }
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
                groupUsersContainer.updateGui();
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
//        anchors.leftMargin: 10;

        width: 400;

        checkable: true;
        radius: 0;

        onCheckedItemsChanged: {
            if (groupUsersContainer.blockUpdatingModel){
                return;
            }

            let indexes = usersTable.getCheckedItems();
            let users = groupUsersContainer.documentModel.GetData("Users");
            let userIDs = [];
            for (let index of indexes){
                let userId = usersTable.elements.GetData("Id", index);
                userIDs.push(userId);
            }

            let newUsers = userIDs.join(';');
            if (users !== newUsers){
                groupUsersContainer.documentModel.SetData("Users", userIDs.join(';'));
                undoRedoManager.makeChanges();
            }
        }
    }
}//Container
