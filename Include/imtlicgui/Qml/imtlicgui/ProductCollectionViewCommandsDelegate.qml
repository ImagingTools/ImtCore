import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import Qt.labs.platform 1.1

CollectionViewCommandsDelegateBase {
    id: container;

    removeDialogTitle: qsTr("Deleting a product");
    removeMessage: qsTr("Delete the selected product ?");

    onSelectionChanged: {
        let indexes = container.tableData.getSelectedIndexes();
        let isEnabled = indexes.length === 1;
        if(container.commandsProvider){
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
            console.log("Export");

            let itemIds = container.tableData.getSelectedIds();

            console.log("id", itemIds[0]);

            fileDialogSave.currentFile = 'file:///' + itemIds[0] + ".xml";

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

        nameFilters: ["Features files (*.xml)", "All files (*)"];
        fileMode: FileDialog.SaveFile;

        onAccepted: {
            var pathDir = fileDialogSave.folder.toString();
            remoteFileController.downloadedFileLocation = pathDir.replace('file:///', '');
            var fileName = fileDialogSave.file.toString().replace(pathDir + "/", '');

            let itemIds = container.tableData.getSelectedIds();
            if (fileName == ""){
                fileName = {};
                fileName["name"] = itemIds[0] + ".xml";
            }
            remoteFileController.GetFile(itemIds[0], fileName);
        }
    }
}
