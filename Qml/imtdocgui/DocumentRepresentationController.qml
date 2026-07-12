import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

QtObject {
	id: root

	property string documentId
	property var representationModel: null
	property ViewBase view: null

	signal startUpdateDocument(string documentId)
	signal updateDocumentFailed(string documentId, string message)
	signal documentUpdated(string documentId)

	signal startUpdateRepresentation(string documentId, var representation)
	signal representationUpdated(string documentId, var representation)
	signal updateRepresentationFailed(string documentId, string message)

	// Subclasses (declared as a Component per document type/view, see
	// EditorRegistry) must override both functions below. There is no
	// working default implementation: silently reporting success without
	// fetching or storing any data would hide a real integration bug
	// instead of surfacing it.
	function updateRepresentationFromDocument(){
		let message = "updateRepresentationFromDocument() is not implemented for this document type"
		console.error(message)
		updateRepresentationFailed(documentId, message)
	}

	function updateDocumentFromRepresentation(){
		let message = "updateDocumentFromRepresentation() is not implemented for this document type"
		console.error(message)
		updateDocumentFailed(documentId, message)
	}
}
