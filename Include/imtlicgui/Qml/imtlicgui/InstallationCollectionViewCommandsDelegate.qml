import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import QtQuick.Dialogs 1.3

CollectionViewCommandsDelegateBase {
    id: container;

    onSelectedIndexChanged: {
        console.log("InstallationCollectionViewContainer onSelectedIndexChanged");

        let mode = container.selectedIndex > -1 ? "Normal" : "Disabled";
        commandsProvider.changeCommandMode("Duplicate", mode);
        commandsProvider.changeCommandMode("CreateLicense", mode);
    }

    onCommandActivated: {
        console.log("InstallationCollectionViewContainer commandActivated", commandId);
        if (commandId === "Duplicate"){
            let itemId = baseCollectionView.table.getSelectedId();
            let itemName = baseCollectionView.table.getSelectedName();

            let copyStr = qsTr("Copy of ");

            multiDocView.addDocument({"Id":         itemId,
                                      "Name":       copyStr + itemName,
                                      "Source":     baseCollectionView.commands.objectViewEditorPath,
                                      "CommandsId": baseCollectionView.commands.objectViewEditorCommandsId});

        }
        else if (commandId === "CreateLicense"){
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

        nameFilters: ["License files (*.lic)", "All files (*)"];

        onAccepted: {
            var pathDir = fileDialogSave.folder.toString();
            remoteFileController.downloadedFileLocation = pathDir.replace('file:///', '');
            var fileName = fileDialogSave.fileUrl.toString().replace(pathDir + "/", '');

            let id = tableData.getSelectedId();

            if (fileName == ""){
                fileName = {};
                fileName["name"] = id + ".lic";
            }
            remoteFileController.GetFile(id, fileName);
        }
    }
}

