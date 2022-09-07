import QtQuick 2.15
import Acf 1.0
import imtgui 1.0
import imtqml 1.0
import QtQuick.Dialogs 1.3

CollectionViewWithMenu {
    id: roleCollectionViewContainer;

    Component.onCompleted: {
        console.log("RoleCollection onCompleted", model.index);
        roleCollectionViewContainer.commandUpdateGui = "RoleCollectionUpdateGui";
        roleCollectionViewContainer.commandsDelegatePath = "../../imtauthgui/RoleCollectionViewCommandsDelegate.qml";
    }
}
