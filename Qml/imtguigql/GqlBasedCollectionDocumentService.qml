import QtQuick 2.12
import Acf 1.0
import imtdocgui 1.0
import imtbaseCollectionDocumentServiceSdl 1.0
import imtbaseUndoManagerSdl 1.0

DocumentServiceBase {
	id: root

	property string collectionId

	// Override properties: assign a function to fully replace the default
	// implementation of the corresponding operation. The override receives
	// the same arguments as the original function. Use the handle*Result
	// helper functions below to process the server response without
	// duplicating internal logic.
	property var getOpenedDocumentListOverride: null
	property var openDocumentOverride: null
	property var createDocumentOverride: null
	property var saveDocumentOverride: null
	property var closeDocumentOverride: null
	property var doUndoOverride: null
	property var doRedoOverride: null
	property var getUndoInfoOverride: null

	// ---- Response handler API ----
	// Call these from custom onFinished handlers to apply the standard
	// DocumentService state transitions without copying internal logic.

	// Call after a successful open-document response.
	function handleDocumentOpened(documentId, objectId, objectTypeId, documentName, hasNameProvider){
		root.setAutoNamedTypeId(objectTypeId, hasNameProvider)
		root.setDocumentName(documentId, documentName)
		root.__internal.createDocumentData(documentId, objectTypeId, false)
		root.setDocumentObjectId(documentId, objectId)
		root.setDocumentIsLoading(documentId, true)
		root.documentOpened(documentId, objectTypeId)
	}

	// Call after a successful create-document response.
	function handleDocumentCreated(documentId, objectTypeId, documentName, hasNameProvider, proposedObjectId){
		root.setAutoNamedTypeId(objectTypeId, hasNameProvider)
		root.setDocumentName(documentId, documentName)
		root.__internal.createDocumentData(documentId, objectTypeId, true)
		if (proposedObjectId && proposedObjectId !== "")
			root.setDocumentObjectId(documentId, proposedObjectId)
		root.documentCreated(documentId, objectTypeId)
		root.setDocumentIsLoading(documentId, true)
	}

	// Call after a save-document response.
	// status: "Success", "InvalidUserId", "InvalidDocumentId",
	//         "InvalidDocumentData", "Failed"
	function handleSaveDocumentResult(documentId, status, message, documentName){
		if (status === "Success"){
			root.documentSaved(documentId)
			if (documentName !== undefined && documentName !== ""){
				root.setDocumentName(documentId, documentName)
			}
		}
		else{
			let msg = (message !== undefined && message !== "") ? message : status
			root.saveDocumentFailed(documentId, msg)
		}
	}

	// Call after a close-document response.
	// status: "Success", "InvalidUserId", "InvalidDocumentId", "Failed"
	function handleCloseDocumentResult(documentId, status){
		if (status === "Success"){
			root.documentClosed(documentId)
		}
		else{
			let msg = status
			if (status === "InvalidUserId") msg = qsTr("Invalid user-ID")
			else if (status === "InvalidDocumentId") msg = qsTr("Invalid document-ID")
			else if (status === "Failed") msg = qsTr("Close document failed")
			root.closeDocumentFailed(documentId, msg)
		}
	}

	// Call after an undo response.
	// status: "Success", "InvalidUserId", "InvalidDocumentId", "Failed",
	//         "InvalidStepCount"
	function handleUndoResult(documentId, status){
		if (status === "Success"){
			root.undoDone(documentId)
		}
		else{
			let msg = status
			if (status === "InvalidUserId") msg = qsTr("Invalid user-ID")
			else if (status === "InvalidDocumentId") msg = qsTr("Invalid document-ID")
			else if (status === "Failed") msg = qsTr("Undo failed")
			else if (status === "InvalidStepCount") msg = qsTr("Invalid step count")
			root.undoFailed(documentId, msg)
		}
	}

	// Call after a redo response.
	// status: "Success", "InvalidUserId", "InvalidDocumentId", "Failed",
	//         "InvalidStepCount"
	function handleRedoResult(documentId, status){
		if (status === "Success"){
			root.redoDone(documentId)
		}
		else{
			let msg = status
			if (status === "InvalidUserId") msg = qsTr("Invalid user-ID")
			else if (status === "InvalidDocumentId") msg = qsTr("Invalid document-ID")
			else if (status === "Failed") msg = qsTr("Redo failed")
			else if (status === "InvalidStepCount") msg = qsTr("Invalid step count")
			root.redoFailed(documentId, msg)
		}
	}

	property SubscriptionClient documentManagerSubscription: SubscriptionClient{
		function getHeaders(){
			return root.getHeaders()
		}

		onMessageReceived: {
			let objectId = data.getData("objectId")
			let documentId = data.getData("documentId")
			let documentName = data.getData("documentName")
			let operation = data.getData("documentOperation")

			if (operation === "DocumentDataLoaded"){
				root.setDocumentIsLoading(documentId, false)
			}
			else if (operation === "DocumentClosed"){
				root.documentClosed(documentId)
			}
			else if (operation === "DocumentRenamed"){
				root.setDocumentName(documentId, documentName)
			}

			root.documentManagerChanged(operation, objectId, documentId, documentName)
		}
	}

	property SubscriptionClient undoManagerSubscription: SubscriptionClient {
		function getHeaders(){
			return root.getHeaders()
		}

		onMessageReceived: {
			let documentId = data.getData("documentId")
			let availableUndoSteps = data.getData("availableUndoSteps")
			let availableRedoSteps = data.getData("availableRedoSteps")
			let isDirty = data.getData("isDirty")

			root.undoInfoReceived(documentId, availableUndoSteps, availableRedoSteps, isDirty)
		}
	}

	onCollectionIdChanged: {
		if (collectionId !== ""){
			documentManagerSubscription.gqlCommandId = "On" + root.collectionId + "DocumentChanged"
			undoManagerSubscription.gqlCommandId = "On" + root.collectionId + "UndoChanged"
			getOpenedDocumentList()
		}
	}

	function getHeaders(){
		return {}
	}

	function getOpenedDocumentList(){
		startGetOpenedDocumentList()

		if (getOpenedDocumentListOverride){
			getOpenedDocumentListOverride()
			return
		}

		collectionIdInput.m_collectionId = collectionId
		getOpenedDocumentListRequest.send(collectionIdInput)
	}

	function openDocument(typeId, documentId){
		let existingDocumentId = getDocumentIdByObjectId(documentId)
		if (existingDocumentId !== ""){
			documentAlreadyOpened(existingDocumentId, typeId)
			return
		}

		startOpenDocument(documentId, typeId)

		if (openDocumentOverride){
			openDocumentOverride(typeId, documentId)
			return
		}

		openDocumentRequest.typeId = typeId
		objectIdInput.m_id = documentId
		objectIdInput.m_collectionId = collectionId

		openDocumentRequest.send(objectIdInput)
	}

	// Creates a new document of the given typeId.
	//
	// Optional `proposedSourceDocumentId`: if non-empty, propagated to the
	// server in `DocumentTypeId.proposedSourceDocumentId` so that the new
	// collection object is inserted with this id (see
	// CCollectionDocumentServiceControllerComp::OnCreateNewDocument and
	// TCollectionDocumentServiceWrap::InsertNewObject). This lets the client
	// pre-allocate the object id (e.g. a fresh UUID) and keeps client- and
	// server-side representations consistent (`representation.m_id` matches
	// the persisted object id without an extra round-trip).
	function createDocument(typeId, proposedSourceDocumentId){
		startCreateDocument(typeId)

		if (createDocumentOverride){
			createDocumentOverride(typeId, proposedSourceDocumentId)
			return
		}

		documentTypeIdInput.m_typeId = typeId
		documentTypeIdInput.m_collectionId = collectionId
		documentTypeIdInput.m_proposedSourceDocumentId = proposedSourceDocumentId || ""
		createDocumentRequest.typeId = typeId
		createDocumentRequest.proposedObjectId = proposedSourceDocumentId || ""

		createDocumentRequest.send(documentTypeIdInput)
	}

	// Unified entry point for single- and multi-document workspaces:
	// - if objectId is empty, creates a new document of the given typeId
	//   (forwarding `proposedSourceDocumentId` to createDocument())
	// - otherwise opens the existing collection object identified by objectId
	// Existing per-document deduplication in openDocument() (via
	// getDocumentIdByObjectId) still applies, so calling this repeatedly for
	// the same objectId will surface documentAlreadyOpened.
	function openOrCreateByObjectId(typeId, objectId, proposedSourceDocumentId){
		if (!objectId || objectId === ""){
			createDocument(typeId, proposedSourceDocumentId)
		}
		else{
			openDocument(typeId, objectId)
		}
	}

	function saveDocument(documentId, documentName){
		startSaveDocument(documentId)

		if (saveDocumentOverride){
			saveDocumentOverride(documentId, documentName)
			return
		}

		let resolvedDocumentName = root.resolveDocumentNameForSave(documentId, documentName || "")

		saveDocumentInput.m_documentId = documentId || ""
		saveDocumentInput.m_documentName = resolvedDocumentName
		saveDocumentInput.m_collectionId = collectionId || ""
		saveDocumentRequest.documentId = documentId || ""

		saveDocumentRequest.send(saveDocumentInput)
	}

	function closeDocument(documentId){
		let closeFunc = function(result){
			if (result === undefined){
				// Cancel
				return
			}

			if (result){
				// Try save
				let savedFunc = function(savedDocumentId){
					if (documentId === savedDocumentId){
						documentSaved.disconnect(savedFunc)
						closeFunc(false)
					}
				}

				let savedFailedFunc = function(savedDocumentId, message){
					if (documentId === savedDocumentId){
						documentSaved.disconnect(savedFunc)
						saveDocumentFailed.disconnect(savedFailedFunc)
					}
				}

				documentSaved.connect(savedFunc)
				saveDocumentFailed.connect(savedFailedFunc)
				saveDocument(documentId)
			}
			else{
				// Close
				startCloseDocument(documentId)

				if (closeDocumentOverride){
					closeDocumentOverride(documentId)
					return
				}

				documentIdInput.m_id = documentId
				documentIdInput.m_collectionId = collectionId
				closeDocumentRequest.documentId = documentId
		
				closeDocumentRequest.send(documentIdInput)
			}
		}

		if (documentIsDirty(documentId)){
			tryCloseDirtyDocument(documentId, closeFunc)
		}
		else{
			closeFunc(false)
		}
	}

	function doUndo(documentId, steps){
		startUndo(documentId, steps)

		if (doUndoOverride){
			doUndoOverride(documentId, steps)
			return
		}

		undoRedoInput.m_documentId = documentId
		undoRedoInput.m_steps = steps
		collectionUndoRedoInput.m_collectionId = collectionId
		collectionUndoRedoInput.m_undoRedoInput = undoRedoInput
		doUndoRequest.documentId = documentId

		doUndoRequest.send(collectionUndoRedoInput)
	}

	function doRedo(documentId, steps){
		startRedo(documentId, steps)

		if (doRedoOverride){
			doRedoOverride(documentId, steps)
			return
		}

		undoRedoInput.m_documentId = documentId
		undoRedoInput.m_steps = steps
		collectionUndoRedoInput.m_collectionId = collectionId
		collectionUndoRedoInput.m_undoRedoInput = undoRedoInput
		doRedoRequest.documentId = documentId

		doRedoRequest.send(collectionUndoRedoInput)
	}

	function resetUndo(documentId){
		startResetUndo(documentId)
	}

	function getUndoInfo(documentId){
		startUndoInfoReceive(documentId)

		if (getUndoInfoOverride){
			getUndoInfoOverride(documentId)
			return
		}

		documentIdInput.m_id = documentId
		documentIdInput.m_collectionId = collectionId

		getUndoInfoRequest.send(documentIdInput)
	}

	property CollectionId collectionIdInput: CollectionId {}
	property ObjectId objectIdInput: ObjectId {}
	property DocumentId documentIdInput: DocumentId {}
	property SaveDocumentInput saveDocumentInput: SaveDocumentInput {}
	property DocumentTypeId documentTypeIdInput: DocumentTypeId {}
	property CollectionUndoRedoInput collectionUndoRedoInput: CollectionUndoRedoInput {}
	property UndoRedoInput undoRedoInput: UndoRedoInput {}

	property GqlSdlRequestSender getOpenedDocumentListRequest: GqlSdlRequestSender {
		gqlCommandId: ImtbaseCollectionDocumentServiceSdlCommandIds.s_getOpenedDocumentList
		sdlObjectComp: Component {
			DocumentList {
				onFinished: {
					root.openedDocumentListReceived(m_documentList)
				}
			}
		}

		function getHeaders(){
			return root.getHeaders()
		}
	}

	property GqlSdlRequestSender openDocumentRequest: GqlSdlRequestSender {
		gqlCommandId: ImtbaseCollectionDocumentServiceSdlCommandIds.s_openDocument
		requestType: 1
		sdlObjectComp: Component {
			DocumentInfo {
				onFinished: {
					root.handleDocumentOpened(m_documentId, m_objectId, m_objectTypeId, m_documentName, m_hasNameProvider)
				}
			}
		}

		property string typeId

		function getHeaders(){
			return root.getHeaders()
		}

		function onError(message, type){
			root.openDocumentFailed("", message)
		}
	}

	property GqlSdlRequestSender createDocumentRequest: GqlSdlRequestSender {
		gqlCommandId: ImtbaseCollectionDocumentServiceSdlCommandIds.s_createNewDocument
		requestType: 1
		sdlObjectComp: Component {
			DocumentInfo {
				onFinished: {
					root.handleDocumentCreated(m_documentId, m_objectTypeId, m_documentName, m_hasNameProvider, root.createDocumentRequest.proposedObjectId)
				}
			}
		}

		property string typeId
		property string proposedObjectId: ""

		function getHeaders(){
			return root.getHeaders()
		}

		function onError(message, type){
			root.createDocumentFailed("", message)
		}
	}

	property GqlSdlRequestSender saveDocumentRequest: GqlSdlRequestSender {
		gqlCommandId: ImtbaseCollectionDocumentServiceSdlCommandIds.s_saveDocument
		requestType: 1
		sdlObjectComp: Component {
			DocumentOperationStatus {
				onFinished: {
					root.handleSaveDocumentResult(root.saveDocumentRequest.documentId, m_status, m_message, m_documentName)
				}
			}
		}

		property string documentId

		function getHeaders(){
			return root.getHeaders()
		}

		function onError(message, type){
			root.saveDocumentFailed(root.saveDocumentRequest.documentId, message)
		}
	}

	property GqlSdlRequestSender closeDocumentRequest: GqlSdlRequestSender {
		gqlCommandId: ImtbaseCollectionDocumentServiceSdlCommandIds.s_closeDocument
		requestType: 1
		sdlObjectComp: Component {
			DocumentOperationStatus {
				onFinished: {
					root.handleCloseDocumentResult(root.closeDocumentRequest.documentId, m_status)
				}
			}
		}

		property string documentId

		function getHeaders(){
			return root.getHeaders()
		}

		function onError(message, type){
			root.closeDocumentFailed("", message)
		}
	}

	property GqlSdlRequestSender doUndoRequest: GqlSdlRequestSender {
		gqlCommandId: ImtbaseCollectionDocumentServiceSdlCommandIds.s_doUndo
		requestType: 1

		sdlObjectComp: Component {
			UndoStatus {
				onFinished: {
					root.handleUndoResult(root.doUndoRequest.documentId, m_status)
				}
			}
		}

		property string documentId

		function getHeaders(){
			return root.getHeaders()
		}

		function onError(message, type){
			root.undoFailed(documentId, message)
		}
	}

	property GqlSdlRequestSender doRedoRequest: GqlSdlRequestSender {
		gqlCommandId: ImtbaseCollectionDocumentServiceSdlCommandIds.s_doRedo
		requestType: 1

		sdlObjectComp: Component {
			UndoStatus {
				onFinished: {
					root.handleRedoResult(root.doRedoRequest.documentId, m_status)
				}
			}
		}

		property string documentId

		function getHeaders(){
			return root.getHeaders()
		}

		function onError(message, type){
			root.redoFailed(documentId, message)
		}
	}

	property GqlSdlRequestSender getUndoInfoRequest: GqlSdlRequestSender {
		gqlCommandId: ImtbaseCollectionDocumentServiceSdlCommandIds.s_getUndoInfo
		sdlObjectComp: Component {
			UndoInfo {
				onFinished: {
					root.undoInfoReceived(m_documentId, m_availableUndoSteps, m_availableRedoSteps, m_isDirty)
				}
			}
		}

		function getHeaders(){
			return root.getHeaders()
		}
	}
}
