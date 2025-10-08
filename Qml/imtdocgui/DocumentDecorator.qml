import QtQuick 2.12
import imtgui 1.0

QtObject {
	id: root

	property string documentId
	property string documentTypeId
	property DocumentManagerBase documentManager: null
	property var registeredViews: ({})

	signal viewRegistered(var view, var representationController)

	onViewRegistered: {
		if (view){
			view.commandActivated.connect(onCommandActivated)
			view.modelDataChanged.connect(onModelDataChanged)
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
		function onUndoInfoReceived(documentId, availableUndoSteps, availableRedoSteps){
			let documentView = target.getDocumentViewInstance(documentId)
			if (documentView){
				documentView.commandsController.setCommandIsEnabled("Undo", availableUndoSteps > 0)
				documentView.commandsController.setCommandIsEnabled("Redo", availableRedoSteps > 0)
			}
		}

		function onDocumentModelChanged(documentId, changeSet){
			let documentView = target.getDocumentViewInstance(documentId)
			if (documentView){
				documentView.commandsController.setCommandIsEnabled("Save", isDirty)
			}
		}
	}

	function onModelDataChanged(view, model){
		console.log("onModelDataChanged", view)
		if (view in registeredViews){
			registeredViews[view].updateDocumentFromRepresentation()
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

		registeredViews[view] = representationController
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

	function updateRepresentationFromDocument(viewTypeId){
		if (viewTypeId in registeredViews){
			registeredViews[viewTypeId].updateRepresentationFromDocument()
		}
		else{
			console.error("Unable to update representation for view: '" +viewTypeId+"'. Error: Document data controller not registered")
		}
	}

	function updateDocumentFromRepresentation(viewTypeId){
		if (viewTypeId in registeredViews){
			registeredViews[viewTypeId].updateDocumentFromRepresentation()
		}
		else{
			console.error("Unable to update document from view: '" +viewTypeId+"'. Error: Document data controller not registered")
		}
	}
}
