import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    anchors.fill: parent;
    property alias itemId: packageCollectionView.itemId;
    property alias model: packageCollectionView.model;
    function menuActivated(menuId) {
        packageCollectionView.menuActivated(menuId)
    }

    CollectionView {
        id: packageCollectionView;
//        anchors.left: parent.left;
//        anchors.right: packageMetaInfo.left;
//        height: parent.height;
        anchors.fill: parent;
        //    color: "red";
        Component.onCompleted: {
            packageCollectionView.gqlModelInfo = "FeaturePackageInfo"
            packageCollectionView.gqlModelItems = "FeaturePackageList"
            console.log("packageCollectionView onCompleted", packageCollectionView.gqlModelInfo)
        }

        onSelectItem: {
            multiDocView.changeCommandsId("PackageEdit")
            multiDocView.addToHeadersArray(itemId, name,  "../../imtlicgui/PackageView.qml")
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
