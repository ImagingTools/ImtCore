import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import Qt.labs.platform 1.1

CollectionViewCommandsDelegateBase {
    id: container;

    removeDialogTitle: qsTr("Deleting a product");
    removeMessage: qsTr("Delete the selected product ?");

    descriptionFieldId: "ProductDescription";

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
            let indexes = container.tableData.getSelectedIndexes();
            if (indexes.length > 0){
                let index = indexes[0];
                let productId = container.tableData.elements.GetData("ProductId", index);
                let fileName = productId + "Features"

                fileDialogSave.currentFile = 'file:///' + fileName + ".xml";
                fileDialogSave.open();
            }
        }
    }

    function onRename(){
        let indexes = tableData.getSelectedIndexes();
        if (indexes.length > 0){
            let selectedName = tableData.elements.GetData("ProductName", indexes[0]);
            modalDialogManager.openDialog(renameDialog, {"message": qsTr("Please enter the name of the document:"), "inputValue": selectedName});
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

            let indexes = container.tableData.getSelectedIndexes();
            if (indexes.length > 0){
                let index = indexes[0];
                let selectedProductId = container.tableData.elements.GetData("Id", index);
                if (selectedProductId !== ""){
                    if (fileName == ""){
                        fileName = {};
                        fileName["name"] = selectedProductId + ".xml";
                    }

                    remoteFileController.GetFile(selectedProductId, fileName);
                }
            }
        }
    }
}
