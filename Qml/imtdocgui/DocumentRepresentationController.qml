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

	// changeIds of the document parts this representation shows (defined by the document type); empty: every change matters
	property var observedChangeIds: []

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

	// Decides whether the representation is read again after the document changed.
	// changeIds: the changed parts of the document; empty when unknown, then the representation is read.
	// isOwnChange: the change was made by the update of this controller, its result is in the representation already.
	// Default: the own change is not read, another one is read if it touches observedChangeIds.
	function isUpdateRequired(changeIds, isOwnChange){
		if (isOwnChange){
			return false
		}

		if (changeIds.length === 0 || root.observedChangeIds.length === 0){
			return true
		}

		for (let i = 0; i < changeIds.length; ++i){
			if (root.observedChangeIds.indexOf(changeIds[i]) >= 0){
				return true
			}
		}

		return false
	}

	// Returns the source ID of the update: the change notification with this ID is the own change of this controller.
	// A GQL controller returns the ID of its update request (GqlSdlRequestSender.send()). Without it every change is read again.
	function updateDocumentFromRepresentation(){
		startUpdateDocument(documentId)
		console.warn("updateDocumentFromRepresentation() should be implemented in a subclass")
		documentUpdated(documentId)

		return ""
	}
}
