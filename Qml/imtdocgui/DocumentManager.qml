import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

QtObject {
	id: documentManager;

	property string defaultDocumentName: qsTr("<no name>");
	property int documentsCount: documentsModel.count;

	property ListModel documentsModel: ListModel {
		dynamicRoles: true;
	}

	signal documentClosed(int documentIndex, string documentId);
	signal documentAdded(int documentIndex, string documentId);

	onDocumentsCountChanged: {
		documentIndexCorrection();
	}

	function documentIndexCorrection(){
		for (let i = 0; i < documentsModel.count; i++){
			let documentData = documentsModel.get(i).DocumentData;
			if (documentData){
				documentData.documentIndex = i;
			}
		}
	}


	function openErrorDialog(message){
		ModalDialogManager.openDialog(errorDialogComp, {"message": message});
	}


	function getDocumentsCount()
	{
		return documentsModel.count;
	}


	function getActiveView()
	{
		return internal.m_activeViewPtr;
	}


	function setActiveView(viewPtr)
	{
		if (internal.m_activeViewPtr !== viewPtr){
			internal.m_activeViewPtr = viewPtr;
		}
	}


	function getDocumentTypeId(document)
	{
		let documentsCount = getDocumentsCount();
		for (let documentIndex = 0; documentIndex < documentsCount; ++documentIndex){
			let info = internal.getSingleDocumentData(documentIndex);
			if (info.documentPtr === document){
				return info.documentTypeId;
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

	function createViewForDocument(documentTypeId, viewTypeId)
	{
		let viewComp = getDocumentViewComp(documentTypeId, viewTypeId);
		if (viewComp){
			return viewComp.createObject(documentManager);
		}

		return null;
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
			console.error("Data controller for documents with type-ID: ", documentTypeId, " is unregistered!");

			return defaultDataController.createObject(documentManager);
		}

		let dataControllerComp = internal.m_registeredDataControllers[documentTypeId];
		return dataControllerComp.createObject(documentManager);
	}


	function insertNewDocument(documentTypeId, viewTypeId)
	{
		let documentData = createTemplateDocument("", documentTypeId);
		if (!documentData){
			return false;
		}

		if (documentData.documentDataController){
			documentData.documentDataController.createDocumentModel();
		}

		addDocumentToModel("", documentTypeId, viewTypeId, documentData);

		return true;
	}

	function getDocumentData(index){
		if (index < 0 || index >= documentsModel.count){
			return null;
		}

		return documentsModel.get(index).DocumentData;
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


	function createTemplateDocument(documentId, documentTypeId){
		let singleDocumentData = singleDocumentDataComp.createObject(documentManager);
		if (singleDocumentData){
			if (documentId === ""){
				singleDocumentData.documentId = UuidGenerator.generateUUID();
			}
			else{
				singleDocumentData.documentId = documentId;
			}

			singleDocumentData.uuid = UuidGenerator.generateUUID();
			singleDocumentData.documentIndex = documentsModel.count;
			singleDocumentData.documentTypeId = documentTypeId;

			let registeredViewIds = getViewTypeIds(documentTypeId);

			for (let i = 0; i < registeredViewIds.length; i++){
				let viewId = registeredViewIds[i];

				let view = createViewForDocument(documentTypeId, viewId);
				singleDocumentData.views.push(view);
			}

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
		let index = getDocumentIndexByDocumentId(documentId);
		if (index >= 0){
			// already opened
			documentAdded(index, documentId);

			return;
		}

		Events.sendEvent("StartLoading");

		let documentData = createTemplateDocument(documentId, documentTypeId);
		if (!documentData){
			return false;
		}

		documentData.isNew = false;

		let onResult = function(){
			addDocumentToModel(documentId, documentTypeId, viewTypeId, documentData);

			documentData.documentDataController.modelChanged.disconnect(onResult);
		}

		if (documentData.documentDataController){
			documentData.documentDataController.updateDocumentModel();

			documentData.documentDataController.modelChanged.connect(onResult);
		}
		else{
			Events.sendEvent("StopLoading");
		}

		return true;
	}

	function addDocumentToModel(documentId, documentTypeId, viewTypeId, documentData)
	{
		documentsModel.append({
								"Uuid": documentData.uuid,
								"Title": defaultDocumentName,
								"TypeId": documentTypeId,
								"DocumentData": documentData,
								"IsNew": documentData.isNew
							});

		documentAdded(documentsModel.count - 1, documentId);

		documentManager.updateDocumentTitle(documentsModel.count - 1);
	}

	/*!
		Save document by UUID.

		\param      documentId           UUID of the document
	*/
	function saveDocument(documentId)
	{
		console.log("saveDocument", documentId);

		let index = getDocumentIndexByDocumentId(documentId);
		if (index >= 0){
			let isNew = documentsModel.get(index).IsNew;
			let document = documentsModel.get(index).DocumentData;
			if (document.isDirty){
				Events.sendEvent("StartLoading");

				for (let i = 0; i < document.views.length; i++){
					if (!document.views[i].readOnly){
						document.views[i].doUpdateModel();
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


	function onDocumentSaved(documentId, documentIndex)
	{
		Events.sendEvent("StopLoading");

		if (internal.m_closingDocuments.includes(documentId)){
			closeDocument(documentId);
		}

		if (documentIndex >= 0){
			let documentData = documentsModel.get(documentIndex);
			if (documentData && documentData.IsNew){
				documentsModel.setProperty(documentIndex, "IsNew", false);
			}
		}
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
			if (documentData && documentData.views.includes(view)){
				return documentData;
			}
		}

		return null;
	}


	function getDocumentTitle(documentIndex)
	{
		if (documentIndex < 0 || documentIndex >= documentsModel.count){
			return "";
		}

		return documentsModel.get(documentIndex).Title;
	}


	function generateDocumentTitle(documentIndex){
		if (documentIndex < 0 || documentIndex >= documentsModel.count){
			return "";
		}

		let title = defaultDocumentName;

		let documentData = documentsModel.get(documentIndex).DocumentData;

		let documentName = "";

		if (documentData){
			documentName = documentData.getDocumentName();
		}

		if (documentName && documentName !== ""){
			title = documentName;
		}

		if (documentData && documentData.isDirty){
			title = "* " + title;
		}

		return title;
	}


	function updateDocumentTitle(documentIndex){
		console.log("updateDocumentTitle", documentIndex);

		if (documentIndex < 0 || documentIndex >= documentsModel.count){
			return;
		}

		let documentTitle = generateDocumentTitle(documentIndex);

		setDocumentTitle(documentIndex, documentTitle);
	}


	function setDocumentTitle(documentIndex, title)
	{
		console.log("setDocumentTitle", documentIndex, title);

		if (documentIndex < 0 || documentIndex >= documentsModel.count){
			return;
		}

		documentsModel.setProperty(documentIndex, "Title", title);
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
		console.log("closeDocumentByIndex", documentIndex, force);
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

			documentData.removeAllViews();
			documentData.destroy();

			documentsModel.remove(documentIndex);

			documentClosed(documentIndex, "");
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

		if (documentData.views.length > 0){
			data["editor"] = documentData.views[0];
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

	property Component singleDocumentDataComp: Component{
		QtObject {
			id: singleDocumentData;

			// UUID for unification
			property string uuid: "";
			// index of the document in document manager
			property int documentIndex: -1;
			property bool isNew: true;
			property string documentId;
			property string documentName;
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

					for (let i = 0; i < singleDocumentData.views.length; i++){
						if (singleDocumentData.views[i].commandsController){
							singleDocumentData.views[i].commandsController.setCommandIsEnabled("Undo", undoSteps > 0);
							singleDocumentData.views[i].commandsController.setCommandIsEnabled("Redo", redoSteps > 0);
						}
					}
				}
			};

			property var views: [];
			property bool isDirty: false;

			Component.onDestruction: {
				console.log("singleDocumentData onDestruction");
				if (views.length > 0){
					removeAllViews();
				}
			}

			function removeAllViews(){
				for (let i = 0; i < views.length; i++){
					views[i].parent = null;
					views[i].destroy();
				}

				views = []
			}

			function updateGui(){
				for (let i = 0; i < singleDocumentData.views.length; i++){
					singleDocumentData.views[i].doUpdateGui();
				}
			}

			function updateModel(){
				for (let i = 0; i < singleDocumentData.views.length; i++){
					singleDocumentData.views[i].doUpdateModel();
				}
			}

			property Connections dataControllerConnections: Connections {
				target: singleDocumentData.documentDataController;

				function onSaved(documentId, documentName){
					singleDocumentData.documentId = documentId;
					singleDocumentData.documentName = documentName;

					singleDocumentData.isDirty = false;

					let documentModel = singleDocumentData.documentDataController.documentModel;
					if (singleDocumentData.undoManager){
						singleDocumentData.undoManager.setStandardModel(documentModel);
					}

					documentManager.updateDocumentTitle(singleDocumentData.documentIndex);

					documentManager.onDocumentSaved(documentId, singleDocumentData.documentIndex);
				}

				function onModelChanged(){
					if (!singleDocumentData.documentDataController || !singleDocumentData.documentDataController.documentModel){
						Events.sendEvent("StopLoading");

						return;
					}

					let documentModel = singleDocumentData.documentDataController.documentModel;

					singleDocumentData.documentId = singleDocumentData.documentDataController.getDocumentId();
					singleDocumentData.documentName = singleDocumentData.documentDataController.getDocumentName();

					singleDocumentData.blockingUpdateModel = true;

					for (let i = 0; i < singleDocumentData.views.length; i++){
						singleDocumentData.views[i].model = documentModel;

						if (singleDocumentData.views[i].commandsDelegate){
							singleDocumentData.views[i].commandsDelegate.commandActivated.connect(singleDocumentData.viewCommandHandle);
						}
					}

					// let isNew = documentManager.documentsModel.get(singleDocumentData.documentIndex).IsNew;
					if (singleDocumentData.isNew){
						singleDocumentData.updateModel();
					}
					else{
						singleDocumentData.updateGui();
					}

					singleDocumentData.blockingUpdateModel = false;

					documentManager.updateDocumentTitle(singleDocumentData.documentIndex);

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
				for (let i = 0; i < singleDocumentData.views.length; i++){
					if (singleDocumentData.views[i].commandsController){
						singleDocumentData.views[i].commandsController.setCommandIsEnabled("Save", isDirty);
					}

					if (singleDocumentData.views[i].commandsView){
						if (singleDocumentData.views[i].commandsView.setPositiveAccentCommandIds !== undefined){
							singleDocumentData.views[i].commandsView.setPositiveAccentCommandIds(["Save"]);
						}
					}
				}

				documentManager.updateDocumentTitle(documentIndex);
			}

			function getDocumentName(){
				if (singleDocumentData.documentDataController){
					return singleDocumentData.documentDataController.getDocumentName();
				}

				return "";
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
		function getSingleDocumentData(index)
		{
			if (index < 0 || index >= m_documentInfos.length){
				return null;
			}

			return m_documentInfos[index];
		}

		function getDocumentInfoFromView(view)
		{
			return null;
		}

		property var m_registeredView: ({});
		property var m_registeredDataControllers: ({});
		property var m_registeredValidators: ({});
		property var m_documentInfos: [];
		property var m_activeViewPtr;
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
