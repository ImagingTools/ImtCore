import QtQuick 2.12

// Unified, transport-agnostic document service for multi-user collection
// documents. Replaces the DocumentServiceBase / GqlBasedCollectionDocumentService
// inheritance pair: instead of subclassing to plug in GraphQL, a transport
// delegate is injected via the `backend` property (dependency injection).
//
// `backend` must implement:
//   getOpenedDocumentList(callback)
//   openDocument(typeId, objectId, callback)
//   createDocument(typeId, proposedSourceDocumentId, callback)
//   saveDocument(documentId, documentName, callback)
//   closeDocument(documentId, callback)
//   doUndo(documentId, steps, callback)
//   doRedo(documentId, steps, callback)
//   resetUndo(documentId, callback)
//   getUndoInfo(documentId, callback)
//   signal documentEvent(operation, objectId, documentId, documentName, isDirty, isLoading)
//   signal undoChanged(documentId, undoSteps, redoSteps, isDirty)
// See GqlDocumentServiceBackend (network, multi-user) and
// LocalDocumentServiceBackend (in-process, client-managed) for the two
// concrete implementations.
QtObject {
	id: root

	property var backend: null
	property EditorRegistry editors: EditorRegistry {
		onViewRegistered: root.documentViewRegistered(documentTypeId, viewTypeId)
	}
	property DocumentInteractionController interaction: DocumentInteractionController {}
	property DocumentEditorHost editorHost: DocumentEditorHost { documentService: root }
	property Component __documentComp: Component { Document {} }

	// Set by the hosting workspace to track which view currently has focus.
	property var __activeView: null

	signal documentAdded(string documentId, string typeId)
	signal documentAlreadyOpened(string documentId, string typeId)
	signal documentClosed(string documentId)
	signal documentReady(string documentId)
	signal documentViewRegistered(string documentTypeId, string viewTypeId)

	// Emitted when a document changed on the server for a reason other than
	// this session's own in-flight request (e.g. another user's edit).
	// DocumentViewBinder listens to this to re-pull representations.
	signal documentChangedRemotely(string documentId)

	signal documentGuiUpdated(string documentId, var representationModel)
	signal updateRepresentationFailed(string documentId, string message)
	signal updateDocumentFailed(string documentId, string message)

	// operation: "open" | "create" | "save" | "close" | "undo" | "redo" |
	//            "resetUndo" | "getUndoInfo" | "getOpenedDocumentList"
	signal operationFailed(string documentId, string operation, string message)

	property QtObject __internal: QtObject {
		property var openedDocuments: []            // Document[]
		property var autoNamedTypeIds: ({})          // typeId -> hasProvider
		property var cachedNames: ({})                // documentId -> name (race guard)
		property var cachedObjectIds: ({})             // documentId -> objectId (race guard)
		property var pendingDataLoaded: ({})           // documentId -> true (race guard)
	}

	onBackendChanged: {
		if (backend){
			__restoreSession()
		}
	}

	Connections {
		target: root.backend

		function onDocumentEvent(operation, objectId, documentId, documentName, isDirty, isLoading){
			root.__onBackendDocumentEvent(operation, objectId, documentId, documentName, isDirty, isLoading)
		}

		function onUndoChanged(documentId, undoSteps, redoSteps, isDirty){
			let doc = root.__findDocument(documentId)
			if (!doc){
				return
			}
			doc.undoSteps = undoSteps
			doc.redoSteps = redoSteps
			doc.isDirty = isDirty
		}
	}

	// ---- Session restore -----------------------------------------------

	// True while the initial list of already-open documents (session
	// restore) is being fetched. Workspaces bind their top-level loading
	// overlay to this instead of listening for dedicated start/stop signals.
	property bool restoringSession: false

	function __restoreSession(){
		restoringSession = true

		backend.getOpenedDocumentList(function(result){
			restoringSession = false

			if (!result.ok){
				root.operationFailed("", "getOpenedDocumentList", result.message || "")
				root.interaction.showError(result.message || "")
				return
			}

			for (let i = 0; i < result.documents.length; ++i){
				let info = result.documents[i]
				root.__setAutoNamedTypeId(info.typeId, info.hasNameProvider)

				let doc = root.__createDocument(info.documentId, info.typeId, info.objectId === "")
				doc.objectId = info.objectId
				doc.name = info.name
				doc.isDirty = info.isDirty
				doc.dataReady = !info.isLoading

				root.documentAdded(doc.documentId, doc.typeId)
				root.getUndoInfo(doc.documentId)
			}
		})
	}

	// ---- Backend event handling ------------------------------------------

	function __onBackendDocumentEvent(operation, objectId, documentId, documentName, isDirty, isLoading){
		if (operation === "DocumentDataLoaded"){
			let doc = root.__findDocument(documentId)
			if (doc){
				root.__markDataReady(doc)
			}
			else{
				__internal.pendingDataLoaded[documentId] = true
			}
			return
		}

		if (operation === "DocumentClosed"){
			root.__removeDocument(documentId)
			return
		}

		if (operation === "DocumentRenamed" || operation === "NewDocumentCreated" || operation === "DocumentOpened"){
			root.setDocumentName(documentId, documentName)
			return
		}

		if (operation === "DocumentChanged"){
			root.documentChangedRemotely(documentId)
		}

		// DocumentSaved / DocumentSavedAs: the authoritative name/state is
		// already applied via the direct mutation response; re-applying it
		// from the subscription payload here has been observed to corrupt
		// non-ASCII names, so it is intentionally skipped.
	}

	// ---- Registry lookups --------------------------------------------------

	function __findDocument(documentId){
		for (let i = 0; i < __internal.openedDocuments.length; ++i){
			if (__internal.openedDocuments[i].documentId === documentId){
				return __internal.openedDocuments[i]
			}
		}
		return null
	}

	function __createDocument(documentId, typeId, isNew){
		let existing = __findDocument(documentId)
		if (existing){
			return existing
		}

		let doc = __documentComp.createObject(root, {defaultName: root.getDefaultDocumentName()})
		doc.documentId = documentId
		doc.typeId = typeId
		doc.isNew = !!isNew

		if (documentId in __internal.cachedNames){
			doc.name = __internal.cachedNames[documentId]
			delete __internal.cachedNames[documentId]
		}
		if (documentId in __internal.cachedObjectIds){
			doc.objectId = __internal.cachedObjectIds[documentId]
			delete __internal.cachedObjectIds[documentId]
		}
		if (documentId in __internal.pendingDataLoaded){
			delete __internal.pendingDataLoaded[documentId]
			doc.dataReady = true
		}

		__internal.openedDocuments.push(doc)

		doc.becameReady.connect(function(){
			root.documentReady(doc.documentId)
		})

		return doc
	}

	function __removeDocument(documentId){
		let index = -1
		for (let i = 0; i < __internal.openedDocuments.length; ++i){
			if (__internal.openedDocuments[i].documentId === documentId){
				index = i
				break
			}
		}
		if (index < 0){
			return
		}

		let doc = __internal.openedDocuments[index]
		let binders = doc.binders.slice()
		for (let i = 0; i < binders.length; ++i){
			binders[i].dispose()
		}

		delete __internal.pendingDataLoaded[documentId]
		delete __internal.cachedObjectIds[documentId]
		__internal.openedDocuments.splice(index, 1)
		doc.destroy()

		documentClosed(documentId)
	}

	function __markDataReady(doc){
		if (doc.isClosing){
			return
		}

		doc.dataReady = true

		for (let i = 0; i < doc.binders.length; ++i){
			let binder = doc.binders[i]
			if (doc.isNew){
				binder.sync.paintFromCurrentModel()
			}
			else{
				binder.sync.pull()
			}
		}
	}

	function __setAutoNamedTypeId(typeId, hasProvider){
		__internal.autoNamedTypeIds[typeId] = !!hasProvider
	}

	// ---- Public query API (documentId-parametrized, kept for the many
	//      product editors/collection views that address documents by ID) ----

	function documentIsOpened(documentId){
		return __findDocument(documentId) !== null
	}

	function documentIsNew(documentId){
		let doc = __findDocument(documentId)
		return doc ? doc.isNew : true
	}

	function documentIsDirty(documentId){
		let doc = __findDocument(documentId)
		return doc ? doc.isDirty : false
	}

	function documentIsLoading(documentId){
		let doc = __findDocument(documentId)
		return doc ? doc.loading : false
	}

	function getDocument(documentId){
		return __findDocument(documentId)
	}

	function getDocumentName(documentId){
		let doc = __findDocument(documentId)
		return doc ? doc.name : ""
	}

	function setDocumentName(documentId, name){
		let doc = __findDocument(documentId)
		if (!doc){
			__internal.cachedNames[documentId] = name
			return
		}
		doc.name = name
	}

	function getDocumentTypeId(documentId){
		let doc = __findDocument(documentId)
		return doc ? doc.typeId : ""
	}

	function getDocumentObjectId(documentId){
		let doc = __findDocument(documentId)
		return doc ? doc.objectId : ""
	}

	function setDocumentObjectId(documentId, objectId){
		let doc = __findDocument(documentId)
		if (!doc){
			__internal.cachedObjectIds[documentId] = objectId
			return
		}
		doc.objectId = objectId
	}

	function getDocumentIdByObjectId(objectId){
		for (let i = 0; i < __internal.openedDocuments.length; ++i){
			if (__internal.openedDocuments[i].objectId === objectId){
				return __internal.openedDocuments[i].documentId
			}
		}
		return ""
	}

	function getOpenedDocumentIds(){
		return __internal.openedDocuments.map(doc => doc.documentId)
	}

	// Closes every currently open document. Each close still goes through
	// the normal dirty-confirmation flow (see closeDocument), so this is
	// "request everything closed", not a forced/silent close.
	function closeAllDocuments(){
		let ids = getOpenedDocumentIds()
		for (let i = 0; i < ids.length; ++i){
			closeDocument(ids[i])
		}
	}

	function getDefaultDocumentName(){
		return qsTr("<no name>")
	}

	function hasDocumentNameProvider(typeId){
		return typeId in __internal.autoNamedTypeIds && __internal.autoNamedTypeIds[typeId]
	}

	function setDocumentSaveNameResolver(documentId, resolver){
		let doc = __findDocument(documentId)
		if (doc){
			doc.nameResolver = (typeof resolver === "function") ? resolver : null
		}
	}

	function clearDocumentSaveNameResolver(documentId){
		let doc = __findDocument(documentId)
		if (doc){
			doc.nameResolver = null
		}
	}

	// ---- Editor registry pass-through ----------------------------------

	function registerDocumentViewData(documentTypeId, viewTypeId, viewEditorComp, representationControllerComp){
		editors.register(documentTypeId, viewTypeId, viewEditorComp, representationControllerComp)
	}

	function getSupportedDocumentTypeIds(){
		return editors.getSupportedTypeIds()
	}

	function getSupportedDocumentViewTypeIds(documentTypeId){
		return editors.getSupportedViewTypeIds(documentTypeId)
	}

	function getDocumentEditorFactory(documentTypeId, viewTypeId){
		return editors.getEditorFactory(documentTypeId, viewTypeId)
	}

	function getDocumentRepresentationControllerFactory(documentTypeId, viewTypeId){
		return editors.getRepresentationControllerFactory(documentTypeId, viewTypeId)
	}

	function getViewTypeIdByViewFactory(documentTypeId, viewFactory){
		return editors.getViewTypeIdByEditorFactory(documentTypeId, viewFactory)
	}

	// ---- View instance lifecycle (used by DocumentEditorHost) ----------

	function registerDocumentView(documentId, viewTypeId, view, controller){
		let doc = __findDocument(documentId)
		if (!doc){
			console.error("DocumentService: cannot register view for unknown document '" + documentId + "'")
			return
		}

		// Binder (and its Connections to this service's documentReady signal)
		// must exist BEFORE addView() can flip the document to ready: addView
		// may synchronously complete document.ready when data was already
		// loaded, and documentReady must not fire into a binder that isn't
		// wired up yet.
		let binder = documentViewBinderComp.createObject(root, {
			documentService: root,
			document: doc,
			view: view,
			controller: controller
		})
		doc.binders.push(binder)

		doc.addView(viewTypeId, view)

		binder.initialize(!doc.isNew && doc.dataReady)
	}

	property Component documentViewBinderComp: Component {
		DocumentViewBinder {}
	}

	function getDocumentViewInstance(documentId, viewTypeId){
		let doc = __findDocument(documentId)
		if (!doc){
			console.error("Unable to get view for document '" + documentId + "'. Error: Document not found")
			return null
		}

		let view = doc.getView(viewTypeId)
		if (!view){
			console.error("Unable to get view for document '" + documentId + "'. Error: View '" + viewTypeId + "' not found")
		}
		return view
	}

	function getDocumentServiceActiveView(){
		return __activeView
	}

	function setDocumentServiceActiveView(view){
		__activeView = view
	}

	// ---- Open / create ---------------------------------------------------

	function openDocument(typeId, objectId){
		let existingDocumentId = getDocumentIdByObjectId(objectId)
		if (existingDocumentId !== ""){
			documentAlreadyOpened(existingDocumentId, typeId)
			return
		}

		backend.openDocument(typeId, objectId, function(result){
			if (!result.ok){
				root.operationFailed("", "open", result.message || "")
				root.interaction.showError(result.message || "")
				return
			}

			root.__setAutoNamedTypeId(result.typeId, result.hasNameProvider)
			let doc = root.__createDocument(result.documentId, result.typeId, false)
			doc.name = result.name
			doc.objectId = result.objectId
			doc.isDirty = !!result.isDirty
			doc.dataReady = !result.isLoading

			root.documentAdded(doc.documentId, doc.typeId)
		})
	}

	function createDocument(typeId, proposedSourceDocumentId){
		backend.createDocument(typeId, proposedSourceDocumentId || "", function(result){
			if (!result.ok){
				root.operationFailed("", "create", result.message || "")
				root.interaction.showError(result.message || "")
				return
			}

			root.__setAutoNamedTypeId(result.typeId, result.hasNameProvider)
			let doc = root.__createDocument(result.documentId, result.typeId, true)
			doc.name = result.name
			if (result.objectId){
				doc.objectId = result.objectId
			}
			doc.isDirty = !!result.isDirty
			doc.dataReady = !result.isLoading

			root.documentAdded(doc.documentId, doc.typeId)
		})
	}

	// Unified entry point: creates a new document when objectId is empty,
	// otherwise opens the existing collection object.
	function openOrCreateByObjectId(typeId, objectId, proposedSourceDocumentId){
		if (!objectId || objectId === ""){
			createDocument(typeId, proposedSourceDocumentId)
		}
		else{
			openDocument(typeId, objectId)
		}
	}


	// ---- Save --------------------------------------------------------------

	// Pushes any in-progress GUI edit into the representation model before
	// save/validate look at it. Without this, an edit still sitting in a
	// focused input (e.g. the user typed a new name and hit Save without
	// tabbing out first, so no editingFinished ever fired) would be silently
	// dropped: save() would persist the model's last-pushed value instead of
	// what's currently on screen.
	function __flushViews(doc){
		for (let i = 0; i < doc.binders.length; ++i){
			let view = doc.binders[i].view
			if (view && typeof view.doUpdateModel === "function"){
				view.doUpdateModel()
			}
		}
	}

	// Consults validate() on every view bound to `doc` (see DocumentViewBase).
	// The first view to report itself invalid blocks the save.
	function __validateDocument(doc){
		for (let i = 0; i < doc.binders.length; ++i){
			let view = doc.binders[i].view
			if (!view || typeof view.validate !== "function"){
				continue
			}

			let result = view.validate()
			if (result && result.valid === false){
				return {valid: false, message: result.message || qsTr("Document is not valid")}
			}
		}

		return {valid: true, message: ""}
	}

	function save(documentId, explicitName){
		let doc = __findDocument(documentId)
		if (!doc){
			return
		}

		__flushViews(doc)

		let validation = __validateDocument(doc)
		if (!validation.valid){
			root.operationFailed(documentId, "save", validation.message)
			root.interaction.showError(validation.message)
			return
		}

		if (hasDocumentNameProvider(doc.typeId)){
			__sendSave(doc, "")
			return
		}

		// Consult the resolver (if any) before falling back to the explicit/
		// existing name. This lets workspaces that disable interactive name
		// input (documentNameInputEnabled=false) always resolve a name here
		// without ever reaching the dialog below.
		let resolvedName = doc.resolveNameForSave(explicitName || doc.name)
		if (resolvedName){
			__sendSave(doc, resolvedName)
			return
		}

		interaction.requestDocumentName(documentId, function(chosenName){
			if (!chosenName){
				return
			}
			doc.name = chosenName
			root.__sendSave(doc, doc.resolveNameForSave(chosenName))
		})
	}

	// Kept as an alias so existing call sites (`documentManager.saveDocument(id, name)`)
	// keep working without change.
	function saveDocument(documentId, documentName){
		save(documentId, documentName)
	}

	function __sendSave(doc, name){
		doc.busy = true
		backend.saveDocument(doc.documentId, name, function(result){
			doc.busy = false

			if (!result.ok){
				root.operationFailed(doc.documentId, "save", result.message || "")
				root.interaction.showError(result.message || "")
				return
			}

			doc.isNew = false
			if (result.documentName){
				doc.name = result.documentName
			}
			// Refresh the dirty/undo-step counters explicitly rather than
			// relying solely on a backend push: a save changes what "clean"
			// means (the baseline moves), and not every backend proactively
			// notifies about that (the local backend in particular doesn't).
			root.getUndoInfo(doc.documentId)
			root.documentSaved(doc.documentId)
		})
	}

	// ---- Close -------------------------------------------------------------

	// force=true skips the dirty-confirmation dialog entirely and discards
	// any unsaved changes: used when the underlying object was already
	// deleted/replaced server-side (nothing left to save against) or when a
	// caller needs an unconditional close (e.g. switching tenants).
	function closeDocument(documentId, force){
		let doc = __findDocument(documentId)
		if (!doc){
			return
		}

		if (force){
			root.__doClose(doc)
			return
		}

		let proceed = function(shouldSaveFirst){
			if (shouldSaveFirst === undefined){
				return // user cancelled
			}

			if (!shouldSaveFirst){
				root.__doClose(doc)
				return
			}

			let onSaved = function(savedDocumentId){
				if (savedDocumentId !== documentId){
					return
				}
				root.documentSaved.disconnect(onSaved)
				root.operationFailed.disconnect(onSaveFailed)
				root.__doClose(doc)
			}
			let onSaveFailed = function(failedDocumentId, operation, message){
				if (failedDocumentId !== documentId || operation !== "save"){
					return
				}
				root.documentSaved.disconnect(onSaved)
				root.operationFailed.disconnect(onSaveFailed)
			}
			root.documentSaved.connect(onSaved)
			root.operationFailed.connect(onSaveFailed)
			save(documentId)
		}

		if (doc.isDirty){
			interaction.confirmCloseDirty(proceed)
		}
		else{
			proceed(false)
		}
	}

	// Internal signal used only to observe save completion from closeDocument's
	// save-then-close flow without re-entrant polling.
	signal documentSaved(string documentId)

	function __doClose(doc){
		doc.isClosing = true
		doc.busy = true

		backend.closeDocument(doc.documentId, function(result){
			if (!result.ok){
				doc.busy = false
				doc.isClosing = false
				root.operationFailed(doc.documentId, "close", result.message || "")
				root.interaction.showError(result.message || "")
				return
			}

			root.__removeDocument(doc.documentId)
		})
	}

	// ---- Undo / redo ---------------------------------------------------

	function doUndo(documentId, steps){
		let doc = __findDocument(documentId)
		if (!doc){
			return
		}
		doc.busy = true
		backend.doUndo(documentId, steps, function(result){
			doc.busy = false
			if (!result.ok){
				root.operationFailed(documentId, "undo", result.message || "")
				root.interaction.showError(result.message || "")
				return
			}
			root.getUndoInfo(documentId)
		})
	}

	function doRedo(documentId, steps){
		let doc = __findDocument(documentId)
		if (!doc){
			return
		}
		doc.busy = true
		backend.doRedo(documentId, steps, function(result){
			doc.busy = false
			if (!result.ok){
				root.operationFailed(documentId, "redo", result.message || "")
				root.interaction.showError(result.message || "")
				return
			}
			root.getUndoInfo(documentId)
		})
	}

	function resetUndo(documentId){
		let doc = __findDocument(documentId)
		if (!doc){
			return
		}
		backend.resetUndo(documentId, function(result){
			if (!result.ok){
				root.operationFailed(documentId, "resetUndo", result.message || "")
				root.interaction.showError(result.message || "")
				return
			}
			root.getUndoInfo(documentId)
		})
	}

	function getUndoInfo(documentId){
		backend.getUndoInfo(documentId, function(result){
			let doc = root.__findDocument(documentId)
			if (!doc){
				return
			}
			if (!result.ok){
				// Not surfaced as a dialog: this runs automatically after every
				// undo/redo and for every document during session restore, so a
				// modal here would fire far more often than a real user action.
				root.operationFailed(documentId, "getUndoInfo", result.message || "")
				return
			}
			doc.undoSteps = result.undoSteps
			doc.redoSteps = result.redoSteps
			doc.isDirty = result.isDirty
		})
	}
}
