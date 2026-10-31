import QtQuick 2.12
import imtguigql 1.0

// The representation request of a DocumentRepresentationController (a Get...Representation query):
// the model received from the server gives the representationModel of the controller, see getRepresentationModel(),
// and the controller answers representationUpdated(); an error answers updateRepresentationFailed().
// sdlObjectComp gives the SDL type of the representation.
GqlSdlRequestSender {
	id: root

	// DocumentRepresentationController whose representation this request reads
	property var representationController: null

	// The received model representationModel is made of: it is destroyed when the next one replaces it
	property var usedReceivedModel: null

	// Message of the error the request failed with, see onError()
	property string errorMessage: ""

	// The controller is answered once; a request refused with 401 is not answered: it is sent again after the token refresh
	onFinished: {
		let message = root.errorMessage
		root.errorMessage = ""
		if (status === 1){
			root.applyReceivedModel(root.sdlObject)
		}
		else if (root.state !== "Unauthorized"){
			root.representationController.updateRepresentationFailed(root.representationController.documentId, message)
		}
	}

	// The representationModel made of receivedModel, the model received from the server;
	// currentModel is the representationModel the controller has now.
	// Default: receivedModel itself. A part of receivedModel may be returned as well.
	// A receivedModel the result is not made of is destroyed: copy its parts (copyFrom()), do not move them into currentModel.
	function getRepresentationModel(receivedModel, currentModel){
		return receivedModel
	}

	function onError(message, type){
		root.errorMessage = message
		root.finished(-1)
	}

	function applyReceivedModel(receivedModel){
		root.sdlObject = null

		let model = root.getRepresentationModel(receivedModel, root.representationController.representationModel)
		if (model !== root.representationController.representationModel){
			root.representationController.representationModel = model
		}

		if (root.isPartOf(model, receivedModel)){
			if (root.usedReceivedModel){
				root.usedReceivedModel.destroy()
			}
			root.usedReceivedModel = receivedModel
		}
		else{
			receivedModel.destroy()
		}

		root.representationController.representationUpdated(root.representationController.documentId, model)
	}

	// The model is the received model or a part of it: the parts have it as their owner
	function isPartOf(model, receivedModel){
		let part = model
		while (part){
			if (part === receivedModel){
				return true
			}
			part = part.owner
		}

		return false
	}
}
