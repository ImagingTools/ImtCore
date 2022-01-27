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
    property alias itemName: productCollectionView.itemName;
    property alias model: productCollectionView.model;

    function refresh() {
        productCollectionView.refresh();
    }

    function menuActivated(menuId) {
        console.log("ProductCollectionView menuActivated", menuId);
        if (menuId === "Duplicate") {
//            var count = productCollectionView.model.GetItemsCount();
//            console.log("Count =", count);
            var dataModelLocal = productCollectionView.model.GetData("data");
//            var index = dataModelLocal.InsertNewItem();

            var currentName = dataModelLocal.GetData("Name", productCollectionView.selectedIndex);
            var currentId = dataModelLocal.GetData("Id", productCollectionView.selectedIndex);

//            var duplicateName = "Copy of " + currentName;

//            dataModelLocal.SetData("Name", duplicateName, index);
//            dataModelLocal.SetData("Id", currentId, index);

//            model.SetData("data", dataModelLocal);
//            model.Refresh();
//            productCollectionView.refresh();
            var name = "Copy of " + currentName;

            productCollectionContainer.multiDocViewItem.addToHeadersArray(currentId, name,  "../../imtlicgui/ProductView.qml", "ProductEdit", "Copy")

        } else {
            productCollectionView.menuActivated(menuId)
        }
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
            var typeOperation = "Open";
            if (itemId === "") {
                name = "New Product";
                typeOperation = "New";
            }

            productCollectionContainer.multiDocViewItem.addToHeadersArray(itemId, name,  "../../imtlicgui/ProductView.qml", "ProductEdit", typeOperation)
        }

        onSelectedIndexChanged: {
            if (productCollectionView.selectedIndex > -1){
                productCollectionContainer.commandsChanged("Products")
            }
        }
    }
}
