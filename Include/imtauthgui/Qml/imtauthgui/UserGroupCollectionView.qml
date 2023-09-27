import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtqml 1.0

CollectionView {
    id: userGroupCollectionViewContainer;

    Component.onCompleted: {
        userGroupCollectionViewContainer.commandsDelegatePath = "../../imtauthgui/UserGroupCollectionViewCommandsDelegate.qml";
    }

    function fillContextMenuModel(){
        contextMenuModel.append({"Id": "Edit", "Name": qsTr("Edit"), "IconSource": "../../../../" + Style.getIconPath("Icons/Edit", Icon.State.On, Icon.Mode.Normal)});
        contextMenuModel.append({"Id": "Remove", "Name": qsTr("Remove"), "IconSource": "../../../../" + Style.getIconPath("Icons/Remove", Icon.State.On, Icon.Mode.Normal)});
        contextMenuModel.append({"Id": "SetDescription", "Name": qsTr("Set Description"), "IconSource": ""});
    }
}
