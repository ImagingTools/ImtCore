import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
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
    importFileDialog.nameFilters: ["Xml files (*.xml)"];
    importDialogMimeTypes: ["application/xml"]
    exportDialogMimeTypes: ["application/xml"]
    exportFileDialog.title: qsTr("Select product");

    importObjectTypeId: "Product";

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
            let productId = container.collectionView.table.elements.getData("productId", index);
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

    function onImportDialogResult(name, fileData, mimeType){
        console.log("onImportDialogResult", name, mimeType)
        if (collectionView && collectionView.dataController){
            collectionView.dataController.importObject(importObjectTypeId, name, "", fileData, mimeType, {"force":container.force});
        }
    }

    function onRename(){
        let indexes = container.collectionView.table.getSelectedIndexes();
        if (indexes.length > 0){
            let selectedName = container.collectionView.table.elements.getData("productName", indexes[0]);
            ModalDialogManager.openDialog(renameDialogComp, {"message": qsTr("Please enter the name of the document:"), "inputValue": selectedName});
        }
    }
}
