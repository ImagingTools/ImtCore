import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    anchors.fill: parent;
    property alias itemId: accountsCollectionView.itemId;
    property alias model: accountsCollectionView.model;

    function menuActivated(menuId) {
        accountsCollectionView.menuActivated(menuId)
    }

    CollectionView {
        id: accountsCollectionView;
        anchors.left: parent.left;
        anchors.right: accountMetaInfo.left;
        height: parent.height;
        onItemIdChanged: {
            if (accountsCollectionView.itemId){
                console.log("AccountView onItemIdChanged")
                accountsCollectionView.gqlModelInfo = "AccountInfo"
                accountsCollectionView.gqlModelItems = "AccountList"
            }
        }

        onSelectItem: {
        }


    }
    Rectangle {
        id: accountMetaInfo;
        anchors.right: parent.right;
        height: parent.height;
        width: 150;
        color: "transparent";
    }
}
