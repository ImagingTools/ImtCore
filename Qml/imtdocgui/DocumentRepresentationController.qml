import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

QtObject {
	id: root

	property string documentId
	property var representationModel: null
	property ViewBase view: null

	// Load the representation of a newly created document from the server instead of using representationModel defaults.
	property bool requestRepresentationOnCreate: false

	// Own updates of the view: false keeps the view as edited, the notification of its own update is not read back;
	// true reads the representation again after each own update, for documents the server completes.
	// Changes made by others, undo, redo and rollbacks are read back in both cases.
	property bool requestRepresentationOnUpdate: false

	// A refused update was not applied: true reads the representation again and drops the edits queued meanwhile,
	// false keeps the view as edited and sends the queued edits
	property bool readBackOnRefusal: true

	// Change-ID of the update being sent: set by DocumentDecorator for the time of updateDocumentFromRepresentation().
	// The update request (DocumentUpdateRequestSender) carries it to the server,
	// the server returns it in the DocumentChanged notification of that update.
	property string changeId: ""

	signal startUpdateDocument(string documentId)
	signal updateDocumentFailed(string documentId, string message)
	signal documentUpdated(string documentId)

	signal startUpdateRepresentation(string documentId, var representation)
	signal representationUpdated(string documentId, var representation)
	signal updateRepresentationFailed(string documentId, string message)

	// A new representation model is given to the view; representationUpdated() is answered by whoever sets the model
	onRepresentationModelChanged: {
		if (view){
			view.model = representationModel
		}
	}

	function updateRepresentationFromDocument(){
		startUpdateRepresentation(documentId, representationModel)
		console.warn("updateRepresentationFromDocument() should be implemented in a subclass")
		representationUpdated(documentId, representationModel)
	}

	// changeSet tells what was edited in representationModel, see Docs/Development/ModelChangeSet.md;
	// it is empty or undefined when that is unknown, then the whole representation is written.
	function updateDocumentFromRepresentation(changeSet){
		startUpdateDocument(documentId)
		console.warn("updateDocumentFromRepresentation() should be implemented in a subclass")
		documentUpdated(documentId)
	}
}
