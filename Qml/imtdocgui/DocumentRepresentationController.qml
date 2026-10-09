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

	signal startUpdateDocument(string documentId)
	signal updateDocumentFailed(string documentId, string message)
	signal documentUpdated(string documentId)

	signal startUpdateRepresentation(string documentId, var representation)
	signal representationUpdated(string documentId, var representation)
	signal updateRepresentationFailed(string documentId, string message)

	onRepresentationModelChanged: {
		if (view){
			view.model = representationModel
			representationUpdated(documentId, representationModel)
		}
	}

	function updateRepresentationFromDocument(){
		startUpdateRepresentation(documentId, representationModel)
		console.warn("updateRepresentationFromDocument() should be implemented in a subclass")
		representationUpdated(documentId, representationModel)
	}

	function updateDocumentFromRepresentation(){
		startUpdateDocument(documentId)
		console.warn("updateDocumentFromRepresentation() should be implemented in a subclass")
		documentUpdated(documentId)
	}
}
