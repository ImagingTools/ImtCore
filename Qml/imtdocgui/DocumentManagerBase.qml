import QtQuick 2.12

QtObject {
	id: root

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
	signal documentGuiUpdated(string documentId, var representation)

	// callback(undefined) - cancel, callback(false) - close, callback(true) - save and close
	signal tryCloseDirtyDocument(string documentId, var callback)

	signal documentViewRegistered(string typeId, string viewTypeId)

	signal documentAlreadyOpened(string documentId, string typeId)

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

	function createDocument(typeId){
		console.warn("createDocument() should be implemented in a subclass")
	}

	function saveDocument(documentId){
		console.warn("saveDocument() should be implemented in a subclass")
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
			else{
				__internal.loadingStartTimes[documentId] = Date.now()
				if (!__loadingCheckAnim.running){
					__loadingCheckAnim.start()
				}
			}
		}

		if (!isLoading){
			delete __internal.loadingStartTimes[documentId]

			if (!docData.isNew){
				docData.documentDecorator.updateRepresentationForAllViews()
			}
			else{
				let decorator = docData.documentDecorator
				for (let i = 0; i < decorator.registeredViews.length; ++i){
					decorator.registeredViews[i].setBlockingUpdateModel(false)
					decorator.registeredViews[i].doUpdateGui()
				}
			}
			documentDataLoaded(documentId)
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
	}

	function getDocumentManagerActiveView(){
		return __internal.documentManagerActiveView
	}

	function setDocumentManagerActiveView(view){
		__internal.documentManagerActiveView = view
	}

	function setAutoNamedTypeId(typeId, hasProvider){
		__internal.autoNamedTypeIds[typeId] = hasProvider
	}

	function hasDocumentNameProvider(typeId){
		return typeId in __internal.autoNamedTypeIds && __internal.autoNamedTypeIds[typeId]
	}

	property int __loadingTimeoutMs: 30000

	property SequentialAnimation __loadingCheckAnim: SequentialAnimation {
		loops: Animation.Infinite
		PauseAnimation { duration: 5000 }
		ScriptAction {
			script: root.__checkLoadingTimeouts()
		}
	}

	function __checkLoadingTimeouts(){
		var now = Date.now()
		var keys = Object.keys(__internal.loadingStartTimes)
		var hasRemaining = false

		for (var i = 0; i < keys.length; i++){
			var docId = keys[i]
			if (now - __internal.loadingStartTimes[docId] > __loadingTimeoutMs){
				console.warn("DocumentManagerBase: loading timeout for document " + docId + ", forcing representation load as fallback")
				delete __internal.loadingStartTimes[docId]
				setDocumentIsLoading(docId, false)
			}
			else{
				hasRemaining = true
			}
		}

		if (!hasRemaining){
			__loadingCheckAnim.stop()
		}
	}

	property QtObject __internal: QtObject {
		property var documentTypeEditors: ({}) // DocumentTypeId -> [{View Type 1}, {View Type 2}]
		property var openedDocuments: [] // Array of objects {id, name, model, view, isDirty}
		property var cachedDocumentNames: ({}) // DocumentId -> Name
		property var pendingDataLoaded: ({}) // DocumentId -> true for early DocumentDataLoaded notifications
		property var loadingStartTimes: ({}) // DocumentId -> Date.now() timestamp for loading timeout tracking
		property var autoNamedTypeIds: ({}) // TypeId -> true for types with automatic name providers
		property var documentManagerActiveView: null

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
			delete loadingStartTimes[documentId]
			openedDocuments.splice(index, 1)
		}
	}
}
