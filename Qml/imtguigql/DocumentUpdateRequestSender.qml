import QtQuick 2.12
import imtguigql 1.0
import imtbaseCollectionDocumentServiceSdl 1.0

// The update request of a DocumentRepresentationController (an Update...FromRepresentation mutation):
// - carries the change-ID of the update being sent in the documentChangeId header,
//   the server returns it in the DocumentChanged notification of this update;
// - answers the controller once: documentUpdated() on success, updateDocumentFailed() on a refusal or an error.
//   A request refused with 401 is not answered: it is sent again after the token refresh.
// What follows a refusal (reading the document again) is decided by DocumentDecorator, see readBackOnRefusal.
GqlSdlRequestSender {
	id: root

	// DocumentRepresentationController whose update this request sends
	property var representationController: null

	// Message of a refusal that the server sends without one
	property string refusalMessage: qsTr("The changes could not be saved")

	// Message of the error the request failed with, see onError()
	property string errorMessage: ""

	requestType: 1

	sdlObjectComp: Component {
		DocumentOperationStatus {}
	}

	onFinished: {
		let message = root.errorMessage
		root.errorMessage = ""
		if (status === 1){
			let operationStatus = root.sdlObject
			root.sdlObject = null
			if (operationStatus.m_status === "Success"){
				root.representationController.documentUpdated(root.representationController.documentId)
			}
			else{
				root.refuse(operationStatus.m_message !== "" ? operationStatus.m_message : root.refusalMessage)
			}
			operationStatus.destroy()
		}
		else if (root.state !== "Unauthorized"){
			root.refuse(message !== "" ? message : root.refusalMessage)
		}
	}

	function getHeaders(){
		let headers = {}
		if (root.representationController && root.representationController.changeId !== ""){
			headers["documentChangeId"] = root.representationController.changeId
		}

		return headers
	}

	function onError(message, type){
		root.errorMessage = message
		root.finished(-1)
	}

	// Reports a refusal to the controller; may be overridden to show the message in another way
	function refuse(message){
		root.representationController.updateDocumentFailed(root.representationController.documentId, message)
	}
}
