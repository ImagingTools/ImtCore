import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import Qt.labs.platform 1.1
import imtcolgui 1.0
import imtdocgui 1.0
import imtcontrols 1.0

DocumentCollectionViewDelegate {
    id: container;

    removeDialogTitle: qsTr("Deleting a product");
    removeMessage: qsTr("Delete the selected product ?");

    exportFileDialog.nameFilters: ["Xml files (*.xml)"];
    exportFileDialog.title: qsTr("Select product");

    property bool force: false;

    function updateItemSelection(selectedItems){
        if (container.collectionView && container.collectionView.commandsController){
            let elementsModel = container.collectionView.table.elements;
            if (!elementsModel){
                return;
            }

            let isEnabled = selectedItems.length === 1;

            let commandsController = container.collectionView.commandsController;
            if(commandsController){
                commandsController.setCommandIsEnabled("Remove", isEnabled);
                commandsController.setCommandIsEnabled("Edit", isEnabled);
                commandsController.setCommandIsEnabled("Export", isEnabled);
            }
        }
    }

    onSelectionChanged: {
        if (selection.length > 0){
            let index = selection[0];
            let productId = container.collectionView.table.elements.getData("ProductId", index);
            let fileName = productId + "Features"

            exportFileDialog.currentFile = fileName + ".xml";
        }
    }

    Component {
        id: messageErrorDialog;

        ErrorDialog {}
    }

    Component {
        id: messageDialogComp;

        MessageDialog {
            title: qsTr("Feature overwriting");
            message: qsTr("Overwrite features ?");

            onFinished: {
                container.force = false;
                if (buttonId == Enums.yes){
                    container.force = true;
                }

                container.importFileDialog.open()
            }
        }
    }

    function onImport(){
        ModalDialogManager.openDialog(messageDialogComp, {});
    }

    function onImportDialogResult(name, fileData){
        if (collectionView && collectionView.dataController){
            collectionView.dataController.importObject("", name, "", fileData, {"force":container.force});
        }
    }

    function onRename(){
        let indexes = container.collectionView.table.getSelectedIndexes();
        if (indexes.length > 0){
            let selectedName = container.collectionView.table.elements.getData("ProductName", indexes[0]);
            ModalDialogManager.openDialog(renameDialogComp, {"message": qsTr("Please enter the name of the document:"), "inputValue": selectedName});
        }
    }

    RemoteFileController {
        id: remoteFileController;
        prefix: "/files/";

        onFileUploaded: {
            ModalDialogManager.openDialog(messageErrorDialog, {"title" : qsTr("Import successful"),"message": qsTr("The product has been successfully imported")});
        }

        onFileUploadFailed: {
            ModalDialogManager.openDialog(messageErrorDialog, {"message": qsTr("Error when trying to import a product")});
        }

        onFileDownloadFailed: {
            ModalDialogManager.openDialog(messageErrorDialog, {"message": qsTr("Error when trying to export a product")});
        }

        onStateChanged: {
            if (remoteFileController.state === "Loading"){
                Events.sendEvent("StartLoading");
            }
            else{
                Events.sendEvent("StopLoading");
            }
        }
    }

    // FileDialog {
    //     id: fileDialogOpen;
    //     title: qsTr("Select file");
    //     fileMode: FileDialog.OpenFile;
    //     nameFilters: ["*.xml"];

    //     onAccepted: {
    //         let filePath = fileDialogOpen.file.toString();
    //         filePath = filePath.replace('file:///', '')

    //         remoteFileController.SendFile(filePath);
    //     }
    // }

    FileDialog {
        id: fileDialogSave;

        title: qsTr("Save file");

        nameFilters: ["Features files (*.xml)", "All files (*)"];
        fileMode: FileDialog.SaveFile;

        onAccepted: {
            var pathDir = fileDialogSave.folder.toString();
            remoteFileController.downloadedFileLocation = pathDir.replace('file:///', '');
            var fileName = fileDialogSave.file.toString().replace(pathDir + "/", '');

            let indexes = container.collectionView.table.getSelectedIndexes();
            if (indexes.length > 0){
                let index = indexes[0];
                let selectedName = container.collectionView.table.elements.getData("ProductId", indexes[0]);
                if (!selectedName){
                    selectedName = "";
                }

                let selectedProductId = container.collectionView.table.elements.getData("Id", index);
                if (selectedProductId !== ""){
                    if (fileName === ""){
                        fileName = selectedName + "Features.xml";
                    }

                    remoteFileController.GetFile(selectedProductId, fileName);
                }
            }
        }
    }
}
