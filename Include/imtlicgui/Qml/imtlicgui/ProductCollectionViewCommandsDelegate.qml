import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0
import imtlicgui 1.0

CollectionViewCommandsDelegateBase {
    id: container;

    onSelectedIndexChanged: {
        console.log("ProductCollectionViewCommands onSelectedIndexChanged");

        let mode = container.selectedIndex > -1 ? "Normal" : "Disabled";
        commandsProvider.changeCommandMode("Duplicate", mode);
    }

    onCommandActivated: {
        if (commandId === "Duplicate"){
            let itemId = container.collectionView.baseCollectionView.table.getSelectedId();
            let itemName = container.collectionView.baseCollectionView.table.getSelectedName();

            let copyStr = qsTr("Copy of ");
            multiDocView.addDocument({"Id":         itemId,
                                      "Name":       copyStr + itemName,
                                      "Source":     container.collectionView.baseCollectionView.commands.objectViewEditorPath,
                                      "CommandsId": container.collectionView.baseCollectionView.commands.objectViewEditorCommandsId});
        }
    }
}
