import QtQuick 2.12

QtObject {
	id: root

	signal startOpenDocument(string documentId, string typeId)
	signal documentOpened(string documentId, string typeId)
	signal openDocumentFailed(string documentId, string message)

	signal startCloseDocument(string documentId)
	signal documentClosed(string documentId)
	signal closeDocumentFailed(string documentId, string message)

	signal startSaveDocument(string documentId, var documentData)
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

	signal documentModelChanged(string documentId, var changeSet)

	signal documentNameChanged(string documentId, string oldName, string newName)

	signal startUndoInfoReceive(string documentId)
	signal undoInfoReceived(string documentId, int availableUndoSteps, int availableRedoSteps)
	signal undoInfoReceiveFailed(string documentId, string message)

	// typeOperation: OPENED, CREATED, CLOSED, SAVED, DOCUMENT_CHANGED
	signal documentManagerChanged(string typeOperation, string objectId, string documentId)

	onDocumentModelChanged: {
		setDocumentIsDirty(documentId, true)
	}

	onDocumentSaved: {
		setDocumentIsDirty(documentId, false)
		setDocumentIsNew(documentId, false)
	}

	onDocumentManagerChanged: {
		getUndoInfo(documentId)
	}

	onDocumentCreated: {
		__internal.createDocumentData(documentId, typeId, true)
	}

	onDocumentOpened: {
		__internal.createDocumentData(documentId, typeId, false)
	}

	function openDocument(typeId, documentId){
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

	function registerDocumentViewData(documentTypeId, viewTypeId, viewEditorComp, representationControllerComp){
		let obj = {"viewTypeId": viewTypeId, "viewEditorComp": viewEditorComp, "representationControllerComp": representationControllerComp}

		if (documentTypeId in __internal.documentTypeEditors){
			__internal.documentTypeEditors[documentTypeId].push(obj)
		}
		else{
			__internal.documentTypeEditors[documentTypeId] = [obj]
		}
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

	function setDocumentIsDirty(documentId, isDirty){
		let index = getDocumentIndexByDocumentId(documentId)
		if (index < 0){
			console.error("Unable to set is dirty for document '"+documentId+"'. Error: Document not found")
			return
		}

		__internal.openedDocuments[index].isDirty = isDirty
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
		if (viewTypeIds.include(viewTypeId)){
			return __internal.openedDocuments[index].views[viewTypeId]
		}

		if (viewTypeId === "" && viewTypeIds.length > 0){
			return __internal.openedDocuments[index].views[viewTypeIds[0]]
		}

		console.error("Unable to get view for document '"+documentId+"'. Error: View '"+viewTypeId+"' not found")
		return null
	}

	function onViewInstanceCreated(documentId, view, viewTypeId){
		console.log("onViewInstanceCreated", documentId, viewTypeId, view)

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

	property QtObject __internal: QtObject {
		property var documentTypeEditors: ({}) // DocumentTypeId -> [{View Type 1}, {View Type 2}]
		property var openedDocuments: [] // Array of objects {id, name, model, view, isDirty}

		property Component documentDataFactory: Component{ 
			QtObject{
				id: documentData

				property string id
				property string typeId
				property string name
				property bool isDirty: false
				property bool isNew: true
				property var views: ({})
				property DocumentDecorator documentDecorator: DocumentDecorator {
					onViewRegistered: {
						console.log("DocumentDecorator onViewRegistered", documentData.isNew)
						if (representationController.representationModel === null){
							console.error("Unable to register view for document '"+documentData.id+"'. Error: Representation model from controller is invalid")
							return
						}

						view.model = representationController.representationModel
						if (documentData.isNew){
							view.doUpdateModel()
						}
						else{
							let cb = function(documentId, representation){
								view.setBlockingUpdateModel(false)
								if (documentId === documentData.id){
									view.doUpdateGui()
								}
								representationController.representationUpdated.disconnect(cb)
							}
	
							representationController.representationUpdated.connect(cb)
							view.setBlockingUpdateModel(true)
							representationController.updateRepresentationFromDocument()
						}
					}
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
					documentDecorator.registerView(view, representationController)
				}

				function addView(viewTypeId, view){
					views[viewTypeId] = view
					viewAdded(viewTypeId, view)
				}
			}
		}

		function createDocumentData(id, typeId, isNew){
			let documentData = documentDataFactory.createObject(root)
			documentData.id = id
			documentData.typeId = typeId

			if (!isNew){
				isNew = false
			}
			documentData.isNew = isNew

			openedDocuments.push(documentData)
		}
	}
}
