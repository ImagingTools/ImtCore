import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0
import imtlicgui 1.0
import QtQuick.Dialogs 1.3

CollectionViewCommandsDelegateBase {
    id: container;

    removeDialogTitle: qsTr("Deleting a product");
    removeMessage: qsTr("Delete the selected product ?");

//    onSelectedIndexChanged: {
//        console.log("ProductCollectionViewCommands onSelectedIndexChanged", container.selectedIndex);

//        let isEnabled = container.selectedIndex > -1;
//        if (container.commandsProvider){
//            commandsProvider.setCommandIsEnabled("Duplicate", isEnabled);
//            commandsProvider.setCommandIsEnabled("Export", isEnabled);
//        }
//    }

    onSelectionChanged: {
        let indexes = container.tableData.getSelectedIndexes();
        let isEnabled = indexes.length === 1;
        if(container.commandsProvider){
//            commandsProvider.setCommandIsEnabled("Duplicate", isEnabled);
            commandsProvider.setCommandIsEnabled("Export", isEnabled);
        }
    }

    onCommandActivated: {
        if (commandId === "Duplicate"){
            let itemId = container.tableData.getSelectedId();
            let itemName = container.tableData.getSelectedName();

            let copyStr = qsTr("Copy of ");
            container.documentManager.addDocument({"Id":         itemId,
                                      "Name":       copyStr + itemName,
                                      "Source":     container.collectionViewBase.baseCollectionView.commands.objectViewEditorPath,
                                      "CommandsId": container.collectionViewBase.baseCollectionView.commands.objectViewEditorCommandsId});
        }
        else if (commandId === "Export"){
            fileDialogSave.open();
        }
    }

    RemoteFileController {
        id: remoteFileController;

        prefix: "/files";
    }

    FileDialog {
        id: fileDialogSave;

        title: qsTr("Save file");
        selectExisting: false;
        folder: shortcuts.home;

        nameFilters: ["Features files (*.xml)", "All files (*)"];

        onAccepted: {
            var pathDir = fileDialogSave.folder.toString();
            remoteFileController.downloadedFileLocation = pathDir.replace('file:///', '');
            var fileName = fileDialogSave.fileUrl.toString().replace(pathDir + "/", '');

            let id = container.tableData.getSelectedId();
            if (fileName == ""){
                fileName = {};
                fileName["name"] = id + ".xml";
            }
            remoteFileController.GetFile(id, fileName);
        }
    }
}
