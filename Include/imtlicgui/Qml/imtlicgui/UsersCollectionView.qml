import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0
import QtQuick.Dialogs 1.3

Item {
    id: usersCollectionContainer;

    anchors.fill: parent;

    property Item rootItem;
    property Item multiDocViewItem;

    property alias itemId: usersCollectionView.itemId;
    property alias itemName: usersCollectionView.itemName;
    property alias model: usersCollectionView.collectionViewModel;

    property string operation;

    ListModel {
        id: contextMenuModel;

        Component.onCompleted: {
            contextMenuModel.append({"id": "Edit", "name": "Edit", "imageSource": "../../../Icons/Light/Edit_On_Normal.svg", "mode": "Normal"});
            contextMenuModel.append({"id": "Remove", "name": "Remove", "imageSource": "../../../Icons/Light/Remove_On_Normal.svg", "mode": "Normal"});
            contextMenuModel.append({"id": "", "name": "", "imageSource": "", "mode": "Normal"});
            contextMenuModel.append({"id": "SetDescription", "name": "Set Description", "imageSource": "", "mode": "Normal"});
            contextMenuModel.append({"id": "Rename", "name": "Rename", "imageSource": "", "mode": "Normal"});
        }
    }

    function openContextMenu(item, mouseX, mouseY) {
        console.log("usersCollectionView openContextMenu", mouseX, mouseY);
        if (usersCollectionView.table.height - mouseY <= 100){
            mouseY = mouseY - 100;
        }
        var point = usersCollectionContainer.mapToItem(thumbnailDecoratorContainer, mouseX, mouseY);
        var source = "AuxComponents/PopupMenuDialog.qml";
        var parameters = {};
        parameters["model"] = contextMenuModel;
        parameters["resultItem"] = usersCollectionContainer;
        parameters["itemHeight"] = 25;
        parameters["itemWidth"] = 150;
        parameters["x"] = point.x;
        parameters["y"] = point.y;

        thumbnailDecoratorContainer.openDialog(source, parameters);
    }

    function menuActivated(menuId) {
    }

    function refresh() {
        usersCollectionView.refresh();
    }

    function commandsChanged(commandsId) {
        console.log("usersCollectionView commandsChanged!", commandsId);
        if (commandsId !== "Users"){
            return;
        }

        if (usersCollectionView.selectedIndex > -1){
            usersCollectionContainer.rootItem.setModeMenuButton("Remove", "Normal");
            usersCollectionContainer.rootItem.setModeMenuButton("Edit", "Normal");
        }
        else{
            usersCollectionContainer.rootItem.setModeMenuButton("Remove", "Disabled");
            usersCollectionContainer.rootItem.setModeMenuButton("Edit", "Disabled");
        }
    }

    function dialogResult(parameters) {
        console.log("usersCollectionView dialogResult", parameters["dialog"], parameters["status"]);
    }

    function selectItem(){
        console.log("usersCollectionView selectItem");

        var itemId = usersCollectionView.table.getSelectedId();
        var name = usersCollectionView.table.getSelectedName();
        usersCollectionView.itemSelect(itemId, name);
    }

    CollectionView {
        id: usersCollectionView;

        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.bottom: parent.bottom;
        anchors.right: parent.right;

//        autoRefresh: true;

        Component.onCompleted: {
            usersCollectionView.gqlModelInfo = "UserInfo";
            usersCollectionView.gqlModelItems = "UserList";
            usersCollectionView.gqlModelRemove = "UserRemove";
        }

//        onItemSelect: {
//            console.log("usersCollectionView onSelectItem", idSelect, name);
//            var typeOperation = "Open";
//            if (idSelect === "") {
//                name = "New user";
//                typeOperation = "New";
//            }
//            usersCollectionContainer.multiDocViewItem.activeCollectionItem = usersCollectionContainer;
//            usersCollectionContainer.multiDocViewItem.addDocument(idSelect, name,  "../../imtlicgui/UserView.qml", "UserEdit", typeOperation)
//        }

//        onCollectionViewRightButtonMouseClicked: {
//            console.log("usersCollectionView CollectionView AuxTable onCollectionViewRightButtonMouseClicked");
//            usersCollectionContainer.openContextMenu(item, mouseX, mouseY);
//        }


//        onSelectedIndexChanged: {
//            if (usersCollectionView.selectedIndex > -1){
//            }
//        }

//        onRemovedItem: {
//            console.log("InstallationCollection CollectionView onRemovedItem");
//            var index = usersCollectionContainer.multiDocViewItem.getTabIndexById(itemId);
//            if (index !== -1){
//                usersCollectionContainer.multiDocViewItem.closeTab(index);
//            }
//        }

//        onRenamedItem: {
//            console.log("InstallationCollection CollectionView onRenamedItem");
//            var index = usersCollectionContainer.multiDocViewItem.getTabIndexById(oldId);
//            if (index !== -1){
//                usersCollectionContainer.multiDocViewItem.updateTitleTab(newId, newId, index);
//            }
////            metaInfo.getMetaInfo();
//        }

//        onSetDescriptionItem: {
////            metaInfo.getMetaInfo();
//        }
    }
}
