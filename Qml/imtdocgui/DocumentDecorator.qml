import QtQuick 2.12
import imtcontrols 1.0
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

			representationController.updateDocumentFailed.connect(onUpdateDocumentFailed)

			// The signals of the controller are bound to its view: its representation model may be replaced by a read
			let registeredView = view
			let connection = {
				"model": representationController.representationModel,
				"representationStarted": function(startedDocumentId, representation){
					root.onStartUpdateRepresentation(registeredView, startedDocumentId, representation)
				},
				"representationUpdated": function(updatedDocumentId, representation){
					root.onRepresentationUpdated(registeredView, updatedDocumentId, representation)
				},
				"representationFailed": function(failedDocumentId, message){
					root.onUpdateRepresentationFailed(registeredView, failedDocumentId, message)
				},
				"viewDestroyed": function(){
					root.unregisterView(registeredView)
				},
				"updateStarted": function(startedDocumentId){
					root.onOwnUpdateStarted(registeredView, startedDocumentId)
				},
				"updateDone": function(updatedDocumentId){
					root.onOwnUpdateAnswered(registeredView, updatedDocumentId, true)
				},
				"updateFailed": function(failedDocumentId, message){
					root.onOwnUpdateAnswered(registeredView, failedDocumentId, false)
				}
			}
			representationController.startUpdateRepresentation.connect(connection.representationStarted)
			representationController.representationUpdated.connect(connection.representationUpdated)
			representationController.updateRepresentationFailed.connect(connection.representationFailed)
			representationController.startUpdateDocument.connect(connection.updateStarted)
			representationController.documentUpdated.connect(connection.updateDone)
			representationController.updateDocumentFailed.connect(connection.updateFailed)
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

		function onDocumentManagerChanged(typeOperation, objectId, documentId, documentName, changeId){
			if (documentId !== root.documentId){
				return
			}

			// A document being closed is not read again
			if (typeOperation === EDocumentOperationEnum.s_documentChanged && !root.documentManager.documentIsClosing(documentId)){
				root.onDocumentChanged(changeId)
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

	function onStartUpdateRepresentation(view, documentId, representation){
		if (root.documentId !== documentId){
			return
		}

		let index = registeredViews.indexOf(view)
		if (index >= 0){
			view.setBlockingUpdateModel(true)
			_internal.updateCounters[index] = _internal.updateCounters[index] + 1
			_internal.failedViews[index] = false
		}

		documentManager.startUpdateRepresentation(documentId, representation)
	}

	// A read may give the view a new model object: the edits queued on the old one are dropped with it,
	// the new model holds what the server has
	function onRepresentationUpdated(view, documentId, representation){
		if (root.documentId !== documentId){
			return
		}

		let index = registeredViews.indexOf(view)
		if (index >= 0){
			let connection = _internal.viewConnections[index]
			if (connection.model !== representation){
				connection.model = representation
				_internal.queuedUpdates[index] = null
			}
			if (view.model !== representation){
				view.model = representation
			}

			_internal.updateCounters[index] = _internal.updateCounters[index] - 1
			if (_internal.updateCounters[index] <= 0){
				_internal.updateCounters[index] = 0
				view.setBlockingUpdateModel(false)
			}

			view.doUpdateGui()
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
			if (typeof view.showContent === "function"){
				view.showContent()
			}
		}

		documentManager.updateRepresentationFailed(documentId, message)
	}

	function onUpdateDocumentFailed(documentId, message){
		if (root.documentId !== documentId){
			return
		}

		documentManager.updateDocumentFailed(documentId, message)
	}

	// A visible view refuses the save while its input is not acceptable (acceptsSave() is optional).
	function viewsAcceptSave(){
		for (let i = 0; i < registeredViews.length; ++i){
			let view = registeredViews[i]
			if (view.visible && typeof view.acceptsSave === "function" && !view.acceptsSave()){
				return false
			}
		}

		return true
	}

	// Writes the GUI state of the visible views to the document.
	function commitChanges(){
		for (let i = 0; i < registeredViews.length; ++i){
			if (registeredViews[i].visible){
				registeredViews[i].doUpdateModel()
			}
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

	function onModelDataChanged(view, model, changeSet){
		if (registeredViews.includes(view)){
			let index = registeredViews.indexOf(view)
			if (_internal.updateCounters[index] > 0){
				// Change originates from representation load (updateRepresentationFromDocument),
				// must not trigger reverse updateDocumentFromRepresentation.
				return
			}
			root.requestUpdate(index, changeSet)
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
		_internal.ownUpdates.push([])
		_internal.deferredViews.push(false)
		_internal.queuedUpdates.push(null)
		
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

		representationController.startUpdateRepresentation.disconnect(connection.representationStarted)
		representationController.representationUpdated.disconnect(connection.representationUpdated)
		representationController.updateRepresentationFailed.disconnect(connection.representationFailed)
		representationController.updateDocumentFailed.disconnect(onUpdateDocumentFailed)
		representationController.startUpdateDocument.disconnect(connection.updateStarted)
		representationController.documentUpdated.disconnect(connection.updateDone)
		representationController.updateDocumentFailed.disconnect(connection.updateFailed)

		registeredViews.splice(index, 1)
		registeredRepresentation.splice(index, 1)
		_internal.updateCounters.splice(index, 1)
		_internal.failedViews.splice(index, 1)
		_internal.viewConnections.splice(index, 1)
		_internal.ownUpdates.splice(index, 1)
		_internal.deferredViews.splice(index, 1)
		_internal.queuedUpdates.splice(index, 1)

		let requestIndex = _internal.requestUpdateViews.indexOf(view)
		if (requestIndex >= 0){
			_internal.requestUpdateViews.splice(requestIndex, 1)
		}

		viewUnregistered(view)
	}

	function onUndo(){
		if (!documentManager){
			console.error("Unable to handle Undo command. Error: Document manager is invalid")
			return
		}

		runAfterUpdates("Undo")
	}

	function onRedo(){
		if (!documentManager){
			console.error("Unable to handle Redo command. Error: Document manager is invalid")
			return
		}

		runAfterUpdates("Redo")
	}

	function onSave(){
		if (!documentManager){
			console.error("Unable to handle Save command. Error: Document manager is invalid")
			return
		}

		if (!viewsAcceptSave()){
			return
		}

		commitChanges()
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

	// Reads every view again, e.g. after a reconnect; a view with own edits pending is read after they are applied.
	function updateRepresentationForAllViews(){
		for (let i = 0; i < registeredViews.length; ++i){
			if (hasPendingEdits(i)){
				_internal.deferredViews[i] = true
			}
			else{
				_internal.deferredViews[i] = false
				updateRepresentation(i)
			}
		}
	}

	// A view sends one update at a time: an edit made while its update is unanswered is sent after the answer,
	// together with the other edits made meanwhile. So an answer always belongs to the one update in flight.
	function requestUpdate(viewIndex, changeSet){
		if (!hasUnansweredUpdate(viewIndex)){
			sendUpdate(viewIndex, changeSet)

			return
		}

		// An edit that does not tell what changed makes the whole queued update unknown
		let queued = _internal.queuedUpdates[viewIndex]
		let isKnown = changeSet && changeSet.length > 0 && (queued === null || queued.length > 0)
		_internal.queuedUpdates[viewIndex] = isKnown ? (queued === null ? changeSet : queued.concat(changeSet)) : []
	}

	function sendQueuedUpdate(viewIndex){
		let queued = _internal.queuedUpdates[viewIndex]
		if (queued === null || hasUnansweredUpdate(viewIndex)){
			return
		}

		_internal.queuedUpdates[viewIndex] = null
		sendUpdate(viewIndex, queued)
	}

	// Sends the representation of a view to the server with a new change-ID: the update request of the controller carries it
	// (DocumentUpdateRequestSender), the server returns it in the DocumentChanged of this update.
	function sendUpdate(viewIndex, changeSet){
		let controller = registeredRepresentation[viewIndex]
		let previousChangeId = controller.changeId
		let previousStartingChangeId = _internal.startingChangeId
		let changeId = UuidGenerator.generateUUID()
		_internal.startingChangeId = changeId
		controller.changeId = changeId
		try {
			controller.updateDocumentFromRepresentation(changeSet)
		}
		finally {
			// Also when the controller throws: later requests must not carry this ID
			controller.changeId = previousChangeId
			_internal.startingChangeId = previousStartingChangeId
		}
	}

	// The document was changed on the server. The notification of an own update (its changeId) is skipped by its view,
	// unless its controller sets requestRepresentationOnUpdate. A view with an update in flight is read after the answer.
	function onDocumentChanged(changeId){
		let now = Date.now()
		for (let i = 0; i < registeredViews.length; ++i){
			pruneOwnUpdates(i, now)

			let ownIndex = findOwnUpdate(i, changeId)
			if (ownIndex >= 0){
				// The server holds this view's change now: a read put off until the answer can run
				let update = _internal.ownUpdates[i][ownIndex]
				update.echoed = true
				if (update.expectsEcho){
					flushDeferredView(i)
					continue
				}
			}

			if (hasPendingEdits(i)){
				_internal.deferredViews[i] = true
				continue
			}

			updateRepresentation(i)
		}
	}

	function onOwnUpdateStarted(view, startedDocumentId){
		let index = registeredViews.indexOf(view)
		if (startedDocumentId !== root.documentId || index < 0){
			return
		}

		let expectsEcho = registeredRepresentation[index].requestRepresentationOnUpdate !== true
		_internal.ownUpdates[index].push({"changeId": _internal.startingChangeId, "startedAt": Date.now(), "answered": false, "answeredAt": 0, "echoed": false, "expectsEcho": expectsEcho})
		_internal.ownUpdatesTimer.start()
	}

	// A view has one update without an answer at a time (see requestUpdate()), so the answer is for it.
	// A failed update leaves the document as the server has it, so its notification is not skipped.
	// After a refusal a controller with readBackOnRefusal reads the view again and drops the edits queued meanwhile:
	// they are made over the refused change. Otherwise the queued edits are sent.
	// The queued update and the read go after the answer is handled, not from inside the handler of the request.
	function onOwnUpdateAnswered(view, answeredDocumentId, succeeded){
		let index = registeredViews.indexOf(view)
		if (answeredDocumentId !== root.documentId || index < 0){
			return
		}

		let updates = _internal.ownUpdates[index]
		for (let k = 0; k < updates.length; ++k){
			if (!updates[k].answered){
				updates[k].answered = true
				updates[k].answeredAt = Date.now()
				if (!succeeded){
					updates[k].expectsEcho = false
				}
				break
			}
		}

		if (!succeeded && registeredRepresentation[index].readBackOnRefusal){
			_internal.queuedUpdates[index] = null
			_internal.deferredViews[index] = true
		}

		if (!succeeded){
			cancelPendingCommands()
		}

		pruneOwnUpdates(index, Date.now())
		_internal.answeredUpdatesTimer.start()
	}

	// Sends the queued updates and the reads put off that the answers released
	function continueAnsweredUpdates(){
		for (let i = 0; i < registeredViews.length; ++i){
			sendQueuedUpdate(i)
			flushDeferredView(i)
		}

		continuePendingCommands()
	}

	// Drops finished updates, echoes that did not come in ownEchoTimeout and answers lost for ownUpdateTimeout.
	function pruneOwnUpdates(viewIndex, now){
		let updates = _internal.ownUpdates[viewIndex]
		let kept = []
		for (let k = 0; k < updates.length; ++k){
			let update = updates[k]
			if (!update.answered){
				if (now - update.startedAt <= root.ownUpdateTimeout){
					kept.push(update)
				}
			}
			else if (update.expectsEcho && !update.echoed && now - update.answeredAt <= root.ownEchoTimeout){
				kept.push(update)
			}
		}

		_internal.ownUpdates[viewIndex] = kept
	}

	function findOwnUpdate(viewIndex, changeId){
		if (!changeId){
			return -1
		}

		let updates = _internal.ownUpdates[viewIndex]
		for (let k = 0; k < updates.length; ++k){
			if (updates[k].changeId === changeId){
				return k
			}
		}

		return -1
	}

	function hasUnansweredUpdate(viewIndex){
		let updates = _internal.ownUpdates[viewIndex]
		for (let k = 0; k < updates.length; ++k){
			if (!updates[k].answered){
				return true
			}
		}

		return false
	}

	// Sent and not yet applied: neither its answer nor its notification has come.
	function hasUpdateInFlight(viewIndex){
		let updates = _internal.ownUpdates[viewIndex]
		for (let k = 0; k < updates.length; ++k){
			if (!updates[k].answered && !updates[k].echoed){
				return true
			}
		}

		return false
	}

	// Own edits not applied yet: an update in flight or edits queued behind it. A read now would replace them:
	// its answer may come before the server applies them, and a new model drops the queued edits.
	function hasPendingEdits(viewIndex){
		return hasUpdateInFlight(viewIndex) || _internal.queuedUpdates[viewIndex] !== null
	}

	// A view or a queue waits for an answer
	function hasPendingUpdates(){
		for (let i = 0; i < registeredViews.length; ++i){
			if (hasUnansweredUpdate(i) || _internal.queuedUpdates[i] !== null){
				return true
			}
		}

		return false
	}

	// A save asked while updates are pending runs once they are answered: the server saves what the views hold.
	// A refused update cancels it (saveDocumentFailed), so a save-and-close does not close.
	function saveAfterUpdates(documentName){
		_internal.pendingCommands.push({"command": "Save", "documentName": documentName || ""})
	}

	// Save, Undo and Redo act on what the server holds: asked while own edits are pending, they wait for them, in their order.
	// An undo sent before a queued edit would undo the step before it, and the edit, which sends the whole view, would redo it.
	function runAfterUpdates(command){
		if (hasPendingUpdates()){
			_internal.pendingCommands.push({"command": command, "documentName": ""})
			return
		}

		runCommand(command, "")
	}

	function runCommand(command, documentName){
		if (command === "Save"){
			documentManager.saveDocument(root.documentId, documentName)
		}
		else if (command === "Undo"){
			documentManager.doUndo(root.documentId, 1)
		}
		else if (command === "Redo"){
			documentManager.doRedo(root.documentId, 1)
		}
	}

	function continuePendingCommands(){
		if (_internal.pendingCommands.length === 0 || hasPendingUpdates()){
			return
		}

		let commands = _internal.pendingCommands
		_internal.pendingCommands = []
		for (let i = 0; i < commands.length; ++i){
			runCommand(commands[i].command, commands[i].documentName)
		}
	}

	// A refused update leaves the views other than the server has them: the commands waiting for it are dropped
	function cancelPendingCommands(){
		let commands = _internal.pendingCommands
		_internal.pendingCommands = []
		for (let i = 0; i < commands.length; ++i){
			if (commands[i].command === "Save"){
				documentManager.saveDocumentFailed(root.documentId, qsTr("The document was not saved: a change was not accepted"))
				return
			}
		}
	}

	function flushDeferredView(viewIndex){
		if (_internal.deferredViews[viewIndex] && !hasPendingEdits(viewIndex)){
			_internal.deferredViews[viewIndex] = false
			updateRepresentation(viewIndex)
		}
	}

	// Lost answers expire only with time, so the put off reads are checked while updates are tracked.
	function checkOwnUpdates(){
		let now = Date.now()
		let tracked = false
		for (let i = 0; i < registeredViews.length; ++i){
			pruneOwnUpdates(i, now)
			sendQueuedUpdate(i)
			flushDeferredView(i)
			if (_internal.ownUpdates[i].length > 0){
				tracked = true
			}
		}

		continuePendingCommands()

		if (!tracked){
			_internal.ownUpdatesTimer.stop()
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
			root.requestUpdate(i, [])
		}
	}

	// How long after its answer an own update is kept for its notification: updates go over HTTP,
	// notifications over the WebSocket, so either can come first. A later notification only reads the view once more.
	property int ownEchoTimeout: 3000

	// An update without an answer for this long counts as answered, so the views it holds back are read
	property int ownUpdateTimeout: 30000

	property QtObject _internal: QtObject {
		property var requestUpdateViews: []
		property bool saveRequested: false
		property var updateCounters: []
		property var failedViews: []
		property var viewConnections: []
		property var ownUpdates: [] // Per view: [{changeId, startedAt, answered, answeredAt, echoed, expectsEcho}]
		property string startingChangeId: "" // Of the update being sent, see sendUpdate()
		property var deferredViews: [] // Per view: read once its updates in flight are answered
		property var queuedUpdates: [] // Per view: changeSet of the edits waiting for the answer to its update, or null
		property var pendingCommands: [] // Save, Undo and Redo waiting for the pending updates: [{command, documentName}], see runAfterUpdates()

		property Timer answeredUpdatesTimer: Timer {
			interval: 0
			repeat: false
			onTriggered: {
				root.continueAnsweredUpdates()
			}
		}

		property Timer ownUpdatesTimer: Timer {
			interval: 1000
			repeat: true
			onTriggered: {
				root.checkOwnUpdates()
			}
		}
	}
}
