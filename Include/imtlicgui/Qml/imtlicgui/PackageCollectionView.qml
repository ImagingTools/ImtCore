import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    property alias itemId: packagesCollectionView.itemId;
    CollectionView {
        id: packagesCollectionView;
//        anchors.left: parent.left;
//        anchors.right: packageMetaInfo.left;
//        height: parent.height;
        anchors.fill: parent;
        //    color: "red";
        Component.onCompleted: {
            packagesCollectionView.gqlModelInfo = "FeaturePackageInfo"
            packagesCollectionView.gqlModelItems = "FeaturePackageList"
        }

        onSelectItem: {
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
