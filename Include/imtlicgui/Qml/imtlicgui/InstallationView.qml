import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    anchors.fill: parent;
    property alias itemId: installationsCollectionView.itemId;
    property alias model: installationsCollectionView.model;

    CollectionView {
        id: installationsCollectionView;
        anchors.left: parent.left;
        anchors.right: installationMetaInfo.left;
        height: parent.height;
//        property string itemId;
        //    anchors.fill: parent;
        //    color: "red";
        onItemIdChanged: {
            if (installationsCollectionView.itemId){
                console.log("installationView onItemIdChanged")
                installationsCollectionView.gqlModelInfo = "InstallationInfo"
                installationsCollectionView.gqlModelItems = "InstallationList"
            }
        }

        onSelectItem: {
//            multiDocView.addToHeadersArray(name,  "../../imtlicgui/PackageView.qml")
        }


    }
    Rectangle {
        id: installationMetaInfo;
        anchors.right: parent.right;
        height: parent.height;
        width: 150;
        color: "transparent";
    }
}
