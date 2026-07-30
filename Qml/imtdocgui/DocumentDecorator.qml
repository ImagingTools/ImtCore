import QtQuick 2.12
import imtbaseCollectionDocumentServiceSdl 1.0

QtObject {
	id: root

	property string documentId
	property string documentName
	property string documentTypeId
	property var documentManager: null

	property var registeredViews: []
	property var registeredRepresentation: []

	signal viewRegistered(var view, var representationController, bool updateRepresentation)

	onViewRegistered: {
		if (view){
			view.setBlockingUpdateModel(true)
			view.model = representationController.representationModel
			view.commandActivated.connect(onCommandActivated)
			view.modelDataChanged.connect(onModelDataChanged)
			view.guiUpdated.connect(onGuiUpdated)
			view.guiVisibleChanged.connect(onGuiVisibleChanged)

			representationController.representationUpdated.connect(onRepresentationUpdated)
			representationController.startUpdateRepresentation.connect(onStartUpdateRepresentation)
			representationController.updateRepresentationFailed.connect(onUpdateRepresentationFailed)
			representationController.updateDocumentFailed.connect(onUpdateDocumentFailed)

			if (documentManager && view.commandsController){
				let isDirty = documentManager.documentIsDirty(documentId)
				view.commandsController.setCommandIsEnabled("Save", isDirty)
			}

			if (updateRepresentation){
				if (view.visible){
					representationController.updateRepresentationFromDocument()
				}
				else{
					if (!_internal.requestUpdateViews.includes(view)){
						_internal.requestUpdateViews.push(view)
					}
				}
			}

			if (view.objectName === "DocumentViewBase"){
				if (view.representationController !== undefined){
					view.representationController = representationController
				}
			}
		}
	}

	function onCommandActivated(commandId){
		if (commandId === "Undo"){
			onUndo()
		}
		else if (commandId === "Redo"){
			onRedo()
		}
		else if (commandId === "Save"){
			onSave()
		}
	}

	property Connections documentManagerConnections: Connections {
		target: root.documentManager

		function onStartSaveDocument(documentId){
			if (documentId !== root.documentId){
				return
			}
			root._internal.saveRequested = false
		}

		function onUndoInfoReceived(documentId, availableUndoSteps, availableRedoSteps, isDirty){
			if (documentId !== root.documentId){
				return
			}

			for (let i = 0; i < root.registeredViews.length; ++i){
				if (root.registeredViews[i].commandsController){
					root.registeredViews[i].commandsController.setCommandIsEnabled("Undo", availableUndoSteps > 0)
					root.registeredViews[i].commandsController.setCommandIsEnabled("Redo", availableRedoSteps > 0)
					root.registeredViews[i].commandsController.setCommandIsEnabled("Save", isDirty)
				}
			}
		}

		function onDocumentIsDirtyChanged(documentId, isDirty){
			if (documentId !== root.documentId){
				return
			}

			for (let i = 0; i < root.registeredViews.length; ++i){
				if (root.registeredViews[i].commandsController){
					root.registeredViews[i].commandsController.setCommandIsEnabled("Save", isDirty)
				}
			}
		}

		function onDocumentManagerChanged(typeOperation, objectId, documentId, hasChanges){
			if (documentId !== root.documentId){
				return
			}

			if (typeOperation === EDocumentOperationEnum.s_documentChanged){
				root.updateRepresentationForAllViews()
			}

			if (typeOperation === EDocumentOperationEnum.s_documentSaved){
				for (let i = 0; i < root.registeredViews.length; ++i){
					if (root.registeredViews[i] && typeof root.registeredViews[i].documentSaved === "function"){
						root.registeredViews[i].documentSaved()
					}
				}
			}
		}

		function onDocumentNameChanged(documentId, oldName, newName){
			if (documentId !== root.documentId){
				return
			}

			root.documentName = newName

			if (root._internal.saveRequested){
				root._internal.saveRequested = false
				root.onSave()
			}
		}
	}

	function onStartUpdateRepresentation(documentId, representation){
		if (root.documentId !== documentId){
			return
		}

		for (let i = 0; i < registeredViews.length; ++i){
			if (registeredViews[i].model === representation){
				registeredViews[i].setBlockingUpdateModel(true)
				_internal.updateCounters[i] = _internal.updateCounters[i] + 1
				break
			}
		}

		documentManager.startUpdateRepresentation(documentId, representation)
	}

	function onRepresentationUpdated(documentId, representation){
		if (root.documentId !== documentId){
			return
		}

		for (let i = 0; i < registeredViews.length; ++i){
			if (registeredViews[i].model === representation){
				_internal.updateCounters[i] = _internal.updateCounters[i] - 1
				if (_internal.updateCounters[i] <= 0){
					_internal.updateCounters[i] = 0
					registeredViews[i].setBlockingUpdateModel(false)
				}

				registeredViews[i].doUpdateGui()
				break
			}
		}

		documentManager.documentRepresentationUpdated(documentId, representation)
	}

	function onUpdateRepresentationFailed(documentId, message){
		if (root.documentId !== documentId){
			return
		}

		for (let i = 0; i < registeredViews.length; ++i){
			_internal.updateCounters[i] = 0
			registeredViews[i].setBlockingUpdateModel(false)
		}

		documentManager.updateRepresentationFailed(documentId, message)
	}

	function onUpdateDocumentFailed(documentId, message){
		if (root.documentId !== documentId){
			return
		}

		documentManager.updateDocumentFailed(documentId, message)
	}

	function onGuiUpdated(view, model){
		if (registeredViews.includes(view)){
			documentManager.documentGuiUpdated(documentId, model)
		}
	}

	function onGuiVisibleChanged(view, visible){
		if (registeredViews.includes(view)){
			if (visible && _internal.requestUpdateViews.includes(view)){
				let index = registeredViews.indexOf(view)

				registeredRepresentation[index].updateRepresentationFromDocument()
				
				let viewIndex = _internal.requestUpdateViews.indexOf(view)
				if (viewIndex >= 0){
					_internal.requestUpdateViews.splice(viewIndex, 1)
				}
			}
		}
	}

	function onModelDataChanged(view, model){
		if (registeredViews.includes(view)){
			let index = registeredViews.indexOf(view)
			if (_internal.updateCounters[index] > 0){
				// Change originates from representation load (updateRepresentationFromDocument),
				// must not trigger reverse updateDocumentFromRepresentation.
				return
			}
			_internal.initiatingView = view
			registeredRepresentation[index].updateDocumentFromRepresentation()
		}
	}

	function registerView(view, representationController, updateRepr){
		if (!view){
			console.error("Unable to register invalid view")
			return
		}

		if (!representationController){
			console.error("Unable to register view with invalid representation controller")
			return
		}

		registeredViews.push(view)
		registeredRepresentation.push(representationController)
		_internal.updateCounters.push(0)
		
		viewRegistered(view, representationController, updateRepr)
	}

	function onUndo(){
		if (!documentManager){
			console.error("Unable to handle Undo command. Error: Document manager is invalid")
			return
		}

		documentManager.doUndo(documentId, 1)
	}

	function onRedo(){
		if (!documentManager){
			console.error("Unable to handle Redo command. Error: Document manager is invalid")
			return
		}

		documentManager.doRedo(documentId, 1)
	}

	function onSave(){
		if (!documentManager){
			console.error("Unable to handle Save command. Error: Document manager is invalid")
			return
		}

		if (documentManager.hasDocumentNameProvider(documentTypeId)){
			documentManager.saveDocument(documentId, "")
		}
		else if (documentName.length === 0){
			_internal.saveRequested = true
			documentManager.requestDocumentName(documentId, documentTypeId)
		}
		else{
			documentManager.saveDocument(documentId, documentName)
		}
	}

	function updateRepresentationForAllViews(){
		let skipView = _internal.initiatingView
		_internal.initiatingView = null

		for (let i = 0; i < registeredViews.length; ++i){
			if (registeredViews[i] === skipView){
				continue
			}

			if (registeredViews[i].visible){
				registeredRepresentation[i].updateRepresentationFromDocument()
			}
			else{
				if (!_internal.requestUpdateViews.includes(registeredViews[i])){
					_internal.requestUpdateViews.push(registeredViews[i])
				}
			}
		}
	}

	function updateDocumentForAllViews(){
		for (let i = 0; i < registeredViews.length; ++i){
			registeredViews[i].setBlockingUpdateModel(true)
			registeredRepresentation[i].updateDocumentFromRepresentation()
		}
	}

	property QtObject _internal: QtObject {
		property var requestUpdateViews: []
		property bool saveRequested: false
		property var updateCounters: []
		property var initiatingView: null
	}
}
