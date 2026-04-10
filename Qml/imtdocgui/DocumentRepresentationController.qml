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

	// Re-entrancy guard: when two notifications arrive simultaneously
	// (e.g. from DocumentDecorator and SubscriptionClient), the second
	// call is deferred until the first completes, preventing
	// setBlockingUpdateModel(false) from being called while copyFrom
	// is still running inside the first request's onFinished handler.
	function updateRepresentationFromDocument(){
		if (__internal._updateInProgress) {
			__internal._updatePending = true
			return
		}
		__internal._updateInProgress = true
		__internal._updatePending = false
		_doUpdateRepresentationFromDocument()
	}

	function _doUpdateRepresentationFromDocument(){
		startUpdateRepresentation(documentId, representationModel)
		console.warn("_doUpdateRepresentationFromDocument() should be implemented in a subclass")
		representationUpdated(documentId, representationModel)
	}

	function updateDocumentFromRepresentation(){
		startUpdateDocument(documentId)
		console.warn("updateDocumentFromRepresentation() should be implemented in a subclass")
		documentUpdated(documentId)
	}

	property QtObject __internal: QtObject {
		id: signalMonitor

		property bool isRepresentationUpdatePending: false
		property bool isDocumentUpdatePending: false
		property bool _updateInProgress: false
		property bool _updatePending: false

		property Connections rootConnections: Connections {
			target: root

			function onStartUpdateRepresentation(documentId, representation){
				signalMonitor.isRepresentationUpdatePending = true
			}

			function onRepresentationUpdated(documentId, representation){
				if (!signalMonitor.isRepresentationUpdatePending){
					console.error(
								"representationUpdated() called for document '" + root.documentId +
								"' before startUpdateRepresentation(). Possible logic error."
								)
				}
				else{
					signalMonitor.isRepresentationUpdatePending = false
				}

				signalMonitor._updateInProgress = false
				if (signalMonitor._updatePending) {
					signalMonitor._updatePending = false
					Qt.callLater(root.updateRepresentationFromDocument)
				}
			}

			function onStartUpdateDocument(documentId){
				signalMonitor.isDocumentUpdatePending = true
			}

			function onDocumentUpdated(documentId){
				if (!signalMonitor.isDocumentUpdatePending){
					console.error(
								"documentUpdated() called for document '" + root.documentId +
								"' before startUpdateDocument(). Possible logic error."
								)
				}
				else{
					signalMonitor.isDocumentUpdatePending = false
				}
			}
		}
	}
}
