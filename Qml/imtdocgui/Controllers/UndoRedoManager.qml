import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

// Undo history for a document model.
//
// A step is a full JSON snapshot of the model, which is the expensive part: it
// walks every node and enumerates every property of each one. Two things keep
// that affordable.
//
// The snapshot is taken once a burst of edits has stopped, not on every change.
// modelChanged travels up from any property write anywhere in the document, so
// typing a name used to serialise the whole document once per character. Undo
// does not want a step per character either, so the wait costs nothing and is
// flushed the moment anything actually needs the history.
//
// And the stack has a lid. Snapshots are strings the size of the document, kept
// for as long as the tab is open, and nothing ever dropped the oldest.
Item {
	id: undoRedoManager

	property bool autoTracking: true // automatic tracking of model changes

	// How long the edits have to stop before a step is recorded.
	property int snapshotDelay: 250
	// Oldest steps are dropped past this. Zero keeps everything.
	property int maxUndoSteps: 50

	signal modelChanged()
	signal undo()
	signal redo()

	function getCurrentStateModel()
	{
		if (internal.m_undoStack.length > 0){
			return internal.m_undoStack[internal.m_undoStack.length - 1]
		}

		return null
	}


	function getAvailableUndoSteps()
	{
		return internal.m_undoStack.length
	}


	function getAvailableRedoSteps()
	{
		return internal.m_redoStack.length
	}


	function resetUndo()
	{
		snapshotTimer.stop()
		internal.m_pendingSnapshot = false

		internal.m_undoStack = []
		internal.m_redoStack = []

		modelChanged()
	}


	function doUndo()
	{
		if (!modelIsRegistered()){
			return
		}

		// Anything still waiting is a step of its own, and it has to land before
		// this one is taken back - otherwise the edit being undone was never
		// recorded and undo would jump past it.
		flushSnapshot()

		if (internal.m_undoStack.length === 0){
			return
		}

		internal.m_isBlocked = true

		internal.m_redoStack.push(internal.m_observedModel.toJson())

		let prevStateModel = internal.m_undoStack.pop()

		internal.m_observedModel.createFromJson(prevStateModel)

		internal.m_beginStateModel = prevStateModel

		modelChanged()
		undo()

		internal.m_isBlocked = false
	}


	function doRedo(steps)
	{
		if (!modelIsRegistered()){
			return
		}

		flushSnapshot()

		if (internal.m_redoStack.length === 0){
			return
		}

		internal.m_isBlocked = true

		internal.m_undoStack.push(internal.m_observedModel.toJson())

		let nextStateModel = internal.m_redoStack.pop()
		internal.m_observedModel.createFromJson(nextStateModel)

		internal.m_beginStateModel = nextStateModel

		modelChanged()
		redo()

		internal.m_isBlocked = false
	}


	function doListShift(steps, fromList, toList)
	{
		if (internal.m_isBlocked){
			return
		}

		if ((steps > 0) && (fromList.length >= steps)){
			internal.m_isBlocked = true

			let stateModel = fromList[fromList.length - steps]
			if (internal.m_observedModel.copy(stateModel)){
				internal.m_observedModel.refresh()
			}
			else{
				console.warn("Unable to copy observer model from current state model")
			}

			for (let i = 0; i < steps; ++i){
				toList.push(fromList.pop())
			}

			modelChanged()

			internal.m_isBlocked = false
		}
	}


	function registerModel(model)
	{
		if (modelIsRegistered()){
			console.warn("Model is already registered in the undo manager")

			unregisterModel()
		}

		resetUndo()

		// Serialised once and used for both the first step and the saved-state
		// baseline. Registering used to serialise the model, then copy it - and
		// the copy was itself a serialise, a parse and a tree of new objects.
		let stateJson = model.toJson()

		internal.m_beginStateModel = stateJson
		internal.m_standardStateModel = stateJson

		internal.m_observedModel = model

		if (autoTracking){
			internal.m_observedModel.modelChanged.connect(onModelChanged)
		}
	}


	function unregisterModel()
	{
		snapshotTimer.stop()
		internal.m_pendingSnapshot = false

		if (autoTracking && internal.m_observedModel){
			internal.m_observedModel.modelChanged.disconnect(onModelChanged)
		}

		internal.m_observedModel = null
		resetUndo()
	}


	function modelIsRegistered()
	{
		return internal.m_observedModel != null
	}


	function isTransaction(){
		return internal.m_isBlocked
	}


	function beginChanges()
	{
		if (!modelIsRegistered()){
			console.error("Unable to begin changes. Model is not registered")

			return
		}

		if (internal.m_isBlocked){
			console.error("The changes in the undo manager have already started")

			return
		}

		// Whatever was still waiting belongs before this transaction, not inside
		// it.
		flushSnapshot()

		internal.m_beginStateModel = internal.m_observedModel.toJson()

		internal.m_isBlocked = true
	}


	function endChanges()
	{
		if (!internal.m_isBlocked){
			console.error("The changes in the undo manager have not started")

			return
		}

		makeChanges()

		internal.m_isBlocked = false
	}


	function makeChanges()
	{
		if (!modelIsRegistered()){
			console.error("Unable to make changes. Model is not registered")

			return
		}

		if (internal.m_beginStateModel != ""){
			internal.m_undoStack.push(internal.m_beginStateModel)

			// The lid. Without it the stack holds one document-sized string per
			// edit for as long as the tab stays open.
			if (undoRedoManager.maxUndoSteps > 0){
				while (internal.m_undoStack.length > undoRedoManager.maxUndoSteps){
					internal.m_undoStack.shift()
				}
			}

			internal.m_redoStack = []

			modelChanged()
		}
	}


	// Records the step the pending edits belong to, if there is one waiting.
	function flushSnapshot()
	{
		snapshotTimer.stop()

		if (!internal.m_pendingSnapshot){
			return
		}

		internal.m_pendingSnapshot = false

		if (!modelIsRegistered()){
			return
		}

		makeChanges()

		internal.m_beginStateModel = internal.m_observedModel.toJson()
	}


	function setBlockingUpdateModel(blockingUpdateModel){
		internal.m_isBlocked = blockingUpdateModel
	}


	function commandHandle(commandId)
	{
		if (commandId === "Undo"){
			doUndo()
		}
		else if (commandId === "Redo"){
			doRedo()
		}
	}


	// The state the document was last saved in, kept as the same JSON the rest of
	// this file already deals in. It used to be a deep copy of the model, made by
	// serialising it and building a second tree of objects from the result - once
	// per save - and compared afterwards by walking both trees property by
	// property on every single change.
	function setStandardModel(model)
	{
		if (!modelIsRegistered()){
			console.error("Unable to set standard model. Model is not registered")

			return
		}

		flushSnapshot()

		internal.m_standardStateModel = model.toJson()
	}


	function isModifiedFrom(model)
	{
		if (!model || internal.m_standardStateModel === ""){
			return false
		}

		return model.toJson() !== internal.m_standardStateModel
	}


	// Kept for callers that still ask; the saved state is a string now.
	function getStandardModel()
	{
		return internal.m_standardStateModel
	}


	QtObject {
		id: internal

		property bool m_isBlocked: false

		property var m_observedModel: null
		property string m_beginStateModel
		property string m_standardStateModel

		property bool m_pendingSnapshot: false

		property var m_undoStack: []
		property var m_redoStack: []
	}

	Timer {
		id: snapshotTimer

		interval: undoRedoManager.snapshotDelay
		repeat: false

		onTriggered: {
			undoRedoManager.flushSnapshot()
		}
	}

	function onModelChanged(){
		if (internal.m_isBlocked){
			return
		}

		// Noted, not serialised. The document is walked once the edits stop.
		internal.m_pendingSnapshot = true
		snapshotTimer.restart()
	}

	// The pending step is recorded before the count is read: an edit made a
	// moment ago has not reached the stack yet, and asking first would answer
	// that there is nothing to undo.
	Shortcut {
		sequence: "Ctrl+Z"
		enabled: true
		onActivated: {
			undoRedoManager.flushSnapshot()

			if (undoRedoManager.getAvailableUndoSteps() > 0){
				undoRedoManager.commandHandle("Undo")
			}
		}
	}

	Shortcut {
		sequence: "Ctrl+Shift+Z"
		enabled: true
		onActivated: {
			undoRedoManager.flushSnapshot()

			if (undoRedoManager.getAvailableRedoSteps() > 0){
				undoRedoManager.commandHandle("Redo")
			}
		}
	}
}
