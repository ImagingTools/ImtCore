import QtQuick 2.15
import Acf 1.0
import imtgui 1.0

CollectionView {
    id: userGroupCollectionViewContainer;

    Component.onCompleted: {
        userGroupCollectionViewContainer.commandsDelegatePath = "../../imtauthgui/UserGroupCollectionViewCommandsDelegate.qml";
        
       // baseCollectionView.commands.fieldsData.push("UserGroupId");
    }
}
