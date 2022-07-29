import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtlicgui 1.0

CollectionViewCommandsDelegateBase {
    id: container;

    onCommandsIdChanged: {
        Events.subscribeEvent(container.commandsId + "CommandActivated", container.commandActivated);
    }

    function commandActivated(commandId){
        console.log("productCollectionViewContainer commandActivated", commandId);

        if (commandId == "Duplicate"){
            let itemId = container.collectionView.baseCollectionView.table.getSelectedId();
            let itemName = container.collectionView.baseCollectionView.table.getSelectedName();
            container.collectionView.rootItem.addPage({"Id": itemId, "Name": itemName, "Source": container.collectionView.editorPath, "CommandsId": container.collectionView.editorCommandsId});
        }
        else{
            container.commandActivatedBase(commandId);
        }
    }
}

