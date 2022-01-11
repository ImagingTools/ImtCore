import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    anchors.fill: parent;
    property alias itemId: accountCollectionView.itemId;
    property alias model: accountCollectionView.model;

    function menuActivated(menuId) {
        accountCollectionView.menuActivated(menuId)
    }

    CollectionView {
        id: accountCollectionView;
//        anchors.left: parent.left;
//        anchors.right: packageMetaInfo.left;
//        height: parent.height;
        anchors.fill: parent;
        //    color: "red";
        Component.onCompleted: {
            accountCollectionView.gqlModelInfo = "AccountInfo"
            accountCollectionView.gqlModelItems = "AccountList"
        }

        onSelectItem: {
            console.log("Item id = ", itemId);
            console.log("Name = ", name);
            //multiDocView.addToHeadersArray(itemId, name,  "../../imtlicgui/AccountView.qml")
            multiDocView.addToHeadersArray(itemId, name,  "../../imtauthgui/ContactInfoEditor.qml")
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
