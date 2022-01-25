import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    id: accountCollectionContainer;
    anchors.fill: parent;
    property Item rootItem;
    property Item multiDocViewItem;
    property alias itemId: accountCollectionView.itemId;
    property alias model: accountCollectionView.model;

    function menuActivated(menuId) {
        console.log("AccountCollectionView menuActivated ", menuId);
        accountCollectionView.menuActivated(menuId)
    }

    function commandsChanged(commandsId) {
        console.log("AccountCollectionView commandsChanged!", commandsId);
        if (commandsId !== "Accounts"){
            return;
        }
        if (accountCollectionView.selectedIndex > -1) {
            accountCollectionContainer.rootItem.setModeMenuButton("Remove", "Active");
            accountCollectionContainer.rootItem.setModeMenuButton("Edit", "Active");
        } else {
            accountCollectionContainer.rootItem.setModeMenuButton("Remove", "Disabled");
            accountCollectionContainer.rootItem.setModeMenuButton("Edit", "Disabled");
        }
    }

    CollectionView {
        id: accountCollectionView;
        anchors.fill: parent;
        Component.onCompleted: {
            accountCollectionView.gqlModelInfo = "AccountInfo"
            accountCollectionView.gqlModelItems = "AccountList"
            accountCollectionView.gqlModelRemove = "AccountRemove"
        }

        onSelectItem: {
            console.log("Item id = ", itemId);
            console.log("Name = ", name);

            if (itemId === "") {
                name = "New Account";
            }

            accountCollectionContainer.multiDocViewItem.addToHeadersArray(itemId, name,  "../../imtauthgui/ContactInfoEditor.qml", "AccountEdit");
//            multiDocView.addToHeadersArray(itemId, name,  "../../imtauthgui/ContactInfoEditor.qml", "AccountEdit")
        }

        onSelectedIndexChanged: {
            if (accountCollectionView.selectedIndex > -1){
                accountCollectionContainer.commandsChanged("Accounts");
            }
        }


    }
//    Rectangle {
//        id: packageMetaInfo;
//        anchors.right: parent.right;
//        height: parent.height;
//        width: 100;
//        color: "green";
//    }
}
