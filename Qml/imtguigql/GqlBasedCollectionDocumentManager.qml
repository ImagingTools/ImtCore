import QtQuick 2.12
import Acf 1.0
import imtdocgui 1.0
import imtbaseCollectionDocumentManagerSdl 1.0
import imtbaseUndoManagerSdl 1.0

DocumentManagerBase {
	id: root

	property string collectionId

	SubscriptionClient {
		id: documentManagerSubscription

		function getHeaders(){
			return root.getHeaders()
		}

		onMessageReceived: {
			let objectId = data.getData("objectId")
			let documentId = data.getData("documentId")
			root.documentManagerChanged("", objectId, documentId)
		}
	}

	DocumentManagerNotification {
		id: documentManagerNotification
	}

	onCollectionIdChanged: {
		if (collectionId !== ""){
			documentManagerSubscription.gqlCommandId = "On" + root.collectionId + "DocumentChanged"
		}
	}

	function getHeaders(){
		return {}
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

		saveDocumentRequest.send(objectIdInput)
	}

	function closeDocument(documentId){
		startCloseDocument(documentId)

		documentIdInput.m_id = documentId
		documentIdInput.m_collectionId = collectionId
		closeDocumentRequest.documentId = documentId

		closeDocumentRequest.send(documentIdInput)
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
		getUndoInfoRequest.documentId = documentId

		getUndoInfoRequest.send(documentIdInput)
	}

	property ObjectId objectIdInput: ObjectId {}
	property DocumentId documentIdInput: DocumentId {}
	property DocumentTypeId documentTypeIdInput: DocumentTypeId {}
	property CollectionUndoRedoInput collectionUndoRedoInput: CollectionUndoRedoInput {}
	property UndoRedoInput undoRedoInput: UndoRedoInput {}

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
						root.closeDocumentFailed(root.closeDocumentRequest.documentId, qsTr("Save document failed"))
					}
				}
			}
		}

		property string documentId

		function getHeaders(){
			return root.getHeaders()
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
	}

	property GqlSdlRequestSender getUndoInfoRequest: GqlSdlRequestSender {
		gqlCommandId: ImtbaseCollectionDocumentManagerSdlCommandIds.s_getUndoInfo
		sdlObjectComp: Component {
			UndoInfo {
				onFinished: {
					root.undoInfoReceived(root.getUndoInfoRequest.documentId, m_availableUndoSteps, m_availableRedoSteps)
				}
			}
		}

		property string documentId

		function getHeaders(){
			return root.getHeaders()
		}
	}

	// property GqlSdlRequestSender resetUndoRequest: GqlSdlRequestSender {
	// 	gqlCommandId: ImtbaseCollectionDocumentManagerSdlCommandIds.s_resetUndo
	// 	requestType: 1
	// 	sdlObjectComp: Component {
	// 		UndoStatus {
	// 			onFinished: {
	// 				if (m_status === "Success"){
	// 					root.un(root.doRedoRequest.documentId)
	// 				}
	// 				else if (m_status === "InvalidUserId"){
	// 					root.redoFailed(root.doRedoRequest.documentId, qsTr("Invalid user-ID"))
	// 				}
	// 				else if (m_status === "InvalidDocumentId"){
	// 					root.redoFailed(root.doRedoRequest.documentId, qsTr("Invalid document-ID"))
	// 				}
	// 				else if (m_status === "Failed"){
	// 					root.redoFailed(root.doRedoRequest.documentId, qsTr("Redo failed"))
	// 				}
	// 				else if (m_status === "InvalidStepCount"){
	// 					root.redoFailed(root.doRedoRequest.documentId, qsTr("Invalid step count"))
	// 				}
	// 			}
	// 		}
	// 	}

	// 	property string documentId

	// 	function getHeaders(){
	// 		return root.getHeaders()
	// 	}
	// }
}
