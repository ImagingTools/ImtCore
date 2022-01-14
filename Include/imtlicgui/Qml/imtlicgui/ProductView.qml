import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    anchors.fill: parent;
    property alias itemId: productCollectionView.itemId;
    property alias model: productCollectionView.model;

    function menuActivated(menuId) {
        productCollectionView.menuActivated(menuId)
    }

    function commandsChanged(commandsId){
        productCollectionView.commandsChanged(commandsId);
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
