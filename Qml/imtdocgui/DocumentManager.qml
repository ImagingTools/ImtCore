import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

QtObject {
	id: documentManager;

	property string defaultDocumentName: qsTr("<no name>");
	property int documentsCount: documentsModel.count;
	property Item activeView;

	property ListModel documentsModel: ListModel {
		dynamicRoles: true;
	}

	/*!
		This signal is committed when the user closes the document. The corresponding handler is
		\c onDocumentClosed.
	*/
	signal documentClosed(string documentId);

	/*!
		This signal is detected when the user opens or creates a new document. The corresponding handler is
		\c onDocumentAdded.
	*/
	signal documentAdded(string documentId);

	/*!
		This signal is detected when the user saves the document. The corresponding handler is
		\c onDocumentSaved.
	*/
	signal documentSaved(string documentId);

	/*!
		This signal is detected when the document becomes modified. The corresponding handler is
		\c onDocumentIsDirtyChanged.
	*/
	signal documentIsDirtyChanged(string documentId, bool isDirty);


	function openErrorDialog(message){
		ModalDialogManager.openDialog(errorDialogComp, {"message": message});
	}


	function getActiveView()
	{
		return activeView;
	}


	function getDocumentTypeId(documentId)
	{
		for (let documentIndex = 0; documentIndex < documentsModel.count; documentIndex++){
			let id = documentsModel.get(documentIndex).Id;
			if (id === documentId){
				return documentsModel.get(documentIndex).TypeId;
			}
		}

		return String();
	}


	function registerDocumentValidator(documentTypeId, validatorComp){
		if (!documentIsRegistered(documentTypeId)){
			console.error("Unable to register validator. Document with documentTypeId: ", documentTypeId, " unregistered!");

			return false;
		}

		internal.m_registeredValidators[documentTypeId] = validatorComp;

		return true;
	}


	function registerDocumentDataController(documentTypeId, dataControllerComp){
		internal.m_registeredDataControllers[documentTypeId] = dataControllerComp;

		return true;
	}

	function unRegisterDocumentDataController(documentTypeId){
		delete internal.m_registeredDataControllers[documentTypeId]

		return true;
	}


	function registerDocumentView(documentTypeId, viewTypeId, viewComp)
	{
		if (documentTypeId === "" || viewTypeId === "" || !viewComp){
			return false;
		}

		if (documentIsRegistered(documentTypeId)){
			if (documentViewIsRegistered(viewTypeId)){
				console.error("View with type-ID: ", viewTypeId, " already registered!");

				return false;
			}

			internal.m_registeredView[documentTypeId].push({"ViewTypeId": viewTypeId, "ViewComp": viewComp});
		}
		else{
			internal.m_registeredView[documentTypeId] = [{"ViewTypeId": viewTypeId, "ViewComp": viewComp}]
		}

		return true;
	}

	function unRegisterDocumentView(documentTypeId, viewTypeId)
	{
		if (!documentIsRegistered(documentTypeId)){
			return false;
		}

		let registeredViewList = internal.m_registeredView[documentTypeId];
		for (let i = 0; i < registeredViewList.length; i++){
			let registeredViewObj = registeredViewList[i];
			if (registeredViewObj["ViewTypeId"] === viewTypeId){
				registeredViewList.splice(i,1)

				return true;
			}
		}

		return false;
	}


	function getDocumentViewComp(documentTypeId, viewTypeId)
	{
		if (!documentViewIsRegistered(documentTypeId, viewTypeId)){
			console.error("Unable to get a document view. View is unregistered!");

			return null;
		}

		let registeredViewList = internal.m_registeredView[documentTypeId];
		for (let i = 0; i < registeredViewList.length; i++){
			let registeredViewObj = registeredViewList[i];
			if (registeredViewObj["ViewTypeId"] === viewTypeId){
				return registeredViewObj["ViewComp"];
			}
		}

		return null;
	}


	function getDocumentViewCompByDocumentId(documentId)
	{
		for (let i = 0; i < documentsModel.count; i++){
			let documentData = documentsModel.get(i).DocumentData;
			if (documentData && documentData.documentId === documentId){
				return documentData.viewComp;
			}
		}
	}


	function getDocumentValidator(documentTypeId){
		if (documentTypeId in internal.m_registeredValidators){
			let validatorComp = internal.m_registeredValidators[documentTypeId];

			return validatorComp.createObject(documentManager);
		}

		return null;
	}


	function documentIsRegistered(documentTypeId)
	{
		return documentTypeId in internal.m_registeredView;
	}


	function documentViewIsRegistered(documentTypeId, viewTypeId)
	{
		if (documentIsRegistered(documentTypeId)){
			let registeredViewList = internal.m_registeredView[documentTypeId];
			for (let i = 0; i < registeredViewList.length; i++){
				let registeredViewObj = registeredViewList[i];
				if (registeredViewObj["ViewTypeId"] === viewTypeId){
					return true;
				}
			}
		}

		return false;
	}


	function dataControllerIsRegistered(documentTypeId)
	{
		return documentTypeId in internal.m_registeredDataControllers;
	}


	function getDocumentDataController(documentTypeId)
	{
		if (!dataControllerIsRegistered(documentTypeId)){
			console.warn("Data controller for documents with type-ID: ", documentTypeId, " is unregistered!");

			return defaultDataController.createObject(documentManager);
		}

		let dataControllerComp = internal.m_registeredDataControllers[documentTypeId];
		return dataControllerComp.createObject(documentManager);
	}


	function insertNewDocument(documentTypeId, viewTypeId)
	{
		let documentData = createTemplateDocument(UuidGenerator.generateUUID(), documentTypeId, viewTypeId);
		if (!documentData){
			return false;
		}

		if (documentData.documentDataController){
			documentData.documentDataController.createDocumentModel();
		}

		addDocumentToModel(documentData.documentId, documentTypeId, viewTypeId, documentData);

		return true;
	}

	function getDocumentData(index){
		if (index < 0 || index >= documentsModel.count){
			return null;
		}

		return documentsModel.get(index).DocumentData;
	}

	function getDocumentDataById(documentId){
		for (let i = 0; i < documentsModel.count; i++){
			let documentData = documentsModel.get(i).DocumentData;
			if (documentData && documentData.documentId === documentId){
				return documentData;
			}
		}

		return null;
	}

	function getViewTypeIds(documentTypeId){
		if (!documentIsRegistered(documentTypeId)){
			return []
		}

		let result = []
		let registeredViewList = internal.m_registeredView[documentTypeId];
		for (let i = 0; i < registeredViewList.length; i++){
			let registeredViewObj = registeredViewList[i];
			let viewTypeId = registeredViewObj["ViewTypeId"];
			result.push(viewTypeId)
		}

		return result;
	}


	function createTemplateDocument(documentId, documentTypeId, viewTypeId){
		let singleDocumentData = singleDocumentDataComp.createObject(documentManager);
		if (singleDocumentData){
			singleDocumentData.documentId = documentId;
			singleDocumentData.documentTypeId = documentTypeId;

			let viewComp = getDocumentViewComp(documentTypeId, viewTypeId);
			singleDocumentData.viewComp = viewComp;

			let documentDataController = getDocumentDataController(documentTypeId);
			if (documentDataController){
				documentDataController.documentId = singleDocumentData.documentId;
				singleDocumentData.documentDataController = documentDataController;
			}

			let documentValidator = getDocumentValidator(documentTypeId);
			if (documentValidator){
				singleDocumentData.documentValidator = documentValidator;
			}

			return singleDocumentData;
		}

		return null;
	}

	function openDocument(documentId, documentTypeId, viewTypeId)
	{
		console.debug("openDocument", documentId, documentTypeId, viewTypeId)

		let index = getDocumentIndexByDocumentId(documentId);
		if (index >= 0){
			// already opened
			documentAdded(documentId);

			return;
		}

		Events.sendEvent("StartLoading");

		let documentData = createTemplateDocument(documentId, documentTypeId, viewTypeId);
		if (!documentData){
			return false;
		}

		documentData.isNew = false;

		let onResult = function(){
			addDocumentToModel(documentId, documentTypeId, viewTypeId, documentData);

			documentData.documentDataController.modelChanged.disconnect(onResult);
		}

		if (documentData.documentDataController){
			documentData.documentDataController.modelChanged.connect(onResult);
			documentData.documentDataController.updateDocumentModel();
		}
		else{
			Events.sendEvent("StopLoading");
		}

		return true;
	}

	function addDocumentToModel(documentId, documentTypeId, viewTypeId, documentData)
	{
		documentsModel.append({
								"Id": documentId,
								"TypeId": documentTypeId,
								"ViewTypeId": viewTypeId,
								"DocumentData": documentData,
								"IsNew": documentData.isNew
							});

		documentAdded(documentId);
	}

	/*!
		Save document by ID.
		\param      documentId           UUID of the document
	*/
	function saveDocument(documentId)
	{
		let index = getDocumentIndexByDocumentId(documentId);
		if (index >= 0){
			let isNew = documentsModel.get(index).IsNew;
			let document = documentsModel.get(index).DocumentData;
			if (document.isDirty){
				Events.sendEvent("StartLoading");

				if (document.view){
					if (!document.view.readOnly){
						document.view.doUpdateModel();
					}
				}

				let data = {}
				let documentIsValid = documentManager.documentIsValid(document, data);
				if (!documentIsValid){
					openErrorDialog(data.message);
					Events.sendEvent("StopLoading");

					return;
				}

				if (document.documentDataController){
					if (isNew){
						document.documentDataController.insertDocument();
					}
					else{
						document.documentDataController.saveDocument();
					}
				}
			}
		}
	}


	function onDocumentSaved(documentId)
	{
		if (internal.m_closingDocuments.includes(documentId)){
			closeDocument(documentId);
		}

		let documentIndex = getDocumentIndexByDocumentId(documentId);
		if (documentIndex >= 0){
			let documentData = documentsModel.get(documentIndex);
			if (documentData && documentData.IsNew){
				documentsModel.setProperty(documentIndex, "IsNew", false);
			}
		}

		documentManager.documentSaved(documentId);
		Events.sendEvent("StopLoading");
	}


	function getDocumentIndexByDocumentId(documentId){
		for (let i = 0; i < documentsModel.count; i++){
			let documentData = documentsModel.get(i).DocumentData;
			if (documentData && documentData.documentId === documentId){
				return i;
			}
		}

		return -1;
	}


	function getDocumentDataByView(view){
		for (let i = 0; i < documentsModel.count; i++){
			let documentData = documentsModel.get(i).DocumentData;
			if (documentData && documentData.view === view){
				return documentData;
			}
		}

		return null;
	}

	// soon
	function saveDirtyDocuments(beQuiet, ignoredPtr)
	{
	}


	/*!
		Close document by document index.

		\param      documentIndex    index of the document from the model of all open documents
		\param      force            if true - document will be closed without isDirty checking
	*/
	function closeDocumentByIndex(documentIndex, force)
	{
		if (documentIndex < 0 || documentIndex >= documentsModel.count){
			console.error("Unable to close document with index: ", documentIndex);

			return;
		}

		if (!force){
			force = false;
		}

		let documentData = documentsModel.get(documentIndex).DocumentData;

		if (documentData.isDirty && !force){
			let callback = function(result){
				console.log("callback", result);

				if (result === Enums.yes){
					internal.m_closingDocuments.push(documentData.documentId);
					documentManager.saveDocument(documentData.documentId);
				}
				else if (result === Enums.no){
					documentData.isDirty = false;

					documentManager.closeDocumentByIndex(documentIndex);
				}
			}

			ModalDialogManager.openDialog(saveDialog, {}, "", callback);
		}
		else{
			let index = internal.m_closingDocuments.indexOf(documentData.documentId);
			if (index !== -1) {
				internal.m_closingDocuments.splice(index, 1);
			}

			let documentId = documentData.documentId;
			documentData.destroy();

			documentsModel.remove(documentIndex);

			documentClosed(documentId);
		}
	}


	function closeDocument(documentId, force)
	{
		let index = getDocumentIndexByDocumentId(documentId);

		closeDocumentByIndex(index, force);
	}


	function documentIsValid(documentData, data){
		if (!documentData){
			return false;
		}

		if (!data){
			data = {}
		}

		if (documentData.view){
			data["editor"] = documentData.view;
		}

		return documentData.documentValidator.isValid(data);
	}

	function getOpenedDocumentIds(){
		let result = []

		for (let i = 0; i < documentsModel.count; i++){
			let documentData = documentsModel.get(i).DocumentData;
			if (documentData && documentData.documentId){
				result.push(documentData.documentId);
			}
		}

		return result;
	}

	function setupDocumentView(documentId, view){
		let documentData = getDocumentDataById(documentId)
		if (documentData){
			if (view.documentId !== undefined){
				view.documentId = documentId;
			}

			if (view.documentTypeId !== undefined){
				view.documentTypeId = getDocumentTypeId(documentId);
			}

			if (view.viewTypeId !== undefined){
				let viewTypeIds = getViewTypeIds(view.documentTypeId);
				if (viewTypeIds.length > 0){
					view.viewTypeId = viewTypeIds[0];
				}
			}

			if (view.documentManager !== undefined){
				view.documentManager = documentManager;
			}

			documentData.view = view;

			if (view.viewRegistered !== undefined){
				view.viewRegistered();
			}

			return true;
		}

		return false;
	}

	property Component singleDocumentDataComp: Component{
		QtObject {
			id: singleDocumentData;
			// index of the document in document manager
			property bool isNew: true;
			property string documentId;
			property string documentTypeId;
			property DocumentDataController documentDataController: null;
			property DocumentValidator documentValidator: DocumentValidator {};

			property UndoRedoManager undoManager: UndoRedoManager {
				onUndo: {
					singleDocumentData.checkDocumentModel();
					singleDocumentData.updateGui();
				}

				onRedo: {
					singleDocumentData.checkDocumentModel()
					singleDocumentData.updateGui();
				}

				onModelChanged: {
					let undoSteps = getAvailableUndoSteps();
					let redoSteps = getAvailableRedoSteps();

					if (singleDocumentData.view && singleDocumentData.view.commandsController){
						singleDocumentData.view.commandsController.setCommandIsEnabled("Undo", undoSteps > 0);
						singleDocumentData.view.commandsController.setCommandIsEnabled("Redo", redoSteps > 0);
					}
				}
			};

			property Component viewComp;
			property ViewBase view;
			property bool isDirty: false;

			Component.onDestruction: {
				if (documentDataController){
					documentDataController.destroy();
				}

				if (documentValidator){
					documentValidator.destroy();
				}

				if (undoManager){
					undoManager.destroy();
				}
			}

			onViewChanged: {
				console.log("onViewChanged", view)
				let documentModel = documentDataController.documentModel;
				view.model = documentModel;
				blockingUpdateModel = true;

				if (isNew){
					updateModel();
				}
				else{
					updateGui();
				}

				if (view.commandsDelegate){
					view.commandsDelegate.commandActivated.connect(singleDocumentData.viewCommandHandle);
				}

				blockingUpdateModel = false;
			}

			function updateGui(){
				if (singleDocumentData.view){
					singleDocumentData.view.doUpdateGui();
				}
			}

			function updateModel(){
				if (singleDocumentData.view){
					singleDocumentData.view.doUpdateModel();
				}
			}

			property Connections dataControllerConnections: Connections {
				target: singleDocumentData.documentDataController;

				function onSaved(documentId){
					singleDocumentData.documentId = documentId;

					singleDocumentData.isDirty = false;

					let documentModel = singleDocumentData.documentDataController.documentModel;
					if (singleDocumentData.undoManager){
						singleDocumentData.undoManager.setStandardModel(documentModel);
					}

					documentManager.onDocumentSaved(singleDocumentData.documentId);
				}

				function onModelChanged(){
					if (!singleDocumentData.documentDataController || !singleDocumentData.documentDataController.documentModel){
						Events.sendEvent("StopLoading");

						return;
					}

					let documentModel = singleDocumentData.documentDataController.documentModel;

					singleDocumentData.documentId = singleDocumentData.documentDataController.getDocumentId();

					singleDocumentData.blockingUpdateModel = true;

					if (singleDocumentData.view){
						singleDocumentData.view.model = documentModel;

						if (singleDocumentData.view.commandsDelegate){
							singleDocumentData.view.commandsDelegate.commandActivated.connect(singleDocumentData.viewCommandHandle);
						}
					}

					if (singleDocumentData.isNew){
						singleDocumentData.updateModel();
					}
					else{
						singleDocumentData.updateGui();
					}

					singleDocumentData.blockingUpdateModel = false;

					singleDocumentData.undoManager.registerModel(documentModel);
					singleDocumentData.documentValidator.documentModel = documentModel;

					singleDocumentData.modelConnections.target = singleDocumentData.documentDataController.documentModel
					singleDocumentData.modelConnections.enabled = true;

					Events.sendEvent("StopLoading");
				}

				function onError(){
					Events.sendEvent("StopLoading");
				}
			}

			property Connections modelConnections: Connections {
				enabled: false;

				function onModelChanged(){
					if (singleDocumentData.blockingUpdateModel){
						return;
					}

					if (singleDocumentData.undoManager && singleDocumentData.undoManager.isTransaction()){
						return;
					}

					singleDocumentData.isDirty = documentManager.documentIsValid(singleDocumentData);
				}
			}

			property bool blockingUpdateModel: false;

			onBlockingUpdateModelChanged: {
				if (undoManager){
					undoManager.setBlockingUpdateModel(blockingUpdateModel);
				}
			}

			signal viewAdded(var view);
			signal viewRemoved(var view);

			onDocumentIdChanged: {
				if (documentDataController){
					documentDataController.documentId = documentId;
				}
			}

			onIsDirtyChanged: {
				console.log("onIsDirtyChanged", isDirty, view)

				if (view){
					if (view.commandsController){
						view.commandsController.setCommandIsEnabled("Save", isDirty);
					}

					if (view.commandsView){
						if (view.commandsView.setPositiveAccentCommandIds !== undefined){
							view.commandsView.setPositiveAccentCommandIds(["Save"]);
						}
					}
				}

				documentManager.documentIsDirtyChanged(documentId, isDirty);
			}

			// Processing commands that came from the view
			function viewCommandHandle(commandId){
				if (singleDocumentData.undoManager){
					singleDocumentData.undoManager.commandHandle(commandId);
				}
				singleDocumentData.commandHandle(commandId);
			}

			function commandHandle(commandId){
				if (!documentManager){
					return;
				}

				if (commandId === "Close"){
					documentManager.closeDocument(documentId);
				}
				else if (commandId === "Save"){
					documentManager.saveDocument(documentId);
				}
			}

			function checkDocumentModel(){
				let currentStateModel = undoManager.getStandardModel();
				if (currentStateModel){
					let documentModel = singleDocumentData.documentDataController.documentModel
					let isEqual = currentStateModel.isEqualWithModel(documentModel);
					isDirty = !isEqual && documentManager.documentIsValid(singleDocumentData);
				}
			}
		}
	}

	property Component defaultDataController: Component {
		DocumentDataController {}
	}

	property QtObject internal: QtObject {
		property var m_registeredView: ({});
		property var m_registeredDataControllers: ({});
		property var m_registeredValidators: ({});
		property var m_closingDocuments: [];
	}

	property Component errorDialogComp: Component {
		ErrorDialog {
		}
	}

	property Component saveDialog: Component {
		MessageDialog {
			title: qsTr("Save document");
			message: qsTr("Save all changes ?")

			Component.onCompleted: {
				buttonsModel.append({Id: Enums.cancel, Name:qsTr("Cancel"), Enabled: true})
			}
		}
	}
}
