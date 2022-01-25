import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    id: packegeCollectionContainer;
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
        console.log("PackageCollectionView commandsChanged!", commandsId, packegeCollectionContainer.rootItem);
        if (commandsId !== "Packages"){
            return;
        }
        console.log("packageCollectionView.selectedIndex", packageCollectionView.selectedIndex);
        if (packageCollectionView.selectedIndex > -1) {

            packegeCollectionContainer.rootItem.setModeMenuButton("Remove", "Normal");
            packegeCollectionContainer.rootItem.setModeMenuButton("Edit", "Normal");
        } else {
            packegeCollectionContainer.rootItem.setModeMenuButton("Remove", "Disabled");
            packegeCollectionContainer.rootItem.setModeMenuButton("Edit", "Disabled");
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

            if (itemId === "") {
                name = "New Package";
            }

            //multiDocView.addToHeadersArray(itemId, name,  "../../imtlicgui/PackageView.qml", "PackageEdit")
            packegeCollectionContainer.multiDocViewItem.addToHeadersArray(itemId, name,  "../../imtlicgui/PackageView.qml", "PackageEdit")
        }

        onSelectedIndexChanged: {
            console.log("packageCollectionView onSelectedIndexChanged", packageCollectionView.selectedIndex);
            if (packageCollectionView.selectedIndex > -1){
                packegeCollectionContainer.commandsChanged("Packages")
            }
        }


    }
}
