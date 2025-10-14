import QtQuick 2.12
import imtgui 1.0
import imtbaseCollectionDocumentManagerSdl 1.0

QtObject {
	id: root

	property string documentId
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
			else{
				representationController.representationUpdated(documentId, view.model)
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

		function onUndoDone(documentId){
			if (documentId !== root.documentId){
				return
			}

			root.updateRepresentationForAllViews()
		}

		function onRedoDone(documentId){
			if (documentId !== root.documentId){
				return
			}

			root.updateRepresentationForAllViews()
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

		function onDocumentManagerChanged(typeOperation, objectId, documentId, hasChanges){
			if (documentId !== root.documentId){
				return
			}

			if (typeOperation === EDocumentOperationEnum.s_documentChanged){
				root.updateRepresentationForAllViews()
			}
		}
	}

	function onRepresentationUpdated(documentId, representation){
		if (root.documentId !== documentId){
			return
		}

		for (let i = 0; i < registeredViews.length; ++i){
			if (registeredViews[i].model === representation){
				registeredViews[i].setBlockingUpdateModel(false)
				
				registeredViews[i].doUpdateGui()
				return
			}
		}

		documentManager.documentRepresentationUpdated(documentId, representation)
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
				
				view.setBlockingUpdateModel(true)
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

		documentManager.saveDocument(documentId)
	}

	function updateRepresentationForAllViews(){
		for (let i = 0; i < registeredViews.length; ++i){
			if (registeredViews[i].visible){
				registeredViews[i].setBlockingUpdateModel(true)
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
	}
}
