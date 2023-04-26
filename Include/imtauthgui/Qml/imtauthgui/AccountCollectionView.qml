import QtQuick 2.12
import imtgui 1.0

CollectionView {
    id: container;

    Component.onCompleted: {
        container.commandsDelegatePath = "../../imtauthgui/AccountCollectionViewCommandsDelegate.qml";
    }

    function fillContextMenuModel(){
        contextMenuModel.append({"Id": "Edit", "Name": qsTr("Edit"), "IconSource": "../../../../Icons/Light/Edit_On_Normal.svg"});
        contextMenuModel.append({"Id": "Remove", "Name": qsTr("Remove"), "IconSource": "../../../../Icons/Light/Remove_On_Normal.svg"});
        contextMenuModel.append({"Id": "SetDescription", "Name": qsTr("Set Description"), "IconSource": ""});
    }
}
