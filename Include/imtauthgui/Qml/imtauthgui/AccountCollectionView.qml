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
    property alias itemName: accountCollectionView.itemName;
    property alias model: accountCollectionView.model;

    function menuActivated(menuId) {
        console.log("AccountCollectionView menuActivated ", menuId);
        accountCollectionView.menuActivated(menuId)
    }

    function refresh() {
        accountCollectionView.refresh();
    }

    function commandsChanged(commandsId) {
        console.log("AccountCollectionView commandsChanged!", commandsId);
        if (commandsId !== "Accounts"){
            return;
        }
        if (accountCollectionView.selectedIndex > -1) {
            accountCollectionContainer.rootItem.setModeMenuButton("Remove", "Normal");
            accountCollectionContainer.rootItem.setModeMenuButton("Edit", "Normal");
        } else {
            accountCollectionContainer.rootItem.setModeMenuButton("Remove", "Disabled");
            accountCollectionContainer.rootItem.setModeMenuButton("Edit", "Disabled");
        }
    }

    CollectionView {
        id: accountCollectionView;
        anchors.fill: parent;
        autoRefresh: true;

        Component.onCompleted: {
            accountCollectionView.gqlModelInfo = "AccountInfo"
            accountCollectionView.gqlModelItems = "AccountList"
            accountCollectionView.gqlModelRemove = "AccountRemove"
        }

        onSelectItem: {
            console.log("Item id = ", selectedId);
            console.log("Name = ", name);
            var typeOperation = "Open";
            if (selectedId === "") {
                name = "New Account";
                typeOperation = "New";
            }
//            name.replace(/[^a-zа-яё^0-9]/gi, ''); удалить все кроме букв и цифр

//            var id = name.replace(/\s/g, '');
            var id = name;
            accountCollectionContainer.multiDocViewItem.activeCollectionItem = accountCollectionContainer;
            accountCollectionContainer.multiDocViewItem.addToHeadersArray(id, name,  "../../imtauthgui/ContactInfoEditor.qml", "AccountEdit", typeOperation);
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
