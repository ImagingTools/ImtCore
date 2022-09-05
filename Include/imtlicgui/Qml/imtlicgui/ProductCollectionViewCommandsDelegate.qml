import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0
import imtlicgui 1.0
import QtQuick.Dialogs 1.3

CollectionViewCommandsDelegateBase {
    id: container;

    onSelectedIndexChanged: {
        console.log("ProductCollectionViewCommands onSelectedIndexChanged");

        let mode = container.selectedIndex > -1 ? "Normal" : "Disabled";
        commandsProvider.changeCommandMode("Duplicate", mode);
    }

    onCommandActivated: {
        if (commandId === "Duplicate"){
            let itemId = tableData.getSelectedId();
            let itemName = tableData.getSelectedName();

            let copyStr = qsTr("Copy of ");
            documentManager.addDocument({"Id":         itemId,
                                      "Name":       copyStr + itemName,
                                      "Source":     baseCollectionView.commands.objectViewEditorPath,
                                      "CommandsId": baseCollectionView.commands.objectViewEditorCommandsId});
        }
        else if (commandId === "Export"){
            fileDialogSave.open();
        }
    }

    RemoteFileController {
        id: remoteFileController;
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

            let id = tableData.getSelectedId();

            if (fileName == ""){
                fileName = {};
                fileName["name"] = id + ".xml";
            }
            remoteFileController.GetFile(id, fileName);
        }
    }
}
