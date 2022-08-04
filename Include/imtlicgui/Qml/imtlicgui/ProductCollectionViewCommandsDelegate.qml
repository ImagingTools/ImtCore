import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0
import imtlicgui 1.0

CollectionViewCommandsDelegateBase {
    id: container;

    onCommandsIdChanged: {
        Events.subscribeEvent(container.commandsId + "CommandActivated", container.commandHandle);
    }

    onSelectedIndexChanged: {
        console.log("ProductCollectionViewCommands onSelectedIndexChanged");

        let mode = container.selectedIndex > -1 ? "Normal" : "Disabled";
        container.commandsProvider.changeCommandMode("Duplicate", mode);
    }

    function commandHandle(commandId){
        console.log("ProductCollectionViewCommandscommandActivated", commandId);

        if (commandId === "Duplicate"){
            let itemId = container.collectionView.baseCollectionView.table.getSelectedId();
            let itemName = container.collectionView.baseCollectionView.table.getSelectedName();

            let copyStr = qsTr("Copy of ");
            container.collectionView.rootItem.addPage({"Id": itemId,
                                                       "Name": copyStr + itemName,
                                                       "Source": container.collectionView.baseCollectionView.commands.objectViewEditorPath,
                                                       "CommandsId": container.collectionView.baseCollectionView.commands.objectViewEditorCommandsId});
        }
        else{
            container.commandHandleBase(commandId);
        }
    }
}
