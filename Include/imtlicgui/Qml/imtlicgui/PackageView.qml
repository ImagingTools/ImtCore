import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    property alias itemId: packagesCollectionView.itemId;
    CollectionView {
        id: packagesCollectionView;
        anchors.left: parent.left;
        anchors.right: packageMetaInfo.left;
        height: parent.height;
//        property string itemId;
        //    anchors.fill: parent;
        //    color: "red";
        onItemIdChanged: {
            if (packagesCollectionView.itemId){
                console.log("PackageView onItemIdChanged")
                packagesCollectionView.gqlModelInfo = "PackageInfo"
                packagesCollectionView.gqlModelItems = "FeatureList"
            }
        }

        onSelectItem: {
//            multiDocView.addToHeadersArray(name,  "../../imtlicgui/PackageView.qml")
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
