import QtQuick 2.12
import imtgui 1.0
import imtbaseCollectionDocumentManagerSdl 1.0

QtObject {
	id: root

	property string documentId
	property string documentTypeId
	property DocumentManagerBase documentManager: null

	property var registeredViews: []
	property var registeredRepresentation: []

	signal viewRegistered(var view, var representationController)

	onViewRegistered: {
		if (view){
			view.setBlockingUpdateModel(true)
			view.model = representationController.representationModel
			view.commandActivated.connect(onCommandActivated)
			view.modelDataChanged.connect(onModelDataChanged)

			representationController.representationUpdated.connect(onRepresentationUpdated)
			representationController.updateRepresentationFromDocument()
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

	Connections {
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

		function onUndoInfoReceived(documentId, availableUndoSteps, availableRedoSteps){
			if (documentId !== root.documentId){
				return
			}

			for (let i = 0; i < root.registeredViews.length; ++i){
				if (root.registeredViews[i].commandsController){
					root.registeredViews[i].commandsController.setCommandIsEnabled("Undo", availableUndoSteps > 0)
					root.registeredViews[i].commandsController.setCommandIsEnabled("Redo", availableRedoSteps > 0)
				}
			}
		}

		function onDocumentManagerChanged(typeOperation, objectId, documentId, hasChanges){
			if (documentId !== root.documentId){
				return
			}

			for (let i = 0; i < root.registeredViews.length; ++i){
				if (root.registeredViews[i].commandsController){
					root.registeredViews[i].commandsController.setCommandIsEnabled("Save", hasChanges)
				}
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
	
	function onModelDataChanged(view, model){
		console.log("onModelDataChanged", view, model)
		if (registeredViews.includes(view)){
			let index = registeredViews.indexOf(view)
			registeredRepresentation[index].updateDocumentFromRepresentation()
		}
	}

	function registerView(view, representationController){
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
		
		viewRegistered(view, representationController)
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
		console.log("updateRepresentationForAllViews", documentId, representation)
		
		for (let i = 0; i < registeredViews.length; ++i){
			registeredViews[i].setBlockingUpdateModel(true)
			registeredRepresentation[i].updateRepresentationFromDocument()
		}
	}

	function updateDocumentForAllViews(){
		for (let i = 0; i < registeredViews.length; ++i){
			registeredViews[i].setBlockingUpdateModel(true)
			registeredRepresentation[i].updateDocumentFromRepresentation()
		}
	}
}
