import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    anchors.fill: parent;
    property alias itemId: packagesCollectionView.itemId;
    CollectionView {
        id: packagesCollectionView;
//        anchors.left: parent.left;
//        anchors.right: packageMetaInfo.left;
//        height: parent.height;
        anchors.fill: parent;
        //    color: "red";
        Component.onCompleted: {
            packagesCollectionView.gqlModelInfo = "InstallationInfo"
            packagesCollectionView.gqlModelItems = "InstallationList"
        }

        onSelectItem: {
            //multiDocView.addToHeadersArray(itemId, name,  "../../imtlicgui/InstallationView.qml")
            multiDocView.addToHeadersArray(itemId, name,  "../../imtgui/AuxComponents/InstallationInfoEditor.qml")
        }


    }

}
