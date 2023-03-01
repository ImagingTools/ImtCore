import QtQuick 2.15
import Acf 1.0
import imtgui 1.0

CollectionView {
    id: userCollectionViewContainer;

    Component.onCompleted: {
        console.log("UserCollection onCompleted", model.index);
        userCollectionViewContainer.commandUpdateGui = "UserCollectionUpdateGui";
        userCollectionViewContainer.commandsDelegatePath = "../../imtauthgui/UserCollectionViewCommandsDelegate.qml";
    }

    function fillContextMenuModel(){
        contextMenuModel.append({"Id": "Edit", "Name": qsTr("Edit"), "IconSource": "../../../../Icons/Light/Edit_On_Normal.svg"});
        contextMenuModel.append({"Id": "Remove", "Name": qsTr("Remove"), "IconSource": "../../../../Icons/Light/Remove_On_Normal.svg"});
        contextMenuModel.append({"Id": "Rename", "Name": qsTr("Rename"), "IconSource": ""});
    }
}
