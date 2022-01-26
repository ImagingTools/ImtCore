import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    id: productCollectionContainer;
    anchors.fill: parent;
    property Item rootItem;
    property Item multiDocViewItem;
    property alias itemId: productCollectionView.itemId;
    property alias model: productCollectionView.model;

    function refresh() {
        productCollectionView.refresh();
    }

    function menuActivated(menuId) {
        productCollectionView.menuActivated(menuId)
    }

    function commandsChanged(commandsId) {
        console.log("ProductCollectionView commandsChanged!", commandsId);
        if (commandsId !== "Products"){
            return;
        }
        if (productCollectionView.selectedIndex > -1) {
            productCollectionContainer.rootItem.setModeMenuButton("Remove", "Normal");
            productCollectionContainer.rootItem.setModeMenuButton("Edit", "Normal");
            productCollectionContainer.rootItem.setModeMenuButton("Duplicate", "Normal");
        } else {
            productCollectionContainer.rootItem.setModeMenuButton("Remove", "Disabled");
            productCollectionContainer.rootItem.setModeMenuButton("Edit", "Disabled");
            productCollectionContainer.rootItem.setModeMenuButton("Duplicate", "Disabled");
        }
    }

    CollectionView {
        id: productCollectionView;
        anchors.fill: parent;
        autoRefresh: true;
        Component.onCompleted: {
            productCollectionView.gqlModelInfo = "ProductCollectionInfo"
            productCollectionView.gqlModelItems = "ProductList"
            productCollectionView.gqlModelRemove = "ProductRemove"
        }

        onSelectItem: {
            console.log("ProductCollectionView productCollectionView onSelectItem", itemId, name);
            if (itemId === "") {
                name = "New Product";
            }

            productCollectionContainer.multiDocViewItem.addToHeadersArray(itemId, name,  "../../imtlicgui/ProductView.qml", "ProductEdit")
//            multiDocView.addToHeadersArray(itemId, name,  "../../imtlicgui/ProductView.qml", "ProductEdit")
        }

        onSelectedIndexChanged: {
            if (productCollectionView.selectedIndex > -1){
                productCollectionContainer.commandsChanged("Products")
            }
        }
    }
}
