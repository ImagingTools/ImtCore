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
//        if (commandsId !== "Packages"){
//            return;
//        }

        if (packageCollectionView.selectedIndex > -1) {
            if (commandsId === "Packages"){
                docsDataDeleg.setModeMenuButton("Remove", "Active");
                docsDataDeleg.setModeMenuButton("Edit", "Active");
            }else if (commandsId === "PackageEdit"){
                docsDataDeleg.setModeMenuButton("Remove", "Active");
                docsDataDeleg.setModeMenuButton("Edit", "Active");
                docsDataDeleg.setModeMenuButton("Import", "Active");
                docsDataDeleg.setModeMenuButton("Export", "Active");
                docsDataDeleg.setModeMenuButton("Save", "Active");
                docsDataDeleg.setModeMenuButton("Close", "Active");
            }
        } else {
            if (commandsId === "Packages"){
                docsDataDeleg.setModeMenuButton("Remove", "Disabled");
                docsDataDeleg.setModeMenuButton("Edit", "Disabled");
            }else if (commandsId === "PackageEdit"){
                docsDataDeleg.setModeMenuButton("Remove", "Disabled");
                docsDataDeleg.setModeMenuButton("Edit", "Disabled");
                docsDataDeleg.setModeMenuButton("Import", "Disabled");
                docsDataDeleg.setModeMenuButton("Export", "Disabled");
                docsDataDeleg.setModeMenuButton("Save", "Disabled");
                docsDataDeleg.setModeMenuButton("Close", "Disabled");
            }
        }
    }

    CollectionView {
        id: packageCollectionView;
        anchors.fill: parent;
        Component.onCompleted: {
            packageCollectionView.gqlModelInfo = "FeaturePackageInfo"
            packageCollectionView.gqlModelItems = "FeaturePackageList"
            console.log("packageCollectionView onCompleted", packageCollectionView.gqlModelInfo)
        }

        onSelectItem: {
            multiDocView.addToHeadersArray(itemId, name,  "../../imtlicgui/PackageView.qml", "PackageEdit")
         }

        onSelectedIndexChanged: {
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
