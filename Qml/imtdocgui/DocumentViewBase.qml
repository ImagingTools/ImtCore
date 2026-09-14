import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtguigql 1.0
import imtcontrols 1.0

/**
 * DocumentViewBase
 *
 * A ViewBase that edits a document: it adds the identity of that document and the
 * document's live state on top of the plain model/GUI plumbing of ViewBase.
 *
 * documentId, documentTypeId and documentManager are filled in by the document
 * framework when the view is registered - by DocumentDecorator.onViewRegistered, and
 * by DocumentService.setupDocumentView for the older service. A view must not assign
 * them itself.
 *
 * isNewDocument, isDirty, isLoading and documentName are derived from the document
 * manager and kept in sync; treat them as read-only. They are plain properties rather
 * than bindings because the manager exposes this state through functions, not
 * properties, so it has to be re-read whenever an event can change the answer.
 */
ViewBase {
	id: root;

	objectName: "DocumentViewBase"

	property string documentId;
	property string documentTypeId;
	property var documentManager: null; // DocumentServiceBase
	property Item documentManagerView: null;
	property DocumentRepresentationController representationController: null;

	property BaseClass subscriptionInput: BaseClass{}
	property string  subscriptionCommandId: ""

	/**
	 * True until the document has actually been persisted.
	 *
	 * This is not the same as "the model has no id": the document framework hands a
	 * draft document a real id as soon as it is created, well before any Save, so an
	 * id-emptiness test reports a brand new document as already saved. Only the
	 * document manager knows whether a Save has happened.
	 *
	 * Defaults to true, and stays true while the document identity is still unknown,
	 * so that a view gating server-bound features on it (extra pages, tickets,
	 * revision history) stays closed until the document is known to exist.
	 */
	property bool isNewDocument: true

	/** True while the document has unsaved changes. */
	property bool isDirty: false

	/** True while the document's data is still being loaded from the server. */
	property bool isLoading: false

	/** Current document name, as known by the document manager. */
	property string documentName: ""

	/**
	 * Collection-objectId of the document, i.e. the id of the stored record, as
	 * opposed to documentId which identifies the editing session. Empty until the
	 * document has been saved for the first time.
	 *
	 * This is the id to link other entities to - tickets, revision history, bindings.
	 * The representation model's own id field is not a substitute: it is only refilled
	 * when the representation is re-fetched, which a save does not do.
	 */
	property string documentObjectId: ""

	/**
	 * Emitted after the document behind this view has been saved.
	 * Raised by DocumentDecorator; the document state above is already up to date.
	 */
	signal documentSaved()

	/**
	 * Re-read the document state from the manager. Called automatically; a view only
	 * needs this if it changes the document through a path the manager does not
	 * report.
	 */
	function updateDocumentState(){
		let manager = root.documentManager
		if (!manager || root.documentId === ""){
			return
		}

		if (typeof manager.documentIsNew === "function"){
			root.isNewDocument = manager.documentIsNew(root.documentId)
		}

		if (typeof manager.documentIsDirty === "function"){
			root.isDirty = manager.documentIsDirty(root.documentId)
		}

		if (typeof manager.documentIsLoading === "function"){
			root.isLoading = manager.documentIsLoading(root.documentId)
		}

		if (typeof manager.getDocumentName === "function"){
			root.documentName = manager.getDocumentName(root.documentId)
		}

		if (typeof manager.getDocumentObjectId === "function"){
			root.documentObjectId = manager.getDocumentObjectId(root.documentId)
		}
	}

	// Routed through the command path rather than straight at the manager, so that the
	// document decorator's own save logic - name providers, asking the user for a name
	// when the document has none yet - is not bypassed.
	function saveDocument(){
		root.commandActivated("Save")
	}

	function undo(){
		root.commandActivated("Undo")
	}

	function redo(){
		root.commandActivated("Redo")
	}

	function closeDocument(){
		let manager = root.documentManager
		if (!manager || root.documentId === "" || typeof manager.closeDocument !== "function"){
			return false
		}

		manager.closeDocument(root.documentId)

		return true
	}

	// Deliberately Connections on root rather than onDocumentIdChanged: / onDocumentSaved:
	// handlers: a signal handler declared here would be overridden by a subclass that
	// declares the same handler, and several editors already do exactly that.
	Connections {
		target: root

		function onDocumentIdChanged(){
			root.updateDocumentState()
		}

		function onDocumentManagerChanged(){
			root.updateDocumentState()
		}

		function onDocumentSaved(){
			root.updateDocumentState()
		}
	}

	Connections {
		target: root.documentManager

		// The two document service implementations do not carry the same signal set -
		// documentAdded against documentCreated, documentOpened with and without a type
		// id - so the ones missing on the current target must not warn.
		ignoreUnknownSignals: true

		function onDocumentSaved(documentId){
			if (documentId === root.documentId){
				root.updateDocumentState()
			}
		}

		// A document is marked as no longer new before the view is given its documentId,
		// so the state has to be re-read once the view and the document are connected -
		// otherwise an existing document opened at start-up still reads as new.
		function onDocumentOpened(documentId){
			if (documentId === root.documentId){
				root.updateDocumentState()
			}
		}

		function onDocumentAdded(documentId){
			if (documentId === root.documentId){
				root.updateDocumentState()
			}
		}

		function onDocumentCreated(documentId){
			if (documentId === root.documentId){
				root.updateDocumentState()
			}
		}

		function onDocumentDataLoaded(documentId){
			if (documentId === root.documentId){
				root.updateDocumentState()
			}
		}

		// The save mutation's reply (documentSaved) and the server notification that
		// carries the new objectId arrive on two independent channels in no fixed
		// order, so both have to trigger a refresh for the state to be complete
		// whichever lands last.
		function onDocumentManagerChanged(typeOperation, objectId, documentId, documentName){
			if (documentId === root.documentId){
				root.updateDocumentState()
			}
		}

		function onDocumentIsDirtyChanged(documentId, isDirty){
			if (documentId === root.documentId){
				root.isDirty = isDirty
			}
		}

		function onDocumentNameChanged(documentId, oldName, newName){
			if (documentId === root.documentId){
				root.documentName = newName
			}
		}
	}

	SubscriptionClient {
		id: subscriptionClient
		gqlCommandId: root.subscriptionCommandId

		sdlInputObject: root.subscriptionInput

		onMessageReceived: {
		}
	}
}
