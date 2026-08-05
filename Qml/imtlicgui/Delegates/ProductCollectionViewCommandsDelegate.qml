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
	
	removeDialogTitle: qsTr("Deleting a product");
	removeMessage: qsTr("Delete the selected product ?");
	
	exportFileDialog.nameFilters: ["Xml files (*.xml)", "C++ Header Files (*.h)"];
	importFileDialog.nameFilters: ["Xml files (*.xml)"];
	importDialogMimeTypes: ["application/xml"]
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
	
	// Shown before the file dialog, because it warns about what importing does at
	// all rather than about one particular file. Overwriting is not a choice -
	// CProductCollectionControllerComp::OnImportObject always writes the features
	// carried in the file into the shared feature collection, creating the ones
	// that are new and replacing the ones that already exist - so the dialog says
	// so plainly instead of offering an option that does not exist.
	//
	// One string literal, not a concatenation: lupdate cannot extract qsTr() when
	// its argument is built with "+".
	Component {
		id: importWarningComp;

		MessageDialog {
			title: qsTr("Import a product");
			message: qsTr("Importing overwrites features.\n\nThe product from the file is added to the collection, and every feature it carries is written into the shared feature collection: features that are not there yet are created, and features that already exist are replaced by the version from the file. Other products and licenses that use those features get the replacement as well.\n\nChoose a file and import?");

			onFinished: {
				if (buttonId !== Enums.yes){
					return;
				}

				container.importFileDialog.open();
			}
		}
	}

	function onImport(){
		ModalDialogManager.openDialog(importWarningComp, {});
	}
	
	function onRename(){
		let indexes = container.collectionView.table.getSelectedIndexes();
		if (indexes.length > 0){
			let selectedName = container.collectionView.table.elements.getData("productName", indexes[0]);
			ModalDialogManager.openDialog(renameDialogComp, {"message": qsTr("Please enter the name of the document:"), "inputValue": selectedName});
		}
	}
}
