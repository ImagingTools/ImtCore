import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import QtQuick.Dialogs 1.3

CollectionViewCommandsDelegateBase {
    id: container;

//    onCommandsIdChanged: {
//        Events.subscribeEvent(container.commandsId + "CommandActivated", container.commandHandle);
//    }

    onSelectedIndexChanged: {
        console.log("InstallationCollectionViewContainer onSelectedIndexChanged");

        let mode = container.selectedIndex > -1 ? "Normal" : "Disabled";
        container.commandsProvider.changeCommandMode("Duplicate", mode);
        container.commandsProvider.changeCommandMode("CreateLicense", mode);
    }

    onCommandActivated: {
        console.log("InstallationCollectionViewContainer commandActivated", commandId);
        if (commandId === "Duplicate"){
            let itemId = container.collectionView.baseCollectionView.table.getSelectedId();
            let itemName = container.collectionView.baseCollectionView.table.getSelectedName();
            container.collectionView.rootItem.addDocument({"Id": itemId, "Name": itemName, "Source": container.collectionView.editorPath, "CommandsId": container.collectionView.editorCommandsId});
        }
        else if (commandId === "CreateLicense"){
            fileDialogSave.open();
        }
    }

//    function commandHandle(commandId){
//        console.log("InstallationCollectionViewContainer commandActivated", commandId);

//        if (commandId === "Duplicate"){
//            let itemId = container.collectionView.baseCollectionView.table.getSelectedId();
//            let itemName = container.collectionView.baseCollectionView.table.getSelectedName();
//            container.collectionView.rootItem.addDocument({"Id": itemId, "Name": itemName, "Source": container.collectionView.editorPath, "CommandsId": container.collectionView.editorCommandsId});
//        }
//        else if (commandId === "CreateLicense"){
//            fileDialogSave.open();
//        }
//        else{
//            container.commandHandleBase(commandId);
//        }
//    }

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

