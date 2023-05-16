import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import Acf 1.0

Item {
    id: groupUsersContainer;

    property TreeItemModel documentModel: TreeItemModel {}
    property UndoRedoManager undoRedoManager: null;

    property bool blockUpdatingModel: false;

    Component.onDestruction: {
        usersProvider.collectionModel.dataChanged.disconnect(groupUsersContainer.updateModel);
    }

    onDocumentModelChanged: {
        console.log("GroupUsers onDocumentModelChanged", documentModel);

        usersProvider.updateModel();
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

        for (let i = 0; i < usersTable.elements.GetItemsCount(); i++){
            let id = usersTable.elements.GetData("Id", i);
            if (userIds.includes(id)){
                usersTable.elements.SetData("CheckedState", Qt.Checked, i);
            }
            else{
                usersTable.elements.SetData("CheckedState", Qt.Unchecked, i);
            }
        }

        groupUsersContainer.blockUpdatingModel = false;
    }

    function updateModel(){
        if (groupUsersContainer.blockUpdatingModel){
            return;
        }

        groupUsersContainer.undoRedoManager.beginChanges();

        let selectedUserIds = []
        for (let i = 0; i < usersTable.elements.GetItemsCount(); i++){
            let id = usersTable.elements.GetData("Id", i);
            let state = usersTable.elements.GetData("CheckedState", i);
            if (state === Qt.Checked){
                selectedUserIds.push(id)
            }
        }

        let result = selectedUserIds.join(';');
        groupUsersContainer.documentModel.SetData("Users", result);

        groupUsersContainer.undoRedoManager.endChanges();
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
                for (let i = 0; i < usersProvider.collectionModel.GetItemsCount(); i++){
                    usersProvider.collectionModel.SetData("CheckedState", Qt.Unchecked, i);
                }

                usersTable.elements = usersProvider.collectionModel;
                groupUsersContainer.updateGui();
                usersProvider.collectionModel.dataChanged.connect(groupUsersContainer.updateModel);
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
    }
}//Container
