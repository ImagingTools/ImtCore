import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    anchors.fill: parent;
    property alias itemId: productCollectionView.itemId;
    property alias model: productCollectionView.model;

    function menuActivated(menuId) {

        if (menuId  === "New"){
            var countItems = model.GetData("data").GetItemsCount();
            var dataModelLocal = model.GetData("data");
            var index = dataModelLocal.InsertNewItem();

            dataModelLocal.SetData("Name", "Product Name", index);//
            dataModelLocal.SetData("Id", "", index);//

            model.SetData("data", dataModelLocal);
            model.Refresh();
            featureCollectionView.refresh();

        }
        else if (menuId  === "Save") {
            //saveModel.updateModel()
        } else {
             productCollectionView.menuActivated(menuId)
        }
    }

    function commandsChanged(commandsId){
        if (commandsId !== "ProductEdit") {
            return;
        }

        if (productCollectionView.selectedIndex > -1) {
            docsDataDeleg.setModeMenuButton("Remove", "Active");
            docsDataDeleg.setModeMenuButton("Edit", "Active");
            docsDataDeleg.setModeMenuButton("Duplicate", "Active");
            docsDataDeleg.setModeMenuButton("Import", "Active");
            docsDataDeleg.setModeMenuButton("Export", "Active");
            docsDataDeleg.setModeMenuButton("Save", "Active");
            docsDataDeleg.setModeMenuButton("Close", "Active");
        } else {
            docsDataDeleg.setModeMenuButton("Remove", "Disabled");
            docsDataDeleg.setModeMenuButton("Edit", "Disabled");
            docsDataDeleg.setModeMenuButton("Duplicate", "Disabled");
            docsDataDeleg.setModeMenuButton("Import", "Disabled");
            docsDataDeleg.setModeMenuButton("Export", "Disabled");
            docsDataDeleg.setModeMenuButton("Save", "Disabled");
            docsDataDeleg.setModeMenuButton("Close", "Disabled");
        }
//        productCollectionView.commandsChanged(commandsId);
    }

    CollectionView {
        id: productCollectionView;
        anchors.left: parent.left;
        anchors.right: productMetaInfo.left;
        height: parent.height;
        onItemIdChanged: {
            if (productCollectionView.itemId){
                console.log("PackageView onItemIdChanged")
                productCollectionView.gqlModelInfo = "ProductInfo"
                productCollectionView.gqlModelItems = "ProductList"
            }
        }

        onSelectItem: {
        }


    }
    Rectangle {
        id: productMetaInfo;
        anchors.right: parent.right;
        height: parent.height;
        width: 150;
        color: "transparent";
    }
}
