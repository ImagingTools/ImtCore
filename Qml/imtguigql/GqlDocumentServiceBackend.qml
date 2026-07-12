import QtQuick 2.12
import Acf 1.0
import imtbaseCollectionDocumentServiceSdl 1.0
import imtbaseUndoManagerSdl 1.0

// GraphQL transport for DocumentService. Owns every SDL/GQL-specific type in
// this feature: request senders, subscriptions, and the mapping between SDL
// status enums and the plain {ok, message} result shape DocumentService
// expects. DocumentService knows nothing about GraphQL; this is the only
// file that does (dependency injection instead of the inheritance the
// previous GqlBasedCollectionDocumentService used).
//
// Every operation creates a fresh, private GqlSdlRequestSender instance per
// call (via Component.createObject) instead of reusing one shared sender.
// A shared sender would mean a single `callback` property and a single
// in-flight input/response slot per operation type: two concurrent calls of
// the same operation (e.g. closeAllDocuments() closing several documents,
// or __restoreSession() fetching undo info for several documents) would
// clobber each other's callback, so only the last call's callback would
// ever fire — for the right document, wrong document, or not at all.
QtObject {
	id: root

	property string collectionId: ""

	// Optional function() -> object, forwarded as HTTP headers on every
	// request and subscription. Defaults to no extra headers.
	property var headersProvider: null

	// operation, objectId, documentId, documentName, isDirty, isLoading
	signal documentEvent(string operation, string objectId, string documentId, string documentName, bool isDirty, bool isLoading)
	// documentId, undoSteps, redoSteps, isDirty
	signal undoChanged(string documentId, int undoSteps, int redoSteps, bool isDirty)

	function getHeaders(){
		return typeof headersProvider === "function" ? headersProvider() : {}
	}

	function __mapStatus(status){
		if (status === "Success"){
			return ""
		}
		if (status === "InvalidUserId"){
			return qsTr("Invalid user-ID")
		}
		if (status === "InvalidDocumentId"){
			return qsTr("Invalid document-ID")
		}
		if (status === "InvalidStepCount"){
			return qsTr("Invalid step count")
		}
		return qsTr("Operation failed")
	}

	function getOpenedDocumentList(callback){
		let sender = getOpenedDocumentListRequestComp.createObject(root, {callback: callback})
		let input = collectionIdInputComp.createObject(sender, {m_collectionId: collectionId})
		sender.send(input)
	}

	// typeId is accepted for contract parity with other backends but unused
	// here: the server resolves the object's type from its collection entry.
	function openDocument(typeId, objectId, callback){
		let sender = openDocumentRequestComp.createObject(root, {callback: callback})
		let input = objectIdInputComp.createObject(sender, {m_id: objectId, m_collectionId: collectionId})
		sender.send(input)
	}

	function createDocument(typeId, proposedSourceDocumentId, callback){
		let proposedObjectId = proposedSourceDocumentId || ""
		let sender = createDocumentRequestComp.createObject(root, {callback: callback, proposedObjectId: proposedObjectId})
		let input = documentTypeIdInputComp.createObject(sender, {
			m_typeId: typeId,
			m_collectionId: collectionId,
			m_proposedSourceDocumentId: proposedObjectId
		})
		sender.send(input)
	}

	function saveDocument(documentId, documentName, callback){
		let sender = saveDocumentRequestComp.createObject(root, {callback: callback})
		let input = saveDocumentInputComp.createObject(sender, {
			m_documentId: documentId || "",
			m_documentName: documentName || "",
			m_collectionId: collectionId || ""
		})
		sender.send(input)
	}

	function closeDocument(documentId, callback){
		let sender = closeDocumentRequestComp.createObject(root, {callback: callback})
		let input = documentIdInputComp.createObject(sender, {m_id: documentId, m_collectionId: collectionId})
		sender.send(input)
	}

	function doUndo(documentId, steps, callback){
		let sender = doUndoRequestComp.createObject(root, {callback: callback})
		let undoRedoInput = undoRedoInputComp.createObject(sender, {m_documentId: documentId, m_steps: steps})
		let input = collectionUndoRedoInputComp.createObject(sender, {m_collectionId: collectionId, m_undoRedoInput: undoRedoInput})
		sender.send(input)
	}

	function doRedo(documentId, steps, callback){
		let sender = doRedoRequestComp.createObject(root, {callback: callback})
		let undoRedoInput = undoRedoInputComp.createObject(sender, {m_documentId: documentId, m_steps: steps})
		let input = collectionUndoRedoInputComp.createObject(sender, {m_collectionId: collectionId, m_undoRedoInput: undoRedoInput})
		sender.send(input)
	}

	function resetUndo(documentId, callback){
		let sender = resetUndoRequestComp.createObject(root, {callback: callback})
		let input = documentIdInputComp.createObject(sender, {m_id: documentId, m_collectionId: collectionId})
		sender.send(input)
	}

	function getUndoInfo(documentId, callback){
		let sender = getUndoInfoRequestComp.createObject(root, {callback: callback})
		let input = documentIdInputComp.createObject(sender, {m_id: documentId, m_collectionId: collectionId})
		sender.send(input)
	}

	property SubscriptionClient documentManagerSubscription: SubscriptionClient {
		function getHeaders(){
			return root.getHeaders()
		}

		onMessageReceived: {
			root.documentEvent(
						data.getData("documentOperation"),
						data.getData("objectId"),
						data.getData("documentId"),
						data.getData("documentName"),
						!!data.getData("isDirty"),
						!!data.getData("isLoading"))
		}
	}

	property SubscriptionClient undoManagerSubscription: SubscriptionClient {
		function getHeaders(){
			return root.getHeaders()
		}

		onMessageReceived: {
			root.undoChanged(
						data.getData("documentId"),
						data.getData("availableUndoSteps"),
						data.getData("availableRedoSteps"),
						!!data.getData("isDirty"))
		}
	}

	onCollectionIdChanged: {
		if (collectionId !== ""){
			documentManagerSubscription.gqlCommandId = "On" + collectionId + "DocumentChanged"
			undoManagerSubscription.gqlCommandId = "On" + collectionId + "UndoChanged"
		}
	}

	// ---- Input object factories (one fresh instance per call, parented to
	//      and destroyed with the sender that consumes it) --------------

	property Component collectionIdInputComp: Component { CollectionId {} }
	property Component objectIdInputComp: Component { ObjectId {} }
	property Component documentIdInputComp: Component { DocumentId {} }
	property Component saveDocumentInputComp: Component { SaveDocumentInput {} }
	property Component documentTypeIdInputComp: Component { DocumentTypeId {} }
	property Component collectionUndoRedoInputComp: Component { CollectionUndoRedoInput {} }
	property Component undoRedoInputComp: Component { UndoRedoInput {} }

	// ---- Request sender factories --------------------------------------

	property Component getOpenedDocumentListRequestComp: Component {
		GqlSdlRequestSender {
			id: sender
			gqlCommandId: ImtbaseCollectionDocumentServiceSdlCommandIds.s_getOpenedDocumentList
			property var callback: null

			sdlObjectComp: Component {
				DocumentList {
					onFinished: {
						let documents = []
						for (let i = 0; i < m_documentList.count; ++i){
							let info = m_documentList.get(i).item
							documents.push({
								documentId: info.m_documentId,
								objectId: info.m_objectId,
								typeId: info.m_objectTypeId,
								name: info.m_documentName,
								isDirty: info.m_isDirty,
								hasNameProvider: info.m_hasNameProvider,
								isLoading: info.m_isLoading
							})
						}

						if (sender.callback){
							sender.callback({ok: true, documents: documents})
						}
						sender.destroy()
					}
				}
			}

			function getHeaders(){
				return root.getHeaders()
			}

			function onError(message, type){
				if (callback){
					callback({ok: false, documents: [], message: message})
				}
				sender.destroy()
			}
		}
	}

	property Component openDocumentRequestComp: Component {
		GqlSdlRequestSender {
			id: sender
			gqlCommandId: ImtbaseCollectionDocumentServiceSdlCommandIds.s_openDocument
			requestType: 1
			property var callback: null

			sdlObjectComp: Component {
				DocumentInfo {
					onFinished: {
						if (sender.callback){
							sender.callback({
								ok: true,
								documentId: m_documentId,
								objectId: m_objectId,
								typeId: m_objectTypeId,
								name: m_documentName,
								hasNameProvider: m_hasNameProvider,
								isDirty: m_isDirty,
								isLoading: m_isLoading
							})
						}
						sender.destroy()
					}
				}
			}

			function getHeaders(){
				return root.getHeaders()
			}

			function onError(message, type){
				if (callback){
					callback({ok: false, message: message})
				}
				sender.destroy()
			}
		}
	}

	property Component createDocumentRequestComp: Component {
		GqlSdlRequestSender {
			id: sender
			gqlCommandId: ImtbaseCollectionDocumentServiceSdlCommandIds.s_createNewDocument
			requestType: 1
			property var callback: null
			property string proposedObjectId: ""

			sdlObjectComp: Component {
				DocumentInfo {
					onFinished: {
						if (sender.callback){
							sender.callback({
								ok: true,
								documentId: m_documentId,
								objectId: (m_objectId && m_objectId !== "") ? m_objectId : sender.proposedObjectId,
								typeId: m_objectTypeId,
								name: m_documentName,
								hasNameProvider: m_hasNameProvider,
								isDirty: m_isDirty,
								isLoading: m_isLoading
							})
						}
						sender.destroy()
					}
				}
			}

			function getHeaders(){
				return root.getHeaders()
			}

			function onError(message, type){
				if (callback){
					callback({ok: false, message: message})
				}
				sender.destroy()
			}
		}
	}

	property Component saveDocumentRequestComp: Component {
		GqlSdlRequestSender {
			id: sender
			gqlCommandId: ImtbaseCollectionDocumentServiceSdlCommandIds.s_saveDocument
			requestType: 1
			property var callback: null

			sdlObjectComp: Component {
				DocumentOperationStatus {
					onFinished: {
						if (sender.callback){
							sender.callback({
								ok: m_status === "Success",
								message: m_status === "Success" ? "" : (m_message || root.__mapStatus(m_status)),
								documentName: m_documentName
							})
						}
						sender.destroy()
					}
				}
			}

			function getHeaders(){
				return root.getHeaders()
			}

			function onError(message, type){
				if (callback){
					callback({ok: false, message: message})
				}
				sender.destroy()
			}
		}
	}

	property Component closeDocumentRequestComp: Component {
		GqlSdlRequestSender {
			id: sender
			gqlCommandId: ImtbaseCollectionDocumentServiceSdlCommandIds.s_closeDocument
			requestType: 1
			property var callback: null

			sdlObjectComp: Component {
				DocumentOperationStatus {
					onFinished: {
						if (sender.callback){
							sender.callback({
								ok: m_status === "Success",
								message: m_status === "Success" ? "" : root.__mapStatus(m_status)
							})
						}
						sender.destroy()
					}
				}
			}

			function getHeaders(){
				return root.getHeaders()
			}

			function onError(message, type){
				if (callback){
					callback({ok: false, message: message})
				}
				sender.destroy()
			}
		}
	}

	property Component doUndoRequestComp: Component {
		GqlSdlRequestSender {
			id: sender
			gqlCommandId: ImtbaseCollectionDocumentServiceSdlCommandIds.s_doUndo
			requestType: 1
			property var callback: null

			sdlObjectComp: Component {
				UndoStatus {
					onFinished: {
						if (sender.callback){
							sender.callback({
								ok: m_status === "Success",
								message: m_status === "Success" ? "" : root.__mapStatus(m_status)
							})
						}
						sender.destroy()
					}
				}
			}

			function getHeaders(){
				return root.getHeaders()
			}

			function onError(message, type){
				if (callback){
					callback({ok: false, message: message})
				}
				sender.destroy()
			}
		}
	}

	property Component doRedoRequestComp: Component {
		GqlSdlRequestSender {
			id: sender
			gqlCommandId: ImtbaseCollectionDocumentServiceSdlCommandIds.s_doRedo
			requestType: 1
			property var callback: null

			sdlObjectComp: Component {
				UndoStatus {
					onFinished: {
						if (sender.callback){
							sender.callback({
								ok: m_status === "Success",
								message: m_status === "Success" ? "" : root.__mapStatus(m_status)
							})
						}
						sender.destroy()
					}
				}
			}

			function getHeaders(){
				return root.getHeaders()
			}

			function onError(message, type){
				if (callback){
					callback({ok: false, message: message})
				}
				sender.destroy()
			}
		}
	}

	property Component resetUndoRequestComp: Component {
		GqlSdlRequestSender {
			id: sender
			gqlCommandId: ImtbaseCollectionDocumentServiceSdlCommandIds.s_resetUndo
			requestType: 1
			property var callback: null

			sdlObjectComp: Component {
				UndoStatus {
					onFinished: {
						if (sender.callback){
							sender.callback({
								ok: m_status === "Success",
								message: m_status === "Success" ? "" : root.__mapStatus(m_status)
							})
						}
						sender.destroy()
					}
				}
			}

			function getHeaders(){
				return root.getHeaders()
			}

			function onError(message, type){
				if (callback){
					callback({ok: false, message: message})
				}
				sender.destroy()
			}
		}
	}

	property Component getUndoInfoRequestComp: Component {
		GqlSdlRequestSender {
			id: sender
			gqlCommandId: ImtbaseCollectionDocumentServiceSdlCommandIds.s_getUndoInfo
			property var callback: null

			sdlObjectComp: Component {
				UndoInfo {
					onFinished: {
						if (sender.callback){
							sender.callback({
								ok: true,
								undoSteps: m_availableUndoSteps,
								redoSteps: m_availableRedoSteps,
								isDirty: m_isDirty
							})
						}
						sender.destroy()
					}
				}
			}

			function getHeaders(){
				return root.getHeaders()
			}

			function onError(message, type){
				if (callback){
					callback({ok: false, message: message})
				}
				sender.destroy()
			}
		}
	}
}
