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

    CollectionView {
        id: productCollectionView;
//        anchors.left: parent.left;
//        anchors.right: packageMetaInfo.left;
//        height: parent.height;
        anchors.fill: parent;
        //    color: "red";
        Component.onCompleted: {
            productCollectionView.gqlModelInfo = "ProductInfo"
            productCollectionView.gqlModelItems = "ProductList"
        }

        onSelectItem: {
            multiDocView.addToHeadersArray(itemId, name,  "../../imtlicgui/ProductView.qml", "ProductEdit")
        }


    }
}
