import QtQuick 2.12
import Acf 1.0

QtObject {
	id: root

	property string documentId
	property var representationModel: null

	signal representationUpdated(string documentId, var representation)
	signal documentUpdated(string documentId)

	function updateRepresentationFromDocument(){
		console.warn("updateRepresentationFromDocument() should be implemented in a subclass")
	}

	function updateDocumentFromRepresentation(){
		console.warn("updateDocumentFromRepresentation() should be implemented in a subclass")
	}
}
