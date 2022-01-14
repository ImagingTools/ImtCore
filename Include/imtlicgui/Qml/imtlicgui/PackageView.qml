import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    anchors.fill: parent;
    property alias itemId: featureCollectionView.itemId;
    property alias model: featureCollectionView.model;

    function menuActivated(menuId) {
        featureCollectionView.menuActivated(menuId)
    }

    function commandsChanged(commandsId){
        featureCollectionView.commandsChanged(commandsId);
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

        onSelectItem: {
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
