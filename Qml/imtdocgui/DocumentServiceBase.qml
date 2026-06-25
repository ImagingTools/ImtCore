import QtQuick 2.12

QtObject {
	id: root

	// Override properties: assign a function to replace the default
	// implementation of the corresponding operation in subclasses.
	// The override receives the same arguments as the original function.
	// Use the handle* helper functions below to process responses.
	property var getOpenedDocumentListOverride: null
	property var openDocumentOverride: null
	property var createDocumentOverride: null
	property var saveDocumentOverride: null
	property var closeDocumentOverride: null
	property var doUndoOverride: null
	property var doRedoOverride: null
	property var getUndoInfoOverride: null

	signal startGetOpenedDocumentList()
	signal openedDocumentListReceived(var documentListInfo)
	signal openedDocumentListReceiveFailed(string message)

	signal startOpenDocument(string documentId, string typeId)
	signal documentOpened(string documentId, string typeId)
	signal openDocumentFailed(string documentId, string message)

	signal startCloseDocument(string documentId)
	signal documentClosed(string documentId)
	signal closeDocumentFailed(string documentId, string message)

	signal startSaveDocument(string documentId)
	signal saveDocumentFailed(string documentId, string message)
	signal documentSaved(string documentId)

	signal startCreateDocument(string typeId)
	signal createDocumentFailed(string typeId, string message)
	signal documentCreated(string documentId, string typeId)

	signal startUndo(string documentId, int steps)
	signal undoDone(string documentId)
	signal undoFailed(string documentId, string message)

	signal startRedo(string documentId, int steps)
	signal redoDone(string documentId)
	signal redoFailed(string documentId, string message)

	signal startResetUndo(string documentId)
	signal resetUndoDone(string documentId)
	signal resetUndoFailed(string documentId, string message)

	signal requestDocumentName(string documentId, string typeId)
	signal documentNameChanged(string documentId, string oldName, string newName)
	signal documentIsDirtyChanged(string documentId, bool isDirty)

	signal startUndoInfoReceive(string documentId)
	signal undoInfoReceived(string documentId, int availableUndoSteps, int availableRedoSteps, bool isDirty)
	signal undoInfoReceiveFailed(string documentId, string message)

	// typeOperation: NewDocumentCreated, DocumentOpened, DocumentChanged, DocumentSaved, DocumentClosed
	signal documentManagerChanged(string typeOperation, string objectId, string documentId, string documentName)
	signal documentDataLoaded(string documentId)
	signal startUpdateRepresentation(string documentId, var representation)
	signal documentRepresentationUpdated(string documentId, var representation)
	signal updateRepresentationFailed(string documentId, string message)
	signal updateDocumentFailed(string documentId, string message)
	signal documentGuiUpdated(string documentId, var representation)

	// callback(undefined) - cancel, callback(false) - close, callback(true) - save and close
	signal tryCloseDirtyDocument(string documentId, var callback)

	signal documentViewRegistered(string typeId, string viewTypeId)

	signal documentAlreadyOpened(string documentId, string typeId)

	// Emitted once per document when both:
	//   - the document data has finished loading from the server, AND
	//   - at least one view instance has been registered for the document.
	// Useful for single-document workspaces that need to switch from a
	// loading state to a content state regardless of the order in which the
	// two events happen.
	signal documentReady(string documentId)

	onDocumentSaved: {
		setDocumentIsNew(documentId, false)
	}

	onUndoInfoReceived: {
		setDocumentIsDirty(documentId, isDirty)
	}

	onStartCloseDocument: {
		let index = getDocumentIndexByDocumentId(documentId)
		if (index >= 0){
			__internal.openedDocuments[index].isClosing = true
		}
	}

	onDocumentClosed: {
		__internal.removeDocumentData(documentId)
	}

	onDocumentCreated: {
		__internal.createDocumentData(documentId, typeId, true)
	}

	onDocumentOpened: {
		__internal.createDocumentData(documentId, typeId, false)
	}

	function openDocument(objectTypeId, objectId){
		console.warn("openDocument() should be implemented in a subclass")
	}

	function createDocument(typeId, proposedSourceDocumentId){
		console.warn("createDocument() should be implemented in a subclass")
	}

	function saveDocument(documentId){
		console.warn("saveDocument() should be implemented in a subclass")
	}

	function setDocumentSaveNameResolver(documentId, resolver){
		if (!documentId){
			return
		}
		if (typeof resolver === "function"){
			__internal.documentSaveNameResolvers[documentId] = resolver
		}
		else{
			delete __internal.documentSaveNameResolvers[documentId]
		}
	}

	function clearDocumentSaveNameResolver(documentId){
		if (!documentId){
			return
		}
		delete __internal.documentSaveNameResolvers[documentId]
	}

	function resolveDocumentNameForSave(documentId, fallbackName){
		let resolvedName = fallbackName || ""
		if (!documentId){
			return resolvedName
		}

		let resolver = __internal.documentSaveNameResolvers[documentId]
		if (typeof resolver === "function"){
			let dynamicName = resolver(documentId)
			if (dynamicName){
				resolvedName = dynamicName
			}
		}

		return resolvedName
	}

	function closeDocument(documentId){
		console.warn("closeDocument() should be implemented in a subclass")
	}

	function doUndo(documentId, steps){
		console.warn("doUndo() should be implemented in a subclass")
	}

	function doRedo(documentId, steps){
		console.warn("doRedo() should be implemented in a subclass")
	}

	function resetUndo(documentId){
		console.warn("resetUndo() should be implemented in a subclass")
	}

	function getDefaultDocumentName(){
		return qsTr("<no name>")
	}

	function getUndoInfo(documentId){
		console.warn("getUndoInfo() should be implemented in a subclass")
	}

	function getOpenedDocumentList(){
		console.warn("getOpenedDocumentList() should be implemented in a subclass")
	}

	function getOpenedDocumentIds(){
		let retVal = []

		for (let i = 0; i < __internal.openedDocuments.length; ++i){
			retVal.push(__internal.openedDocuments[i].id)
		}

		return retVal
	}

	function registerDocumentViewData(documentTypeId, viewTypeId, viewEditorComp, representationControllerComp){
		let obj = {"viewTypeId": viewTypeId, "viewEditorComp": viewEditorComp, "representationControllerComp": representationControllerComp}

		if (documentTypeId in __internal.documentTypeEditors){
			let registeredViews = __internal.documentTypeEditors[documentTypeId]
			for (let i = 0; i < registeredViews.length; ++i){
				if (registeredViews[i].viewTypeId === viewTypeId){
					console.error("Unable to register view with type-ID '"+viewTypeId+"' for document type: '"+documentTypeId+"'. Error: View already registered")
					return
				}
			}

			__internal.documentTypeEditors[documentTypeId].push(obj)
		}
		else{
			__internal.documentTypeEditors[documentTypeId] = [obj]
		}

		documentViewRegistered(documentTypeId, viewTypeId)
	}

	function getDocumentEditorFactory(documentTypeId, viewTypeId){
		if (!viewTypeId){
			viewTypeId = ""
		}

		if (documentTypeId in __internal.documentTypeEditors){
			let viewArr = __internal.documentTypeEditors[documentTypeId]
			for (let i = 0; i < viewArr.length; ++i){
				if (viewTypeId === "" || viewArr[i].viewTypeId === viewTypeId){
					return viewArr[i].viewEditorComp
				}
			}
		}

		return null
	}

	function getDocumentRepresentationControllerFactory(documentTypeId, viewTypeId){
		if (!viewTypeId){
			viewTypeId = ""
		}

		if (documentTypeId in __internal.documentTypeEditors){
			let viewArr = __internal.documentTypeEditors[documentTypeId]
			for (let i = 0; i < viewArr.length; ++i){
				if (viewTypeId === "" || viewArr[i].viewTypeId === viewTypeId){
					return viewArr[i].representationControllerComp
				}
			}
		}

		return null
	}

	function documentIsNew(documentId){
		let index = getDocumentIndexByDocumentId(documentId)
		if (index < 0){
			return true
		}

		return __internal.openedDocuments[index].isNew
	}

	function setDocumentIsNew(documentId, isNew){
		let index = getDocumentIndexByDocumentId(documentId)
		if (index < 0){
			return
		}

		__internal.openedDocuments[index].isNew = isNew
	}

	function getSupportedDocumentTypeIds(){
		return Object.keys(__internal.documentTypeEditors)
	}

	function getDocumentIdByView(view){
		for (let i = 0; i < __internal.openedDocuments.length; ++i){
			let documentData = __internal.openedDocuments[i]

			let views = documentData.views
			let viewTypeIds = Object.keys(views)
			for (let j = 0; j < viewTypeIds.length; ++j){
				if (views[viewTypeIds[j]] === view){
					return documentData.id
				}
			}
		}

		return ""
	}

	function getSupportedDocumentViewTypeIds(documentTypeId){
		if (!(documentTypeId in __internal.documentTypeEditors)){
			return []
		}

		let viewTypeIds = []

		let viewArr = __internal.documentTypeEditors[documentTypeId]
		for (let i = 0; i < viewArr.length; ++i){
			viewTypeIds.push(viewArr[i].viewTypeId)
		}

		return viewTypeIds
	}

	function getDocumentTypeId(documentId){
		for (let i = 0; i < __internal.openedDocuments.length; ++i){
			let documentData = __internal.openedDocuments[i]
			if (documentData.id === documentId){
				return documentData.typeId
			}
		}

		return ""
	}

	function documentIsOpened(documentId){
		let index = getDocumentIndexByDocumentId(documentId)
		return index >= 0
	}

	function setDocumentObjectId(documentId, objectId){
		let index = getDocumentIndexByDocumentId(documentId)
		if (index < 0){
			return
		}

		__internal.openedDocuments[index].objectId = objectId
	}

	function getDocumentIdByObjectId(objectId){
		for (let i = 0; i < __internal.openedDocuments.length; ++i){
			let documentData = __internal.openedDocuments[i]
			if (documentData.objectId === objectId){
				return documentData.id
			}
		}

		return ""
	}

	// Returns the collection-objectId associated with a given documentId
	// (symmetric to setDocumentObjectId). Returns "" if the document is unknown
	// or has no associated objectId (e.g. a freshly created, unsaved document).
	function getDocumentObjectId(documentId){
		let index = getDocumentIndexByDocumentId(documentId)
		if (index < 0){
			return ""
		}

		return __internal.openedDocuments[index].objectId
	}

	function getDocumentIndexByDocumentId(documentId){
		for (let i = 0; i < __internal.openedDocuments.length; ++i){
			let documentData = __internal.openedDocuments[i]
			if (documentData.id === documentId){
				return i
			}
		}

		return -1
	}

	function setDocumentName(documentId, name){
		let index = getDocumentIndexByDocumentId(documentId)
		if (index < 0){
			__internal.cachedDocumentNames[documentId] = name
			return ""
		}

		let oldName = __internal.openedDocuments[index].name
		__internal.openedDocuments[index].name = name

		documentNameChanged(documentId, oldName, name)
	}

	function getDocumentName(documentId){
		let index = getDocumentIndexByDocumentId(documentId)
		if (index < 0){
			return ""
		}

		return __internal.openedDocuments[index].name
	}

	function documentIsDirty(documentId){
		let index = getDocumentIndexByDocumentId(documentId)
		if (index < 0){
			return false
		}

		return __internal.openedDocuments[index].isDirty
	}

	function documentIsLoading(documentId){
		let index = getDocumentIndexByDocumentId(documentId)
		if (index < 0){
			return false
		}

		return __internal.openedDocuments[index].isLoading
	}

	function setDocumentIsLoading(documentId, isLoading){
		let index = getDocumentIndexByDocumentId(documentId)
		if (index < 0){
			if (!isLoading){
				__internal.pendingDataLoaded[documentId] = true
			}
			return
		}

		let docData = __internal.openedDocuments[index]
		if (docData.isClosing){
			return
		}

		docData.isLoading = isLoading

		if (isLoading){
			if (documentId in __internal.pendingDataLoaded){
				delete __internal.pendingDataLoaded[documentId]
				docData.isLoading = false
				isLoading = false
			}
		}

		if (!isLoading){
			docData.documentDecorator.updateRepresentationForAllViews()
			documentDataLoaded(documentId)
			__internal.maybeEmitDocumentReady(documentId)
		}
	}

	function setDocumentIsDirty(documentId, isDirty){
		let index = getDocumentIndexByDocumentId(documentId)
		if (index < 0){
			return
		}

		__internal.openedDocuments[index].isDirty = isDirty

		documentIsDirtyChanged(documentId, isDirty)
	}

	function getViewTypeIdByViewFactory(documentTypeId, viewFactory){
		if (documentTypeId in __internal.documentTypeEditors){
			let editors = __internal.documentTypeEditors[documentTypeId]
			for (let i = 0; i < editors.length; ++i){
				if (editors[i].viewEditorComp === viewFactory){
					return editors[i].viewTypeId
				}
			}
		}

		return ""
	}

	function getDocumentViewInstance(documentId, viewTypeId){
		let index = getDocumentIndexByDocumentId(documentId)
		if (index < 0){
			console.error("Unable to get view for document '"+documentId+"'. Error: Document not found")
			return null
		}

		if (!viewTypeId){
			viewTypeId = ""
		}

		let viewTypeIds = Object.keys(__internal.openedDocuments[index].views)
		if (viewTypeIds.includes(viewTypeId)){
			return __internal.openedDocuments[index].views[viewTypeId]
		}

		if (viewTypeId === "" && viewTypeIds.length > 0){
			return __internal.openedDocuments[index].views[viewTypeIds[0]]
		}

		console.error("Unable to get view for document '"+documentId+"'. Error: View '"+viewTypeId+"' not found")
		return null
	}

	function onViewInstanceCreated(documentId, view, viewTypeId){
		if (!viewTypeId){
			viewTypeId = ""
		}

		let index = getDocumentIndexByDocumentId(documentId)
		if (index < 0){
			return
		}

		if (viewTypeId === ""){
			let documentTypeId = getDocumentTypeId(documentId)
			let viewTypeIds = getSupportedDocumentViewTypeIds(documentTypeId)

			if (viewTypeIds.length > 0){
				viewTypeId = viewTypeIds[0]
			}
		}

		__internal.openedDocuments[index].addView(viewTypeId, view)
		__internal.maybeEmitDocumentReady(documentId)
	}

	function getDocumentServiceActiveView(){
		return __internal.documentManagerActiveView
	}

	function setDocumentServiceActiveView(view){
		__internal.documentManagerActiveView = view
	}

	function setAutoNamedTypeId(typeId, hasProvider){
		__internal.autoNamedTypeIds[typeId] = hasProvider
	}

	function hasDocumentNameProvider(typeId){
		return typeId in __internal.autoNamedTypeIds && __internal.autoNamedTypeIds[typeId]
	}

	// ---- Response handler API ----
	// Call these from custom override implementations or onFinished handlers
	// to apply the standard DocumentService state transitions.

	function handleDocumentOpened(documentId, objectId, objectTypeId, documentName, hasNameProvider, isDirty){
		setAutoNamedTypeId(objectTypeId, hasNameProvider)
		setDocumentName(documentId, documentName)
		documentOpened(documentId, objectTypeId)
		setDocumentObjectId(documentId, objectId)
		if (isDirty)
			setDocumentIsDirty(documentId, true)
		setDocumentIsLoading(documentId, true)
	}

	function handleDocumentCreated(documentId, objectTypeId, documentName, hasNameProvider, proposedObjectId, isDirty){
		setAutoNamedTypeId(objectTypeId, hasNameProvider)
		setDocumentName(documentId, documentName)
		documentCreated(documentId, objectTypeId)
		if (proposedObjectId && proposedObjectId !== "")
			setDocumentObjectId(documentId, proposedObjectId)
		if (isDirty)
			setDocumentIsDirty(documentId, true)
		setDocumentIsLoading(documentId, true)
	}

	function handleSaveDocumentResult(documentId, status, message, documentName){
		if (status === "Success"){
			documentSaved(documentId)
			if (documentName !== undefined && documentName !== ""){
				setDocumentName(documentId, documentName)
			}
		}
		else{
			let msg = (message !== undefined && message !== "") ? message : status
			saveDocumentFailed(documentId, msg)
		}
	}

	function handleCloseDocumentResult(documentId, status){
		if (status === "Success"){
			documentClosed(documentId)
		}
		else{
			let msg = status
			if (status === "InvalidUserId") msg = qsTr("Invalid user-ID")
			else if (status === "InvalidDocumentId") msg = qsTr("Invalid document-ID")
			else if (status === "Failed") msg = qsTr("Close document failed")
			closeDocumentFailed(documentId, msg)
		}
	}

	function handleUndoResult(documentId, status){
		if (status === "Success"){
			undoDone(documentId)
		}
		else{
			let msg = status
			if (status === "InvalidUserId") msg = qsTr("Invalid user-ID")
			else if (status === "InvalidDocumentId") msg = qsTr("Invalid document-ID")
			else if (status === "Failed") msg = qsTr("Undo failed")
			else if (status === "InvalidStepCount") msg = qsTr("Invalid step count")
			undoFailed(documentId, msg)
		}
	}

	function handleRedoResult(documentId, status){
		if (status === "Success"){
			redoDone(documentId)
		}
		else{
			let msg = status
			if (status === "InvalidUserId") msg = qsTr("Invalid user-ID")
			else if (status === "InvalidDocumentId") msg = qsTr("Invalid document-ID")
			else if (status === "Failed") msg = qsTr("Redo failed")
			else if (status === "InvalidStepCount") msg = qsTr("Invalid step count")
			redoFailed(documentId, msg)
		}
	}

	property QtObject __internal: QtObject {
		property var documentTypeEditors: ({}) // DocumentTypeId -> [{View Type 1}, {View Type 2}]
		property var openedDocuments: [] // Array of objects {id, name, model, view, isDirty}
		property var cachedDocumentNames: ({}) // DocumentId -> Name
		property var documentSaveNameResolvers: ({}) // DocumentId -> function(documentId): string
		property var pendingDataLoaded: ({}) // DocumentId -> true for early DocumentDataLoaded notifications
		property var autoNamedTypeIds: ({}) // TypeId -> true for types with automatic name providers
		property var documentManagerActiveView: null
		property var readyEmitted: ({}) // DocumentId -> true once documentReady has been emitted

		property Component documentDataFactory: Component{ 
			QtObject{
				id: documentData

				property string id
				property string typeId
				property string name
				property string objectId: ""
				property bool isDirty: false
				property bool isNew: true
				property bool isLoading: false
				property bool isClosing: false
				property var views: ({})
				property DocumentDecorator documentDecorator: DocumentDecorator {
					documentId: documentData.id
					documentName: documentData.name
					documentTypeId: documentData.typeId
					documentManager: root
				}

				signal viewAdded(string viewTypeId, var view)

				onViewAdded: {
					let representationControllerFactory = root.getDocumentRepresentationControllerFactory(typeId, viewTypeId)
					if (representationControllerFactory === null){
						console.error("Unable to register view '"+viewTypeId+"' to document '"+id+"'")
						return
					}

					let representationController = representationControllerFactory.createObject(documentData)
					representationController.documentId = id
					representationController.view = view

					if (view.objectName === "DocumentViewBase"){
						if (view.representationController !== undefined){
							view.representationController = representationController
						}
					}

					documentDecorator.registerView(view, representationController, !isNew && !isLoading)
				}

				function addView(viewTypeId, view){
					views[viewTypeId] = view
					viewAdded(viewTypeId, view)
				}
			}
		}

		function createDocumentData(id, typeId, isNew){
			if (root.getDocumentIndexByDocumentId(id) >= 0){
				return
			}

			let documentData = documentDataFactory.createObject(root)
			documentData.id = id
			documentData.typeId = typeId

			if (id in root.__internal.cachedDocumentNames){
				documentData.name = root.__internal.cachedDocumentNames[id]
				delete root.__internal.cachedDocumentNames[id]
			}

			if (!isNew){
				isNew = false
			}
			documentData.isNew = isNew

			openedDocuments.push(documentData)
		}

		function removeDocumentData(documentId){
			let index = root.getDocumentIndexByDocumentId(documentId)
			if (index < 0){
				return
			}

			delete pendingDataLoaded[documentId]
			delete readyEmitted[documentId]
			delete documentSaveNameResolvers[documentId]
			openedDocuments.splice(index, 1)
		}

		// Emits root.documentReady(documentId) at most once per document, when
		// the document has finished loading AND at least one view has been
		// registered for it. Safe to call from multiple code paths.
		function maybeEmitDocumentReady(documentId){
			let index = root.getDocumentIndexByDocumentId(documentId)
			if (index < 0){
				return
			}

			if (readyEmitted[documentId]){
				return
			}

			let docData = openedDocuments[index]
			if (docData.isLoading){
				return
			}

			let viewTypeIds = Object.keys(docData.views)
			if (viewTypeIds.length === 0){
				return
			}

			readyEmitted[documentId] = true
			root.documentReady(documentId)
		}
	}
}
