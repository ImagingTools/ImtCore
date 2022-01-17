import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    id: productCollectionContainer;
    anchors.fill: parent;
    property alias itemId: productCollectionView.itemId;
    property alias model: productCollectionView.model;

    function menuActivated(menuId) {
        productCollectionView.menuActivated(menuId)
    }

    function commandsChanged(commandsId) {
        console.log("ProductCollectionView commandsChanged!", commandsId);
        if (commandsId !== "Products"){
            return;
        }
        if (productCollectionView.selectedIndex > -1) {
            docsDataDeleg.setModeMenuButton("Remove", "Active");
            docsDataDeleg.setModeMenuButton("Edit", "Active");
            docsDataDeleg.setModeMenuButton("Duplicate", "Active");
        } else {
            docsDataDeleg.setModeMenuButton("Remove", "Disabled");
            docsDataDeleg.setModeMenuButton("Edit", "Disabled");
            docsDataDeleg.setModeMenuButton("Duplicate", "Disabled");
        }
    }

    CollectionView {
        id: productCollectionView;
        anchors.fill: parent;
        Component.onCompleted: {
            productCollectionView.gqlModelInfo = "ProductInfo"
            productCollectionView.gqlModelItems = "ProductList"
            productCollectionView.gqlModelItems = "ProductRemove"
        }

        onSelectItem: {
            multiDocView.addToHeadersArray(itemId, name,  "../../imtlicgui/ProductView.qml", "ProductEdit")
        }

        onSelectedIndexChanged: {
            if (productCollectionView.selectedIndex > -1){
                productCollectionContainer.commandsChanged("Products")
            }
        }
    }
}
