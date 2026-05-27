import QtQuick 2.12
import Acf 1.0
import imtdocgui 1.0
import imtbaseCollectionDocumentServiceSdl 1.0
import imtbaseUndoManagerSdl 1.0

DocumentServiceBase {
	id: root

	property string collectionId

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

		documentTypeIdInput.m_typeId = typeId
		documentTypeIdInput.m_collectionId = collectionId
		documentTypeIdInput.m_proposedSourceDocumentId = proposedSourceDocumentId || ""
		createDocumentRequest.typeId = typeId

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

		saveDocumentInput.m_documentId = documentId || ""
		saveDocumentInput.m_documentName = documentName || ""
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

		undoRedoInput.m_documentId = documentId
		undoRedoInput.m_steps = steps
		collectionUndoRedoInput.m_collectionId = collectionId
		collectionUndoRedoInput.m_undoRedoInput = undoRedoInput
		doUndoRequest.documentId = documentId

		doUndoRequest.send(collectionUndoRedoInput)
	}

	function doRedo(documentId, steps){
		startRedo(documentId, steps)

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
					root.setAutoNamedTypeId(m_objectTypeId, m_hasNameProvider)
					root.setDocumentName(m_documentId, m_documentName)
					root.__internal.createDocumentData(m_documentId, m_objectTypeId, false)
					root.setDocumentObjectId(m_documentId, m_objectId)
					root.setDocumentIsLoading(m_documentId, true)
					root.documentOpened(m_documentId, m_objectTypeId)
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
					root.setAutoNamedTypeId(m_objectTypeId, m_hasNameProvider)
					root.setDocumentName(m_documentId, m_documentName)
					root.__internal.createDocumentData(m_documentId, m_objectTypeId, true)
					root.documentCreated(m_documentId, m_objectTypeId)
					root.setDocumentIsLoading(m_documentId, true)
				}
			}
		}

		property string typeId

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
					let statusMessage = function(defaultMessage){
						if (m_message !== undefined && m_message !== "") {
							return m_message
						}
						return defaultMessage
					}
					if (m_status === "Success"){
						let docId = root.saveDocumentRequest.documentId
						root.documentSaved(docId)
						if (m_documentName !== undefined && m_documentName !== "") {
							root.setDocumentName(docId, m_documentName)
						}
					}
					else if (m_status === "InvalidUserId"){
						root.saveDocumentFailed(root.saveDocumentRequest.documentId, statusMessage(qsTr("Invalid user-ID")))
					}
					else if (m_status === "InvalidDocumentId"){
						root.saveDocumentFailed(root.saveDocumentRequest.documentId, statusMessage(qsTr("Invalid document-ID")))
					}
					else if (m_status === "InvalidDocumentData"){
						root.saveDocumentFailed(root.saveDocumentRequest.documentId, statusMessage(qsTr("Document data is invalid")))
					}
					else if (m_status === "Failed"){
						root.saveDocumentFailed(root.saveDocumentRequest.documentId, statusMessage(qsTr("Save document failed")))
					}
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
					if (m_status === "Success"){
						root.documentClosed(root.closeDocumentRequest.documentId)
					}
					else if (m_status === "InvalidUserId"){
						root.closeDocumentFailed(root.closeDocumentRequest.documentId, qsTr("Invalid user-ID"))
					}
					else if (m_status === "InvalidDocumentId"){
						root.closeDocumentFailed(root.closeDocumentRequest.documentId, qsTr("Invalid document-ID"))
					}
					else if (m_status === "Failed"){
						root.closeDocumentFailed(root.closeDocumentRequest.documentId, qsTr("Close document failed"))
					}
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
					if (m_status === "Success"){
						root.undoDone(root.doUndoRequest.documentId)
					}
					else if (m_status === "InvalidUserId"){
						root.undoFailed(root.doUndoRequest.documentId, qsTr("Invalid user-ID"))
					}
					else if (m_status === "InvalidDocumentId"){
						root.undoFailed(root.doUndoRequest.documentId, qsTr("Invalid document-ID"))
					}
					else if (m_status === "Failed"){
						root.undoFailed(root.doUndoRequest.documentId, qsTr("Undo failed"))
					}
					else if (m_status === "InvalidStepCount"){
						root.undoFailed(root.doUndoRequest.documentId, qsTr("Invalid step count"))
					}
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
					if (m_status === "Success"){
						root.redoDone(root.doRedoRequest.documentId)
					}
					else if (m_status === "InvalidUserId"){
						root.redoFailed(root.doRedoRequest.documentId, qsTr("Invalid user-ID"))
					}
					else if (m_status === "InvalidDocumentId"){
						root.redoFailed(root.doRedoRequest.documentId, qsTr("Invalid document-ID"))
					}
					else if (m_status === "Failed"){
						root.redoFailed(root.doRedoRequest.documentId, qsTr("Redo failed"))
					}
					else if (m_status === "InvalidStepCount"){
						root.redoFailed(root.doRedoRequest.documentId, qsTr("Invalid step count"))
					}
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
