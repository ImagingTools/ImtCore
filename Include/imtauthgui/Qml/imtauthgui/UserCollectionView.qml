import QtQuick 2.15
import Acf 1.0
import imtgui 1.0
import imtqml 1.0
import QtQuick.Dialogs 1.3

CollectionViewWithMenu {
    id: userCollectionViewContainer;

    Component.onCompleted: {
        console.log("UserCollection onCompleted", model.index);
        userCollectionViewContainer.commandUpdateGui = "UserCollectionUpdateGui";
        userCollectionViewContainer.commandsDelegatePath = "../../imtauthgui/UserCollectionViewCommandsDelegate.qml";
    }
}
