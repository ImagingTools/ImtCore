import QtQuick 2.15
import Acf 1.0
import imtgui 1.0

CollectionView {
    id: userGroupCollectionViewContainer;

    Component.onCompleted: {
        userGroupCollectionViewContainer.commandsDelegatePath = "../../imtauthgui/UserGroupCollectionViewCommandsDelegate.qml";
    }

    function fillContextMenuModel(){
        contextMenuModel.append({"Id": "Edit", "Name": qsTr("Edit"), "IconSource": "../../../../Icons/Light/Edit_On_Normal.svg"});
        contextMenuModel.append({"Id": "Remove", "Name": qsTr("Remove"), "IconSource": "../../../../Icons/Light/Remove_On_Normal.svg"});
        contextMenuModel.append({"Id": "SetDescription", "Name": qsTr("Set Description"), "IconSource": ""});
    }
}
