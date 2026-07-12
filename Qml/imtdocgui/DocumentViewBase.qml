import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

// Base type for editor views hosted by a DocumentService. Beyond the raw
// identifiers every registered editor needs (documentId/documentTypeId/
// documentManager/documentManagerView), this exposes the live Document
// state object plus small convenience wrappers, so a concrete editor rarely
// needs to call back into documentManager by hand.
ViewBase {
	id: root;

	objectName: "DocumentViewBase"

	property string documentId;
	property string documentTypeId;
	property var documentManager: null; // DocumentService
	property Item documentManagerView: null;
	property DocumentRepresentationController representationController: null;

	// Live state of the document this view is editing. Bind to it directly
	// instead of polling documentManager.documentIsDirty(documentId) etc.:
	//   visible: root.document && root.document.isDirty
	readonly property var document: (documentManager && documentId) ? documentManager.getDocument(documentId) : null

	// Read-only convenience aliases for the three states editors check most
	// often. Safe to use even before `document` resolves (default to the
	// same values Document itself defaults to).
	readonly property bool isDirty: document ? document.isDirty : false
	readonly property bool isNew: document ? document.isNew : true
	readonly property bool busy: document ? document.loading : false

	// Emitted once after this document's data has finished loading AND this
	// view has been registered — i.e. exactly when it's safe to assume the
	// model has real content. Equivalent to DocumentService.documentReady
	// filtered to this view's own documentId.
	signal documentReady()

	// Emitted right after a save this view's document initiated completes
	// successfully (server round-trip already resolved; document.isDirty
	// has already gone false and document.isNew already gone false by the
	// time this fires).
	signal documentSaved()

	// Emitted right before this document is closed (view is about to be
	// torn down). Useful for one-shot cleanup that doesn't belong in
	// Component.onDestruction (e.g. releasing something documentManager
	// itself doesn't know about).
	signal documentClosing()

	// Override in a subclass to validate the current representationModel
	// before it is allowed to save. Return {valid: false, message: "..."}
	// to block the save and show `message`; anything else (including the
	// default) is treated as valid. Has direct access to the view's own
	// child controls, which is what makes cross-field checks (e.g. a
	// password-confirmation input) practical here rather than in a
	// separate validator object.
	function validate(){
		return {valid: true, message: ""}
	}

	function requestSave(){
		if (documentManager && documentId){
			documentManager.save(documentId)
		}
	}

	function requestClose(){
		if (documentManager && documentId){
			documentManager.closeDocument(documentId)
		}
	}

	function requestUndo(){
		if (documentManager && documentId){
			documentManager.doUndo(documentId, 1)
		}
	}

	function requestRedo(){
		if (documentManager && documentId){
			documentManager.doRedo(documentId, 1)
		}
	}
}
