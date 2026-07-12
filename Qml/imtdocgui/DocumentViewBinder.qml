import QtQuick 2.12

// Wires exactly one (view, representationController) pair to its Document
// and to the owning DocumentService. Replaces DocumentDecorator, which used
// to manage every registered view of a document through parallel arrays
// indexed by position. One binder = one pair; disposal is explicit.
QtObject {
	id: root

	required property var documentService
	required property var document
	required property Item view
	required property QtObject controller

	readonly property string documentId: document ? document.documentId : ""

	property ModelSyncController sync: ModelSyncController {
		view: root.view
		controller: root.controller

		onPushStarted: {
			root.document.__lastPushingBinder = root
		}
	}

	function initialize(pullInitialRepresentation){
		view.setBlockingUpdateModel(true)
		view.model = controller.representationModel

		if (view.commandsController){
			view.commandsController.setCommandIsEnabled("Save", document.isDirty)
			view.commandsController.setCommandIsEnabled("Undo", document.undoSteps > 0)
			view.commandsController.setCommandIsEnabled("Redo", document.redoSteps > 0)
		}

		if (view.objectName === "DocumentViewBase" && view.representationController !== undefined){
			view.representationController = controller
		}

		if (pullInitialRepresentation){
			sync.pull()
		}
		else if (document.isNew){
			sync.paintFromCurrentModel()
		}
	}

	// Re-pulls the representation into this view, unless it is the view
	// that just pushed the change which produced this broadcast (avoids an
	// unnecessary round-trip back into the view that originated the edit).
	function pullUnlessInitiator(){
		if (document.__lastPushingBinder === root){
			document.__lastPushingBinder = null
			return
		}

		sync.pull()
	}

	function dispose(){
		if (typeof view.documentClosing === "function"){
			view.documentClosing()
		}

		let index = document.binders.indexOf(root)
		if (index >= 0){
			document.binders.splice(index, 1)
		}

		root.destroy()
	}

	Connections {
		target: root.view

		function onCommandActivated(commandId){
			if (commandId === "Undo"){
				root.documentService.doUndo(root.documentId, 1)
			}
			else if (commandId === "Redo"){
				root.documentService.doRedo(root.documentId, 1)
			}
			else if (commandId === "Save"){
				root.documentService.save(root.documentId)
			}
			else if (commandId === "Close"){
				root.documentService.closeDocument(root.documentId)
			}
		}

		function onModelDataChanged(){
			root.sync.onViewModelChanged()
		}

		function onGuiUpdated(view, model){
			root.documentService.documentGuiUpdated(root.documentId, model)
		}

		function onGuiVisibleChanged(view, visible){
			if (visible){
				root.sync.onViewBecameVisible()
			}
		}
	}

	Connections {
		target: root.controller

		function onStartUpdateRepresentation(){
			root.document.beginSync()
		}

		function onRepresentationUpdated(){
			root.document.endSync()
			root.sync.onRepresentationUpdated()
		}

		function onUpdateRepresentationFailed(documentId, message){
			root.document.endSync()
			root.sync.onRepresentationUpdateFailed()
			root.documentService.updateRepresentationFailed(root.documentId, message)
		}

		function onUpdateDocumentFailed(documentId, message){
			root.documentService.updateDocumentFailed(root.documentId, message)
		}
	}

	Connections {
		target: root.document

		function onIsDirtyChanged(){
			if (root.view.commandsController){
				root.view.commandsController.setCommandIsEnabled("Save", root.document.isDirty)
			}
		}

		function onUndoStepsChanged(){
			if (root.view.commandsController){
				root.view.commandsController.setCommandIsEnabled("Undo", root.document.undoSteps > 0)
			}
		}

		function onRedoStepsChanged(){
			if (root.view.commandsController){
				root.view.commandsController.setCommandIsEnabled("Redo", root.document.redoSteps > 0)
			}
		}
	}

	Connections {
		target: root.documentService

		function onDocumentChangedRemotely(documentId){
			if (documentId === root.documentId){
				root.pullUnlessInitiator()
			}
		}

		function onDocumentReady(documentId){
			if (documentId === root.documentId && typeof root.view.documentReady === "function"){
				root.view.documentReady()
			}
		}

		function onDocumentSaved(documentId){
			if (documentId === root.documentId && typeof root.view.documentSaved === "function"){
				root.view.documentSaved()
			}
		}
	}
}
