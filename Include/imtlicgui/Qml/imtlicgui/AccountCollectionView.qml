import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    anchors.fill: parent;
    property alias itemId: packageCollectionView.itemId;

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
            packageCollectionView.gqlModelInfo = "AccountInfo"
            packageCollectionView.gqlModelItems = "AccountList"
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
