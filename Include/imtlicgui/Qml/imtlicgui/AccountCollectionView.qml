import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    id: accountCollectionContainer;
    anchors.fill: parent;
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
            docsDataDeleg.setModeMenuButton("Remove", "Active");
            docsDataDeleg.setModeMenuButton("Edit", "Active");
        } else {
            docsDataDeleg.setModeMenuButton("Remove", "Disabled");
            docsDataDeleg.setModeMenuButton("Edit", "Disabled");
        }
    }

    CollectionView {
        id: accountCollectionView;
        anchors.fill: parent;
        Component.onCompleted: {
            accountCollectionView.gqlModelInfo = "AccountInfo"
            accountCollectionView.gqlModelItems = "AccountList"
        }

        onSelectItem: {
            console.log("Item id = ", itemId);
            console.log("Name = ", name);

            if (itemId === "") {
                name = "New Account";
            }

            multiDocView.addToHeadersArray(itemId, name,  "../../imtauthgui/ContactInfoEditor.qml", "AccountEdit")
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
