import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    id: featureCollectionViewContainer;
    anchors.fill: parent;
    property alias itemId: featureCollectionView.itemId;
    property alias model: featureCollectionView.model;

    function menuActivated(menuId) {
        featureCollectionView.menuActivated(menuId)
    }

    function commandsChanged(commandsId){
        if (commandsId !== "PackageEdit") {
            return;
        }

        if (featureCollectionView.selectedIndex > -1) {
            docsDataDeleg.setModeMenuButton("Remove", "Active");
            docsDataDeleg.setModeMenuButton("Edit", "Active");
            docsDataDeleg.setModeMenuButton("Import", "Active");
            docsDataDeleg.setModeMenuButton("Export", "Active");
            docsDataDeleg.setModeMenuButton("Save", "Active");
            docsDataDeleg.setModeMenuButton("Close", "Active");
        } else {
            docsDataDeleg.setModeMenuButton("Remove", "Disabled");
            docsDataDeleg.setModeMenuButton("Edit", "Disabled");
            docsDataDeleg.setModeMenuButton("Import", "Disabled");
            docsDataDeleg.setModeMenuButton("Export", "Disabled");
            docsDataDeleg.setModeMenuButton("Save", "Disabled");
            docsDataDeleg.setModeMenuButton("Close", "Disabled");
        }
    }

    CollectionView {
        id: featureCollectionView;
        anchors.left: parent.left;
        anchors.right: packageMetaInfo.left;
        height: parent.height;
        onItemIdChanged: {
            if (featureCollectionView.itemId){
                console.log("PackageView onItemIdChanged")
                featureCollectionView.gqlModelInfo = "PackageInfo"
                featureCollectionView.gqlModelItems = "FeatureList"
            }
        }

        onSelectedIndexChanged: {
            console.log("featurePackageCollectionView onSelectedIndexChanged", featureCollectionView.selectedIndex);
            if (featureCollectionView.selectedIndex > -1){
                featureCollectionViewContainer.commandsChanged("PackageEdit")
            }
        }

    }
    Rectangle {
        id: packageMetaInfo;
        anchors.right: parent.right;
        height: parent.height;
        width: 150;
        color: "transparent";
    }
}
