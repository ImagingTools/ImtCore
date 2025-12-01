import QtQuick 2.12
import Acf 1.0
import imtdocgui 1.0
import imtbaseCollectionDocumentManagerSdl 1.0
import imtbaseUndoManagerSdl 1.0

DocumentManagerBase {
	id: root

	property string collectionId

	property SubscriptionClient documentManagerSubscription: SubscriptionClient{
		function getHeaders(){
			return root.getHeaders()
		}

		onMessageReceived: {
			console.log("Document Manager onMessageReceived")
			let objectId = data.getData("objectId")
			let documentId = data.getData("documentId")
			let operation = data.getData("documentOperation")

			root.documentManagerChanged(operation, objectId, documentId)
		}
	}

	property SubscriptionClient undoManagerSubscription: SubscriptionClient {
		function getHeaders(){
			return root.getHeaders()
		}

		onMessageReceived: {
			console.log("UndoChanged onMessageReceived", data.toJson())
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
		startOpenDocument(documentId, typeId)

		openDocumentRequest.typeId = typeId
		objectIdInput.m_id = documentId
		objectIdInput.m_collectionId = collectionId

		openDocumentRequest.send(objectIdInput)
	}

	function createDocument(typeId){
		startCreateDocument(typeId)

		documentTypeIdInput.m_typeId = typeId
		documentTypeIdInput.m_collectionId = collectionId
		createDocumentRequest.typeId = typeId

		createDocumentRequest.send(documentTypeIdInput)
	}

	function saveDocument(documentId){
		startSaveDocument(documentId)

		documentIdInput.m_id = documentId
		documentIdInput.m_collectionId = collectionId
		saveDocumentRequest.documentId = documentId

		saveDocumentRequest.send(documentIdInput)
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
	property DocumentTypeId documentTypeIdInput: DocumentTypeId {}
	property CollectionUndoRedoInput collectionUndoRedoInput: CollectionUndoRedoInput {}
	property UndoRedoInput undoRedoInput: UndoRedoInput {}

	property GqlSdlRequestSender getOpenedDocumentListRequest: GqlSdlRequestSender {
		gqlCommandId: ImtbaseCollectionDocumentManagerSdlCommandIds.s_getOpenedDocumentList
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
		gqlCommandId: ImtbaseCollectionDocumentManagerSdlCommandIds.s_openDocument
		requestType: 1
		sdlObjectComp: Component {
			DocumentId {
				onFinished: {
					root.documentOpened(m_id, root.openDocumentRequest.typeId)
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
		gqlCommandId: ImtbaseCollectionDocumentManagerSdlCommandIds.s_createNewDocument
		requestType: 1
		sdlObjectComp: Component {
			DocumentId {
				onFinished: {
					root.documentCreated(m_id, root.createDocumentRequest.typeId)
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
		gqlCommandId: ImtbaseCollectionDocumentManagerSdlCommandIds.s_saveDocument
		requestType: 1
		sdlObjectComp: Component {
			DocumentOperationStatus {
				onFinished: {
					if (m_status === "Success"){
						root.documentSaved(root.saveDocumentRequest.documentId)
					}
					else if (m_status === "InvalidUserId"){
						root.saveDocumentFailed(root.saveDocumentRequest.documentId, qsTr("Invalid user-ID"))
					}
					else if (m_status === "InvalidDocumentId"){
						root.saveDocumentFailed(root.saveDocumentRequest.documentId, qsTr("Invalid document-ID"))
					}
					else if (m_status === "Failed"){
						root.saveDocumentFailed(root.saveDocumentRequest.documentId, qsTr("Save document failed"))
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
		gqlCommandId: ImtbaseCollectionDocumentManagerSdlCommandIds.s_closeDocument
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
		gqlCommandId: ImtbaseCollectionDocumentManagerSdlCommandIds.s_doUndo
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
		gqlCommandId: ImtbaseCollectionDocumentManagerSdlCommandIds.s_doRedo
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
		gqlCommandId: ImtbaseCollectionDocumentManagerSdlCommandIds.s_getUndoInfo
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
