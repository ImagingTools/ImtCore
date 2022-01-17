import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    id: packegeCollectionContainer;
    anchors.fill: parent;
    property alias itemId: packageCollectionView.itemId;
    property alias model: packageCollectionView.model;

    function menuActivated(menuId) {
        packageCollectionView.menuActivated(menuId)
    }

    function commandsChanged(commandsId) {
        console.log("PackageCollectionView commandsChanged!", commandsId);
        if (commandsId !== "Packages"){
            return;
        }
        console.log("packageCollectionView.selectedIndex", packageCollectionView.selectedIndex);
        if (packageCollectionView.selectedIndex > -1) {
            docsDataDeleg.setModeMenuButton("Remove", "Active");
            docsDataDeleg.setModeMenuButton("Edit", "Active");
        } else {
            docsDataDeleg.setModeMenuButton("Remove", "Disabled");
            docsDataDeleg.setModeMenuButton("Edit", "Disabled");
        }
    }

    CollectionView {
        id: packageCollectionView;
        anchors.fill: parent;
        Component.onCompleted: {
            packageCollectionView.gqlModelInfo = "FeaturePackageInfo"
            packageCollectionView.gqlModelItems = "FeaturePackageList"
            packageCollectionView.gqlModelItems = "FeaturePackageRemove"
            console.log("packageCollectionView onCompleted", packageCollectionView.gqlModelInfo)
        }

        onSelectItem: {
            console.log("packageCollectionView onSelectItem", packageCollectionView.selectedIndex);
            console.log("Item id = ", itemId);
            console.log("Name = ", name);

            if (itemId === "") {
                name = "New Package";
            }

            multiDocView.addToHeadersArray(itemId, name,  "../../imtlicgui/PackageView.qml", "PackageEdit")
        }

        onSelectedIndexChanged: {
            console.log("packageCollectionView onSelectedIndexChanged", packageCollectionView.selectedIndex);
            if (packageCollectionView.selectedIndex > -1){
                packegeCollectionContainer.commandsChanged("Packages")
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
