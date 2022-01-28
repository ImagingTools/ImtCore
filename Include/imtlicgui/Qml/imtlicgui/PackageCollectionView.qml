import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    id: packageCollectionContainer;
    anchors.fill: parent;
    property Item rootItem;
    property Item multiDocViewItem;
    property alias itemId: packageCollectionView.itemId;
    property alias itemName: packageCollectionView.itemName;
    property alias model: packageCollectionView.model;

    function refresh() {
        packageCollectionView.refresh();
    }

    function menuActivated(menuId) {
        packageCollectionView.menuActivated(menuId)
    }

    function commandsChanged(commandsId) {
        console.log("PackageCollectionView commandsChanged!", commandsId, packageCollectionContainer.rootItem);
        if (commandsId !== "Packages"){
            return;
        }
        console.log("packageCollectionView.selectedIndex", packageCollectionView.selectedIndex);
        if (packageCollectionView.selectedIndex > -1) {

            packageCollectionContainer.rootItem.setModeMenuButton("Remove", "Normal");
            packageCollectionContainer.rootItem.setModeMenuButton("Edit", "Normal");
        } else {
            packageCollectionContainer.rootItem.setModeMenuButton("Remove", "Disabled");
            packageCollectionContainer.rootItem.setModeMenuButton("Edit", "Disabled");
        }
    }

    CollectionView {
        id: packageCollectionView;
        anchors.fill: parent;
        autoRefresh: true;
        Component.onCompleted: {
            packageCollectionView.gqlModelInfo = "FeaturePackageInfo"
            packageCollectionView.gqlModelItems = "FeaturePackageList";
            packageCollectionView.gqlModelRemove = "FeaturePackageRemove";
            console.log("packageCollectionView onCompleted", packageCollectionView.gqlModelInfo)
        }

        onSelectItem: {
            console.log("packageCollectionView onSelectItem", packageCollectionView.selectedIndex);
            console.log("Item id = ", itemId);
            console.log("Name = ", name);
            var typeOperation = "Open";
            if (itemId === "") {
                name = "New Package";
                typeOperation = "New";
            }
            packageCollectionContainer.multiDocViewItem.addToHeadersArray(itemId, name,  "../../imtlicgui/PackageView.qml", "PackageEdit", typeOperation)
        }

        onSelectedIndexChanged: {
            console.log("packageCollectionView onSelectedIndexChanged", packageCollectionView.selectedIndex);
            if (packageCollectionView.selectedIndex > -1){
                packageCollectionContainer.commandsChanged("Packages")
            }
        }


    }
}
