import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

QtObject {
	id: documentManager

	property string defaultDocumentName: qsTr("<no name>")
	property int documentsCount: documentsModel.count
	property Item activeView // Document Manager view
	property string typeId // Document Manager type-ID

	property ListModel documentsModel: ListModel {
		dynamicRoles: true
	}

	/*!
		This signal is committed when the user closes the document. The corresponding handler is
		\c onDocumentClosed.
	*/
	signal documentClosed(string documentId)

	/*!
		This signal is detected when the user opens or creates a new document. The corresponding handler is
		\c onDocumentAdded.
	*/
	signal documentAdded(string documentId)

	/*!
		This signal is detected when the user saves the document. The corresponding handler is
		\c onDocumentSaved.
	*/
	signal documentSaved(string documentId)
	signal documentSavingStarted(string documentId)
	signal documentSavingFailed(string documentId, string message)
	
	/*!
		This signal is detected when the document becomes modified. The corresponding handler is
		\c onDocumentIsDirtyChanged.
	*/
	signal documentIsDirtyChanged(string documentId, bool isDirty)
	
	signal documentOpeningStarted(string documentId)
	signal documentOpened(string documentId)
	signal documentOpeningFailed(string documentId, string message)
	
	// callback(undefined) - cancel, callback(false) - close, callback(true) - save and close
	signal tryCloseDirtyDocument(string documentId, var callback)

	signal documentTypeIdRegistered(string documentTypeId)
	
	function getActiveView()
	{
		return activeView;
	}

	function getTypeId()
	{
		return typeId
	}

	function getDocumentTypeId(documentId)
	{
		for (let documentIndex = 0; documentIndex < documentsModel.count; ++documentIndex){
			let id = documentsModel.get(documentIndex).id;
			if (id === documentId){
				return documentsModel.get(documentIndex).typeId;
			}
		}

		return String();
	}

	function getSupportedDocumentTypeIds(){
		return Object.keys(internal.m_registeredView)
	}

	function getDocumentName(documentId){
		for (let documentIndex = 0; documentIndex < documentsModel.count; ++documentIndex){
			let id = documentsModel.get(documentIndex).id;
			if (id === documentId){
				return documentsModel.get(documentIndex).name;
			}
		}
		return ""
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


	function registerDocumentView(documentTypeId, viewComp)
	{
		if (documentTypeId === "" || !viewComp){
			return false;
		}

		internal.m_registeredView[documentTypeId] = viewComp

		documentTypeIdRegistered(documentTypeId)

		return true
	}

	function unRegisterDocumentTypeId(documentTypeId)
	{
		if (!documentIsRegistered(documentTypeId)){
			return false;
		}

		delete internal.m_registeredView.documentTypeId

		return true
	}


	function getDocumentViewComp(documentTypeId)
	{
		if (!documentIsRegistered(documentTypeId)){
			return null;
		}

		return internal.m_registeredView[documentTypeId]
	}


	function getDocumentViewCompByDocumentId(documentId)
	{
		for (let i = 0; i < documentsModel.count; i++){
			let documentData = documentsModel.get(i).documentData;
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


	function insertNewDocument(documentTypeId, name)
	{
		let documentId = UuidGenerator.generateUUID()
		let documentData = createTemplateDocument(documentId, documentTypeId);
		if (!documentData){
			return false;
		}

		addDocumentToModel(documentId, name, documentTypeId, documentData);

		if (documentData.documentDataController){
			documentData.documentDataController.createDocumentModel();
		}

		return true;
	}

	function getDocumentData(index){
		if (index < 0 || index >= documentsModel.count){
			return null;
		}

		return documentsModel.get(index).documentData;
	}

	function getDocumentDataById(documentId){
		for (let i = 0; i < documentsModel.count; i++){
			let documentData = documentsModel.get(i).documentData;
			if (documentData && documentData.documentId === documentId){
				return documentData;
			}
		}

		return null;
	}

	function createTemplateDocument(documentId, documentTypeId){
		let singleDocumentData = singleDocumentDataComp.createObject(documentManager);
		if (singleDocumentData){
			singleDocumentData.documentId = documentId;
			singleDocumentData.documentTypeId = documentTypeId;

			let viewComp = getDocumentViewComp(documentTypeId);
			if (!viewComp){
				console.error("Unable to create view for document with type-ID '"+documentTypeId+"'")
				return null
			}

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

	function openDocument(documentId, documentTypeId, name)
	{
		console.debug("DocumentManager.qml openDocument", documentId, documentTypeId, name)
		let index = getDocumentIndexByDocumentId(documentId);
		if (index >= 0){
			// already opened
			documentAdded(documentId);

			return;
		}

		documentOpeningStarted(documentId, documentTypeId)

		let documentData = createTemplateDocument(documentId, documentTypeId);
		if (!documentData){
			return false;
		}

		documentData.isNew = false;

		let onResult = function(){
			addDocumentToModel(documentId, name, documentTypeId, documentData);

			documentData.documentDataController.modelChanged.disconnect(onResult);
		}

		if (documentData.documentDataController){
			documentData.documentDataController.modelChanged.connect(onResult);
			documentData.documentDataController.updateDocumentModel();
		}
		else{
			documentOpeningFailed(documentId, qsTr("Unable to get a model for document. Error: Document data controller is invalid"))
		}

		return true;
	}

	function addDocumentToModel(documentId, name, documentTypeId, documentData)
	{
		documentsModel.append({
								"id": documentId,
								"name": name,
								"typeId": documentTypeId,
								"documentData": documentData,
								"isNew": documentData.isNew
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
			let isNew = documentsModel.get(index).isNew;
			let document = documentsModel.get(index).documentData;
			if (document.isDirty){
				documentSavingStarted(documentId)

				if (document.view){
					if (!document.view.readOnly){
						document.view.doUpdateModel();
					}
				}

				let data = {}
				let documentIsValid = documentManager.documentIsValid(document, data);
				if (!documentIsValid){
					documentSavingFailed(documentId, data.message)

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
			let documentData = documentsModel.get(documentIndex).documentData;
			if (documentData && documentData.isNew){
				documentData.isNew = false
				documentsModel.setProperty(documentIndex, "isNew", false);
			}
		}

		documentManager.documentSaved(documentId);
	}


	function getDocumentIndexByDocumentId(documentId){
		for (let i = 0; i < documentsModel.count; i++){
			let documentData = documentsModel.get(i).documentData;
			if (documentData && documentData.documentId === documentId){
				return i;
			}
		}

		return -1;
	}


	function getDocumentDataByView(view){
		for (let i = 0; i < documentsModel.count; i++){
			let documentData = documentsModel.get(i).documentData;
			if (documentData && documentData.view === view){
				return documentData;
			}
		}

		return null;
	}
	
	function documentIsNew(documentId){
		let documentIndex = getDocumentIndexByDocumentId(documentId);
		if (documentIndex >= 0){
			let documentData = documentsModel.get(documentIndex).documentData;
			if (documentData){
				return documentData.isNew
			}
		}
		
		return false
	}
	
	function documentIsDirty(documentId){
		let documentIndex = getDocumentIndexByDocumentId(documentId);
		if (documentIndex >= 0){
			let documentData = documentsModel.get(documentIndex).documentData;
			if (documentData){
				return documentData.isDirty
			}
		}
		
		return false
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

		let documentData = documentsModel.get(documentIndex).documentData;

		if (documentData.isDirty && !force){
			let callback = function(result){
				if (result === true){
					internal.m_closingDocuments.push(documentData.documentId);
					documentManager.saveDocument(documentData.documentId);
				}
				else if (result === false){
					documentData.isDirty = false;

					documentManager.closeDocumentByIndex(documentIndex);
				}
			}
			
			tryCloseDirtyDocument(documentData.documentId, callback)

			// ModalDialogManager.openDialog(saveDialog, {}, "", callback);
		}
		else{
			let index = internal.m_closingDocuments.indexOf(documentData.documentId);
			if (index !== -1){
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

	function closeAllDocuments(){
		let openedDocumentIds = getOpenedDocumentIds()
		for (let i = 0; i < openedDocumentIds.length; ++i){
			closeDocument(openedDocumentIds[i], true)
		}
	}

	function getOpenedDocumentIds(){
		let result = []

		for (let i = 0; i < documentsModel.count; i++){
			let documentData = documentsModel.get(i).documentData;
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
	
	function setBlockUndoManager(documentId, isBlock){
		let documentIndex = getDocumentIndexByDocumentId(documentId);
		if (documentIndex >= 0){
			let documentData = documentsModel.get(documentIndex).documentData;
			if (documentData.undoManager){
				documentData.undoManager.setBlockingUpdateModel(isBlock)
			}
		}
	}
	
	function clearUndoManager(documentId){
		let documentIndex = getDocumentIndexByDocumentId(documentId);
		if (documentIndex >= 0){
			let documentData = documentsModel.get(documentIndex).documentData;
			if (documentData.undoManager){
				documentData.undoManager.resetUndo()
				return true
			}
		}
		
		return false
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
			property bool modelReceived: false;
			property bool modelInitialized: false;
			property bool viewRegistered: false;

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
				viewRegistered = view != undefined && view != null
				
				if (viewRegistered && view.commandsDelegate){
					view.commandsDelegate.commandActivated.connect(singleDocumentData.viewCommandHandle);
				}

				if (modelReceived && !modelInitialized){
					initModelForView()
				}
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

			function initModelForView(){
				if (!singleDocumentData.view){
					console.error("Unable to init model for view. Error: View is invalid")
					return;
				}

				if (!singleDocumentData.documentDataController || !singleDocumentData.documentDataController.documentModel){
					documentManager.documentOpeningFailed(singleDocumentData.documentId, qsTr("Internal error"))
					return;
				}
				
				let documentModel = singleDocumentData.documentDataController.documentModel;

				singleDocumentData.documentId = singleDocumentData.documentDataController.getDocumentId();

				singleDocumentData.blockingUpdateModel = true;

				singleDocumentData.view.model = documentModel;

				if (singleDocumentData.isNew){
					singleDocumentData.updateModel();
				}
				else{
					singleDocumentData.updateGui();
				}

				singleDocumentData.blockingUpdateModel = false;

				singleDocumentData.undoManager.registerModel(documentModel);
				singleDocumentData.documentValidator.documentModel = documentModel;

				singleDocumentData.modelInitialized = true
				singleDocumentData.modelConnections.target = singleDocumentData.documentDataController.documentModel
				singleDocumentData.modelConnections.enabled = true;
				
				documentManager.documentOpened(singleDocumentData.documentId)
			}

			property Connections dataControllerConnections: Connections {
				target: singleDocumentData.documentDataController;

				function onSaved(documentId){
					singleDocumentData.isDirty = false;

					let documentModel = singleDocumentData.documentDataController.documentModel;
					if (singleDocumentData.undoManager){
						singleDocumentData.undoManager.setStandardModel(documentModel);
					}

					documentManager.onDocumentSaved(documentId);
				}

				function onModelChanged(){
					singleDocumentData.modelReceived = true
					
					if (singleDocumentData.viewRegistered){
						singleDocumentData.initModelForView()
					}
				}

				function onError(){
					documentManager.documentOpeningFailed(singleDocumentData.documentId, qsTr("Internal error"))
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
}
