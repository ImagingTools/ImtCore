import QtQuick 2.12

// Reactive state of a single open document instance. Owned exclusively by
// DocumentService, which is the only object allowed to mutate these
// properties. Everything else (workspaces, editors, binders) only reads or
// binds to them.
QtObject {
	id: root

	property string documentId
	property string typeId
	property string objectId: ""
	property string name: ""

	// Set once by DocumentService at creation time (DocumentService.
	// getDefaultDocumentName()), so displayName has a single source of truth
	// instead of every workspace re-deriving the "name || fallback" fallback
	// on its own.
	property string defaultName: ""

	// Single source of truth for what a workspace should show for this
	// document: its name (falling back to defaultName), optionally prefixed
	// with a dirty marker. Replaces the "name || fallback" / "* " + name
	// logic that used to be duplicated in DocumentTabsWorkspace and
	// SingleDocumentWorkspace.
	readonly property string displayName: name !== "" ? name : defaultName
	readonly property string tabTitle: isDirty ? "* " + displayName : displayName

	property bool isDirty: false
	property bool isNew: true
	property bool isClosing: false

	property int undoSteps: 0
	property int redoSteps: 0

	// True while a request for this document is in flight (open, create,
	// save, close, undo, redo). Replaces the scattered startLoading/
	// stopLoading bookkeeping that used to live in every workspace view.
	property bool busy: false

	// True while a representation pull (server -> view) is in progress for
	// any view bound to this document. Kept separate from `busy` because it
	// is driven by the view-sync layer (DocumentViewBinder), not by the
	// document-service request/response cycle.
	property int __activeSyncs: 0
	readonly property bool syncing: __activeSyncs > 0

	function beginSync(){
		__activeSyncs++
	}

	function endSync(){
		__activeSyncs = Math.max(0, __activeSyncs - 1)
	}

	// Becomes true once the document's data has finished loading from the
	// server at least once.
	property bool dataReady: false

	// Becomes true once at least one view instance has been registered.
	property bool viewReady: false

	readonly property bool ready: dataReady && viewReady
	property bool readyEmitted: false

	// Single reactive flag workspaces bind their loading overlay to: true
	// while the document is still loading, or any request/sync is in
	// flight. Replaces the startLoading/stopLoading signal bookkeeping that
	// used to be duplicated across every workspace view.
	readonly property bool loading: !dataReady || syncing || busy

	// viewTypeId -> view Item
	property var views: ({})

	// DocumentViewBinder instances bound to this document (one per
	// registered view). Populated/cleared by DocumentService.
	property var binders: []

	// Set by a DocumentViewBinder right before it pushes a view-originated
	// change, so that the broadcast pull triggered by that change can skip
	// pulling it straight back into the same view.
	property var __lastPushingBinder: null

	// Optional function(documentId): string, resolves the name to use for
	// the next save when the document has no explicit name yet.
	property var nameResolver: null

	// Emitted exactly once, the first time `ready` becomes true.
	signal becameReady()

	onReadyChanged: {
		if (ready && !readyEmitted){
			readyEmitted = true
			becameReady()
		}
	}

	function addView(viewTypeId, view){
		views[viewTypeId] = view
		if (Object.keys(views).length > 0){
			viewReady = true
		}
	}

	function getView(viewTypeId){
		if (viewTypeId && viewTypeId in views){
			return views[viewTypeId]
		}

		let viewTypeIds = Object.keys(views)
		return viewTypeIds.length > 0 ? views[viewTypeIds[0]] : null
	}

	function resolveNameForSave(fallbackName){
		let resolvedName = fallbackName || ""

		if (typeof nameResolver === "function"){
			let dynamicName = nameResolver(documentId)
			if (dynamicName){
				resolvedName = dynamicName
			}
		}

		return resolvedName
	}
}
