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
	signal viewUnregistered(var view)
	signal viewVisibilityChanged(var view, bool visible)

	Component.onDestruction: {
		while (registeredViews.length > 0){
			unregisterView(registeredViews[registeredViews.length - 1])
		}
	}

	onViewRegistered: {
		if (view){
			if (view.objectName === "DocumentViewBase"){
				if (view.documentManager !== undefined){
					view.documentId = documentId
					view.documentTypeId = documentTypeId
					view.documentManager = documentManager
				}
			}

			view.setBlockingUpdateModel(true)
			view.model = representationController.representationModel
			view.commandActivated.connect(onCommandActivated)
			view.modelDataChanged.connect(onModelDataChanged)
			view.guiUpdated.connect(onGuiUpdated)
			view.guiVisibleChanged.connect(onGuiVisibleChanged)

			representationController.representationUpdated.connect(onRepresentationUpdated)
			representationController.startUpdateRepresentation.connect(onStartUpdateRepresentation)
			representationController.updateDocumentFailed.connect(onUpdateDocumentFailed)
			representationController.startUpdateDocument.connect(onStartUpdateDocument)
			representationController.documentUpdated.connect(onDocumentUpdated)

			// The failure signal does not say which representation failed, so it is bound to its view.
			let registeredView = view
			let connection = {
				"representationFailed": function(failedDocumentId, message){
					root.onUpdateRepresentationFailed(registeredView, failedDocumentId, message)
				},
				"viewDestroyed": function(){
					root.unregisterView(registeredView)
				}
			}
			representationController.updateRepresentationFailed.connect(connection.representationFailed)
			view.Component.destruction.connect(connection.viewDestroyed)
			_internal.viewConnections.push(connection)

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
				_internal.failedViews[i] = false
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

	function onUpdateRepresentationFailed(view, documentId, message){
		if (root.documentId !== documentId){
			return
		}

		let index = registeredViews.indexOf(view)
		if (index >= 0){
			_internal.updateCounters[index] = 0
			_internal.failedViews[index] = true
			view.setBlockingUpdateModel(false)
		}

		documentManager.updateRepresentationFailed(documentId, message)
	}

	function onUpdateDocumentFailed(documentId, message){
		if (root.documentId !== documentId){
			return
		}

		_internal.pendingDocumentUpdates = Math.max(0, _internal.pendingDocumentUpdates - 1)
		_internal.saveAfterUpdate = false

		documentManager.updateDocumentFailed(documentId, message)
	}

	function onStartUpdateDocument(documentId){
		if (root.documentId !== documentId){
			return
		}

		_internal.pendingDocumentUpdates = _internal.pendingDocumentUpdates + 1
	}

	function onDocumentUpdated(documentId){
		if (root.documentId !== documentId){
			return
		}

		_internal.pendingDocumentUpdates = Math.max(0, _internal.pendingDocumentUpdates - 1)
		if (_internal.pendingDocumentUpdates === 0 && _internal.saveAfterUpdate){
			_internal.saveAfterUpdate = false
			root.doSave()
		}
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

			viewVisibilityChanged(view, visible)
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
		_internal.failedViews.push(false)
		
		viewRegistered(view, representationController, updateRepr)
	}

	function unregisterView(view){
		let index = registeredViews.indexOf(view)
		if (index < 0){
			return
		}

		let representationController = registeredRepresentation[index]
		let connection = _internal.viewConnections[index]

		view.commandActivated.disconnect(onCommandActivated)
		view.modelDataChanged.disconnect(onModelDataChanged)
		view.guiUpdated.disconnect(onGuiUpdated)
		view.guiVisibleChanged.disconnect(onGuiVisibleChanged)
		view.Component.destruction.disconnect(connection.viewDestroyed)

		representationController.representationUpdated.disconnect(onRepresentationUpdated)
		representationController.startUpdateRepresentation.disconnect(onStartUpdateRepresentation)
		representationController.updateRepresentationFailed.disconnect(connection.representationFailed)
		representationController.updateDocumentFailed.disconnect(onUpdateDocumentFailed)
		representationController.startUpdateDocument.disconnect(onStartUpdateDocument)
		representationController.documentUpdated.disconnect(onDocumentUpdated)

		registeredViews.splice(index, 1)
		registeredRepresentation.splice(index, 1)
		_internal.updateCounters.splice(index, 1)
		_internal.failedViews.splice(index, 1)
		_internal.viewConnections.splice(index, 1)

		let requestIndex = _internal.requestUpdateViews.indexOf(view)
		if (requestIndex >= 0){
			_internal.requestUpdateViews.splice(requestIndex, 1)
		}

		if (_internal.initiatingView === view){
			_internal.initiatingView = null
		}

		viewUnregistered(view)
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

		// A repeated Save while still waiting is the way out of an update that never reported back.
		if (_internal.saveAfterUpdate){
			_internal.saveAfterUpdate = false
			_internal.pendingDocumentUpdates = 0
			doSave()
			return
		}

		for (let i = 0; i < registeredViews.length; ++i){
			if (registeredViews[i].visible){
				registeredViews[i].doUpdateModel()
			}
		}

		if (_internal.pendingDocumentUpdates > 0){
			_internal.saveAfterUpdate = true
			return
		}

		doSave()
	}

	function doSave(){
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
			if (registeredViews[i] !== skipView){
				updateRepresentation(i)
			}
		}
	}

	// Hidden views are updated when they become visible.
	function updateRepresentation(viewIndex){
		let view = registeredViews[viewIndex]
		if (view.visible){
			registeredRepresentation[viewIndex].updateRepresentationFromDocument()
		}
		else if (!_internal.requestUpdateViews.includes(view)){
			_internal.requestUpdateViews.push(view)
		}
	}

	function releaseView(viewIndex){
		if (_internal.updateCounters[viewIndex] <= 0){
			registeredViews[viewIndex].setBlockingUpdateModel(false)
		}
		registeredViews[viewIndex].doUpdateGui()
	}

	function representationRequired(viewIndex, isNewDocument){
		return !isNewDocument || registeredRepresentation[viewIndex].requestRepresentationOnCreate
	}

	// Hidden views count only while no view is visible: their representation is requested once shown.
	function isAwaitingRepresentation(isNewDocument){
		let hasVisibleView = false
		for (let i = 0; i < registeredViews.length; ++i){
			if (registeredViews[i].visible){
				hasVisibleView = true
				if (representationRequired(i, isNewDocument)){
					return true
				}
			}
		}

		if (hasVisibleView){
			return false
		}

		for (let i = 0; i < registeredViews.length; ++i){
			if (representationRequired(i, isNewDocument)){
				return true
			}
		}

		return false
	}

	function isUpdatingRepresentation(){
		for (let i = 0; i < _internal.updateCounters.length; ++i){
			if (_internal.updateCounters[i] > 0){
				return true
			}
		}

		return false
	}

	// Cleared for a view when its representation is requested again.
	function hasFailedRepresentation(){
		return _internal.failedViews.includes(true)
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
		property var failedViews: []
		property var viewConnections: []
		property var initiatingView: null
		property int pendingDocumentUpdates: 0
		property bool saveAfterUpdate: false
	}
}
