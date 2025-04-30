import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtdocgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0 as ImtControls
import imtcontrols 1.0
import Qt.labs.platform 1.1

ViewCommandsDelegateBase {
	id: collectionViewCommandsDelegate;

	property Item collectionView: null;

	property string descriptionFieldId: "Description";
	property string collectionId: collectionView != null ? collectionView.collectionId : "";

	property string removeDialogTitle: qsTr("Deleting a selected element");
	property string removeMessage: qsTr("Remove selected item from the collection ?");

	property TreeItemModel contextMenuModel: TreeItemModel {}

	property bool loadingInParts: false;

	property alias renameDialogComp: renameDialog;
	property alias removeDialogComp: removeDialog;
	property alias importFileDialog: importFileDialog_;
	property alias exportFileDialog: fileDialogSave;
	property alias setDescriptionDialogComp: setDescriptionDialog;

	property string importObjectTypeId;

	property bool canRename: false;
	property bool canSetDescription: false;

	property var importDialogMimeTypes: []
	property var exportDialogMimeTypes: []
	property var exportContextMenuItems: []

	signal renamed(string id, string newName);
	signal descriptionSetted(string id, string description);
	signal removed(string id);
	signal selectionChanged(var selection)
	signal beginImport(string fileName, string filePath, int fileSize);

	Component.onDestruction: {
		if (collectionViewCommandsDelegate.collectionView){
			Events.unSubscribeAllFromSlot(collectionViewCommandsDelegate.commandHandle);
		}
	}

	onCollectionViewChanged: {
		if (collectionView){
			commandsControllerConn.target = collectionView.commandsController
			collectionConnections.target = collectionView;
			elementsConnections.target = collectionView.table;

			if (collectionView.dataController){
				controllerConnections.target = collectionView.dataController;
			}
		}
	}

	ListModel {
		id: extensionModel;
	}

	Connections {
		id: commandsControllerConn;

		function onCommandsChanged(){
			collectionViewCommandsDelegate.setupContextMenu();
		}
	}

	Connections {
		id: collectionConnections;

		function onSelectionChanged(selection){
			collectionViewCommandsDelegate.updateBaseCommandsAccent();
			collectionViewCommandsDelegate.updateItemSelection(selection);

			collectionViewCommandsDelegate.selectionChanged(selection);
		}

		function onDataControllerChanged(){
			controllerConnections.target = collectionViewCommandsDelegate.collectionView.dataController;
		}
	}

	Connections {
		id: elementsConnections;

		function onElementsChanged(){
			let indexes = collectionViewCommandsDelegate.collectionView.table.getSelectedIndexes();
			collectionViewCommandsDelegate.updateItemSelection(indexes);
		}
	}

	Connections {
		id: controllerConnections;

		function onImported(){
			ModalDialogManager.showInfoDialog(qsTr("The object has been successfully imported"));
		}

		function onExported(name, data){
			if (Qt.platform.os == "web"){
				exportFileIO.source = name;
			}

			let encodedStr = Qt.atob(data);
			exportFileIO.write(encodedStr);
		}
	}

	function setupExtensionsModel(){
		extensionModel.clear();

		for (let i = 0; i < fileDialogSave.mimeTypes.length; i++){
			let mimeType = fileDialogSave.mimeTypes[i];
			let name = "";

			if (i < exportContextMenuItems.length){
				name = exportContextMenuItems[i];
			}

			if (name === ""){
				name = mimeType;
			}

			extensionModel.append({"id": mimeType, "name": name})
		}
	}

	function getContextMenuModel(){
		return collectionViewCommandsDelegate.contextMenuModel;
	}

	function updateBaseCommandsAccent(){
		let indexes = collectionViewCommandsDelegate.collectionView.table.getSelectedIndexes();
		let isEnabled = indexes.length > 0;

		let commandsView = collectionViewCommandsDelegate.collectionView.commandsView;
		if (commandsView){
			if (commandsView.setPositiveAccentCommandIds !== undefined){
				commandsView.setPositiveAccentCommandIds(["New"]);
			}

			if (isEnabled){
				if (commandsView.setNegativeAccentCommandIds !== undefined){
					commandsView.setNegativeAccentCommandIds(["Remove"]);
				}

				if (commandsView.setPositiveAccentCommandIds !== undefined){
					commandsView.setPositiveAccentCommandIds(["Edit"]);
				}
			}
		}

		collectionViewCommandsDelegate.updateCustomCommandsAccent();
	}

	function updateCustomCommandsAccent(){}

	function updateItemSelection(selectedItems){
		if (collectionViewCommandsDelegate.collectionView && collectionViewCommandsDelegate.collectionView.commandsController){
			let commandsController = collectionViewCommandsDelegate.collectionView.commandsController;
			let elementsModel = collectionView.table.elements;

			updateStateBaseCommands(selectedItems, commandsController, elementsModel);
			updateStateCustomCommands(selectedItems, commandsController, elementsModel);
		}
	}

	function updateStateBaseCommands(selection, commandsController, elementsModel){
		let isEnabled = selection.length > 0;
		if(commandsController){
			commandsController.setCommandIsEnabled("Remove", isEnabled);
			commandsController.setCommandIsEnabled("Edit", isEnabled);
			commandsController.setCommandIsEnabled("Export", selection.length === 1);
			commandsController.setCommandIsEnabled("Revision", selection.length === 1);
		}
	}

	function updateStateCustomCommands(selection, commandsController, elementsModel){
	}

	function setupContextMenu(){
		let commandsController = collectionView.commandsController;
		if (commandsController){
			collectionViewCommandsDelegate.contextMenuModel.clear();

			let canEdit = commandsController.commandExists("Edit");
			let canRemove = commandsController.commandExists("Remove");

			if (canEdit){
				let index = collectionViewCommandsDelegate.contextMenuModel.insertNewItem();

				collectionViewCommandsDelegate.contextMenuModel.setData("id", "Edit", index);
				collectionViewCommandsDelegate.contextMenuModel.setData("name", qsTr("Edit"), index);
				collectionViewCommandsDelegate.contextMenuModel.setData("icon", "Icons/Edit", index);
			}

			if (canRemove){
				let index = collectionViewCommandsDelegate.contextMenuModel.insertNewItem();

				collectionViewCommandsDelegate.contextMenuModel.setData("id", "Remove", index);
				collectionViewCommandsDelegate.contextMenuModel.setData("name", qsTr("Remove"), index);
				collectionViewCommandsDelegate.contextMenuModel.setData("icon", "Icons/Delete", index);
			}

			if (canEdit){
				if (collectionViewCommandsDelegate.canRename){
					let index = collectionViewCommandsDelegate.contextMenuModel.insertNewItem();

					collectionViewCommandsDelegate.contextMenuModel.setData("id", "Rename", index);
					collectionViewCommandsDelegate.contextMenuModel.setData("name", qsTr("Rename"), index);
					collectionViewCommandsDelegate.contextMenuModel.setData("icon", "", index);
				}

				if (collectionViewCommandsDelegate.canSetDescription){
					let index = collectionViewCommandsDelegate.contextMenuModel.insertNewItem();

					collectionViewCommandsDelegate.contextMenuModel.setData("id", "SetDescription", index);
					collectionViewCommandsDelegate.contextMenuModel.setData("name", qsTr("Set Description"), index);
					collectionViewCommandsDelegate.contextMenuModel.setData("icon", "", index);
				}
			}

			collectionViewCommandsDelegate.contextMenuModel.refresh();
		}
	}

	Component {
		id: extensionsPopupMenuDialog;

		PopupMenuDialog {
			itemWidth: 350;
			// centered: true;
			// hiddenBackground: false;
			onFinished: {
				if (index >= 0){
					fileDialogSave.fileExt = collectionViewCommandsDelegate.getExtensionFromNameFilter(fileDialogSave.nameFilters, index);
					fileDialogSave.open();
				}
			}
		}
	}

	// for reimplemented
	function onEdit(){
	}

	function onImport(){
		importFileDialog_.open();
	}

	function onExport(){
		if (Qt.platform.os == "web"){
			setupExtensionsModel();

			if (extensionModel.count === 1){
				fileDialogSave.fileExt = collectionViewCommandsDelegate.getExtensionFromNameFilter(fileDialogSave.nameFilters, 0);

				fileDialogSave.open();
			}
			else{
				ModalDialogManager.openDialog(extensionsPopupMenuDialog, {"x": ModalDialogManager.activeView.width / 2, "y": ModalDialogManager.activeView.height / 2, "model": extensionModel});
			}
		}
		else{
			fileDialogSave.open();
		}
	}

	function onRevision(){
	}

	// importObject(typeId, name, description, b64encoded, ext, additionalParamsObj) - signature in dataController
	function onImportDialogResult(name, fileData, mimeType){
		if (collectionView && collectionView.dataController){
			collectionView.dataController.importObject(importObjectTypeId, name, "", fileData, mimeType);
		}
	}

	// exportObject(objectId, extension, additionalParamsObj) - signature in dataController
	function onExportDialogResult(objectId, mimeType){
		if (collectionView && collectionView.dataController){
			collectionView.dataController.exportObject(objectId, mimeType);
		}
	}

	function getDialogNameFilterIndex(nameFilters, fileExt){
		for (let i = 0; i < nameFilters.length; i++){
			let nameFilter = nameFilters[i];
			let filterExt = nameFilter.split("*.")[1].split(")")[0];
			if (filterExt === fileExt){
				return i;
			}
		}

		return -1;
	}

	function getExtensionFromNameFilter(nameFilters, index){
		let nameFilter = nameFilters[index];

		let filterExt = nameFilter.split("*.")[1].split(")")[0];
		return filterExt;
	}

	FileDialog {
		id: importFileDialog_
		title: qsTr("Import file")
		fileMode: FileDialog.OpenFile
		nameFilters: ["All files (*)"]

		property var mimeTypes: collectionViewCommandsDelegate.importDialogMimeTypes;

		onAccepted: {
			let filePath;
			let fileSize = 0;
			if (Qt.platform.os == "web"){
				filePath = importFileDialog_.file.toString()
				fileSize = filePath.size;
			}
			else {
				filePath = importFileDialog_.file.toString()
			}

			filePath = filePath.replace('file:///', '')

			let mimeType = ""
			if (Qt.platform.os == "web"){
				let fileName = filePath.name
				let fileNameWithExt = fileName;
				let data = filePath.name.split('.')
				let ext = "";
				if (data.length > 0){
					ext = data[data.length -1]
					fileName = data[0]
					for(let i = 1; i < (data.length - 1); i++){
						fileName = "." + data[i]
					}
				}
				if (!collectionViewCommandsDelegate.loadingInParts){
					let reader = new FileReader()

					reader.readAsDataURL(filePath)

					reader.onload = function(){
						let encodedContentWithHeader = reader.result
						let encodedContent = encodedContentWithHeader.replace(/^.{0,}base64,/, '')

						let index = collectionViewCommandsDelegate.getDialogNameFilterIndex(importFileDialog_.nameFilters, ext)
						if (index < 0){
							console.error("Extension not found in name filters");
							return;
						}

						mimeType = mimeTypes[index]

						collectionViewCommandsDelegate.onImportDialogResult(fileName, encodedContent, mimeType);
					}.bind(this)

				}
				else {
					collectionViewCommandsDelegate.beginImport(fileNameWithExt, filePath, fileSize);
				}
			}//web
			else {
				let fileName = filePath
				let fileNameWithExt = fileName;
				let parts = filePath.split('/')
				if (parts.length > 0){
					fileNameWithExt = parts[parts.length - 1];
					let data = parts[parts.length - 1].split('.')
					let ext = "";
					if (data.length > 0){
						ext = data[data.length - 1]
						fileName = data[0]
						for(let i = 1; i < (data.length - 1); i++){
							fileName = "." + data[i]
						}
					}
				}
				if(!collectionViewCommandsDelegate.loadingInParts){
					fileIO.source = filePath
					let fileData = fileIO.read()
					fileSize = fileData.length;
					let encodedData = Qt.btoa(fileData);
					let index = collectionViewCommandsDelegate.getDialogNameFilterIndex(importFileDialog_.nameFilters, ext)
					if (index < 0){
						console.error("Extension not found in namefilters");
						return;
					}
					mimeType = mimeTypes[index]
					collectionViewCommandsDelegate.onImportDialogResult(fileName, encodedData, mimeType);
				}
				else {
					//fileSize =
					//collectionViewCommandsDelegate.beginImport(fileNameWithExt, filePath, fileSize)
				}
			}
		}//onAccepted

		FileIO {
			id: fileIO
		}
	}

	function getFormatFromMimeType(mimeType) {
		return mimeType.split('/').pop();
	}

	FileDialog {
		id: fileDialogSave;
		title: qsTr("Save file");
		nameFilters: ["All files (*)"];
		fileMode: FileDialog.SaveFile;
		currentFile: "Default";
		property string fileExt;
		property var mimeTypes: collectionViewCommandsDelegate.exportDialogMimeTypes;

		onAccepted: {
			let filePath = fileDialogSave.file.toString();

			let mimeType = ""
			if (Qt.platform.os == "web"){
				exportFileIO.source = fileDialogSave.currentFile;

				let index = collectionViewCommandsDelegate.getDialogNameFilterIndex(fileDialogSave.nameFilters, fileDialogSave.fileExt)
				if (index < 0){
					console.error("Extension not found in name filters");
					return;
				}

				mimeType = mimeTypes[index]
			}
			else{
				let extension = filePath.substring(filePath.lastIndexOf('.') + 1);
				fileDialogSave.fileExt = extension;

				filePath = filePath.replace('file:///', '')
				exportFileIO.source = filePath;

				let index = collectionViewCommandsDelegate.getDialogNameFilterIndex(fileDialogSave.nameFilters, fileDialogSave.fileExt)
				if (index < 0){
					console.error("Extension not found in name filters");
					return;
				}

				mimeType = mimeTypes[index]
			}

			let indexes = collectionViewCommandsDelegate.collectionView.table.getSelectedIndexes();
			if (indexes.length > 0){
				let index = indexes[0];
				let selectedId = collectionViewCommandsDelegate.collectionView.table.elements.getData("id", index);
				if (selectedId !== ""){
					collectionViewCommandsDelegate.onExportDialogResult(selectedId, mimeType);
				}
			}
		}

		FileIO {
			id: exportFileIO
		}
	}

	function onRemove(){
		ModalDialogManager.openDialog(removeDialog, {});
	}

	function onNew(){}

	function onRename(){
		let indexes = collectionViewCommandsDelegate.collectionView.table.getSelectedIndexes();
		if (indexes.length > 0){
			let selectedName = collectionViewCommandsDelegate.collectionView.table.elements.getData("name", indexes[0]);
			ModalDialogManager.openDialog(renameDialog, {"message": qsTr("Please enter the name of the document:"), "inputValue": selectedName});
		}
	}

	function onSetDescription(){
		let elements = collectionViewCommandsDelegate.collectionView.table.elements;

		let indexes = collectionViewCommandsDelegate.collectionView.table.getSelectedIndexes();
		if (indexes.length > 0){
			let selectedDescription = "";

			if (elements.containsKey(collectionViewCommandsDelegate.descriptionFieldId, indexes[0])){
				selectedDescription = elements.getData(collectionViewCommandsDelegate.descriptionFieldId, indexes[0]);
			}

			ModalDialogManager.openDialog(setDescriptionDialog, {"message": qsTr("Please enter the description of the document:"), "inputValue": selectedDescription});
		}
	}

	function commandHandle(commandId){
		console.log("CollectionViewCommandsDelegateBase commandHandle", commandId);

		if (!collectionViewCommandsDelegate.collectionView){
			return;
		}

		let commandsController = collectionViewCommandsDelegate.collectionView.commandsController;
		if (!commandsController){
			return;
		}

		let commandIsEnabled = commandsController.commandIsEnabled(commandId);
		console.log("commandIsEnabled", commandIsEnabled);

		if (commandIsEnabled){
			if (commandId === "New"){
				collectionViewCommandsDelegate.onNew();
			}
			else if (commandId === "Remove"){
				collectionViewCommandsDelegate.onRemove();
			}
			else if (commandId === "Edit"){
				collectionViewCommandsDelegate.onEdit();
			}
			else if (commandId === "Import"){
				collectionViewCommandsDelegate.onImport();
			}
			else if (commandId === "Export"){
				collectionViewCommandsDelegate.onExport();
			}
			else if (commandId === "Revision"){
				collectionViewCommandsDelegate.onRevision();
			}
		}

		let editIsEnabled = commandsController.commandIsEnabled("Edit");
		if (editIsEnabled){
			if (commandId === "Rename"){
				collectionViewCommandsDelegate.onRename();
			}
			else if (commandId === "SetDescription"){
				collectionViewCommandsDelegate.onSetDescription();
			}
		}

		collectionViewCommandsDelegate.commandActivated(commandId);
	}

	Component {
		id: errorDialog;
		ImtControls.ErrorDialog {
		}
	}

	Component {
		id: setDescriptionDialog;

		ImtControls.InputDialog {
			width: 300;
			title: qsTr("Set description");
			placeHolderText: qsTr("Enter the description")
			onFinished: {
				if (buttonId == Enums.ok){
					let indexes = collectionViewCommandsDelegate.collectionView.table.getSelectedIndexes();
					if (indexes.length >= 0){
						let index = indexes[0];
						collectionViewCommandsDelegate.collectionView.setElementDescription(index, inputValue)
					}
				}

				if (collectionViewCommandsDelegate.collectionView){
					collectionViewCommandsDelegate.collectionView.table.forceActiveFocus();
				}
			}
		}
	}

	Component {
		id: renameDialog;
		ImtControls.InputDialog {
			width: 300;
			title: qsTr("Rename document");
			placeHolderText: qsTr("Enter the name")
			onFinished: {
				if (buttonId == Enums.ok){
					let indexes = collectionViewCommandsDelegate.collectionView.table.getSelectedIndexes();
					if (indexes.length >= 0){
						let index = indexes[0];
						collectionViewCommandsDelegate.collectionView.setElementName(index, inputValue)
					}
				}

				if (collectionViewCommandsDelegate.collectionView){
					collectionViewCommandsDelegate.collectionView.table.forceActiveFocus();
				}
			}
		}
	}

	Component {
		id: removeDialog;
		ImtControls.MessageDialog {
			width: 400;
			title: collectionViewCommandsDelegate.removeDialogTitle;
			message: collectionViewCommandsDelegate.removeMessage;
			onFinished: {
				if (buttonId == Enums.yes){
					let indexes = collectionViewCommandsDelegate.collectionView.table.getSelectedIndexes();

					for (let i = 0; i < indexes.length; i++){
						collectionViewCommandsDelegate.collectionView.removeElement(indexes[i])
					}
				}

				if (collectionViewCommandsDelegate.collectionView){
					collectionViewCommandsDelegate.collectionView.table.forceActiveFocus();
				}
			}
		}
	}
}

