import QtQuick 2.12

// Explicit state machine owning the bidirectional sync between one editor
// view and its representation controller. Replaces the parallel
// "registeredViews[i] / updateCounters[i]" arrays that used to live in
// DocumentDecorator.
//
// States:
//   Idle     - nothing in flight for this pair
//   Pulling  - representation -> view update in progress (view input is
//              blocked so the write-back doesn't loop into a push)
//   Deferred - a pull was requested while the view was invisible; it runs
//              automatically once the view becomes visible again
QtObject {
	id: root

	readonly property int sIdle: 0
	readonly property int sPulling: 1
	readonly property int sDeferred: 2

	property Item view: null
	property QtObject controller: null

	property int state: sIdle
	property int __pullDepth: 0

	// Emitted right before a view-originated push (view -> document) is
	// issued, so the owning DocumentViewBinder can tell the Document
	// "I initiated this change, skip me on the next broadcast pull".
	signal pushStarted()

	// Requests a representation -> view pull. Runs immediately if the view
	// is visible, otherwise defers until it becomes visible.
	function pull(){
		if (!view || !controller){
			return
		}

		if (!view.visible){
			state = sDeferred
			return
		}

		state = sPulling
		__pullDepth++
		view.setBlockingUpdateModel(true)
		controller.updateRepresentationFromDocument()
	}

	function onViewBecameVisible(){
		if (state === sDeferred){
			pull()
		}
	}

	function onRepresentationUpdated(){
		__pullDepth = Math.max(0, __pullDepth - 1)
		if (__pullDepth === 0 && view){
			state = sIdle
			view.setBlockingUpdateModel(false)
			view.doUpdateGui()
		}
	}

	function onRepresentationUpdateFailed(){
		__pullDepth = 0
		state = sIdle
		if (view){
			view.setBlockingUpdateModel(false)
		}
	}

	// Called when the view reports a GUI-driven model change. Returns
	// false when the change is an echo of an in-progress pull (this
	// controller just wrote the model itself) and must be ignored. Also
	// ignored while sDeferred: the view isn't visible/interactive then, so
	// any reported change is a stale echo, not genuine input - pushing it
	// would overwrite the document with pre-pull data right before the
	// queued pull runs and reflects that overwrite straight back.
	function onViewModelChanged(){
		if (state === sPulling || state === sDeferred){
			return false
		}

		pushStarted()

		if (controller){
			controller.updateDocumentFromRepresentation()
		}

		return true
	}

	// Directly paints the view from the current representation model
	// without going through the server (used for brand-new documents that
	// have no server-side representation to pull yet).
	function paintFromCurrentModel(){
		if (!view){
			return
		}

		view.setBlockingUpdateModel(false)
		view.doUpdateGui()
	}
}
