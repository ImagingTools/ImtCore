import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import Qt.labs.platform 1.1
import imtcolgui 1.0
import imtdocgui 1.0
import imtcontrols 1.0
import imtlicProductsSdl 1.0
import imtguigql 1.0

DocumentCollectionViewDelegate {
	id: container;
	revisionsPermissionPath: "/ProductManagement/Revisions";
	
	removeDialogTitle: qsTr("Deleting a product");
	removeMessage: qsTr("Delete the selected product ?");
	
	exportFileDialog.nameFilters: ["Xml files (*.xml)", "C++ Header Files (*.h)"];
	importFileDialog.nameFilters: ["Xml files (*.xml)", "C++ Header Files (*.h)"];
	importDialogMimeTypes: ["application/xml", "text/x-c"]
	exportDialogMimeTypes: ["application/xml", "text/x-c"]
	exportFileDialog.title: qsTr("Export the product to a file");
	importFileDialog.title: qsTr("Choose a product file to import");

	importObjectTypeId: "Product";
	canSetDescription: true
	canRename: true

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
		if (selectedIndexes.length > 0){
			let index = selectedIndexes[0]
			let productId = container.collectionView.table.elements.getData("productId", index);
			let fileName = productId + "Features"
			
			exportFileDialog.currentFile = fileName + ".xml";
		}
	}
	
	Component {
		id: messageErrorDialog;
		
		ErrorDialog {}
	}
	
	// The choice is made before the file dialog and sent with the import (replaceExisting).
	// One string literal, not a concatenation: lupdate cannot extract qsTr() when
	// its argument is built with "+".
	property bool replaceExistingFeatures: false;

	Component {
		id: importWarningComp;

		MessageDialog {
			title: qsTr("Import a product");
			message: qsTr("The product from the file (XML or the C++ header Lisa exports) is added to the collection; a product whose id already exists is not imported. Features the file carries that are not in the system yet are created.\n\nOverwrite the features that already exist with the versions from the file? Other products and licenses that use those features get the new version as well.\n\nYes - overwrite them, No - keep the existing features and ignore their versions in the file.");

			Component.onCompleted: {
				addButton(Enums.cancel, qsTr("Cancel"), true)
			}

			onFinished: {
				if (buttonId !== Enums.yes && buttonId !== Enums.no){
					return;
				}

				container.replaceExistingFeatures = buttonId === Enums.yes;
				container.importFileDialog.open();
			}
		}
	}

	function onImport(){
		ModalDialogManager.openDialog(importWarningComp, {});
	}

	function onImportDialogResult(name, fileData, mimeType){
		if (container.collectionView && container.collectionView.dataController){
			container.collectionView.dataController.importObject(importObjectTypeId, name, "", fileData, mimeType, container.replaceExistingFeatures);
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
