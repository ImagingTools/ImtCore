import QtQuick 2.12
import QtTest 1.12
import imtgui 1.0
import imtdocgui 1.0
import imtguigql 1.0
import imtqmltest 1.0
import imtbaseComplexCollectionFilterSdl 1.0

TestCase {
	id: testCase

	name: "DocumentRepresentationRequestSender"

	property int receivedDestroyed: 0
	property bool defaultDestroyed: false

	TextFilter {
		id: defaultModel

		Component.onDestruction: {
			testCase.defaultDestroyed = true
		}
	}

	Component {
		id: receivedComp

		TextFilter {
			Component.onDestruction: {
				testCase.receivedDestroyed++
			}
		}
	}

	Component {
		id: filterComp

		ComplexCollectionFilter {
			Component.onDestruction: {
				testCase.receivedDestroyed++
			}
		}
	}

	DocumentRepresentationController {
		id: controller

		documentId: "doc-1"
	}

	ViewBase {
		id: view
	}

	DocumentRepresentationController {
		id: viewController

		documentId: "doc-2"
		view: view
	}

	SignalSpy {
		id: updatedSpy

		target: controller
		signalName: "representationUpdated"
	}

	SignalSpy {
		id: viewUpdatedSpy

		target: viewController
		signalName: "representationUpdated"
	}

	SignalSpy {
		id: failedSpy

		target: controller
		signalName: "updateRepresentationFailed"
	}

	SignalSpy {
		id: finishedSpy

		signalName: "finished"
	}

	DocumentRepresentationRequestSender {
		id: representationSender

		representationController: controller
		permissionPath: "/Documents/View"
		gqlCommandId: "GetDocumentRepresentation"
		sdlObjectComp: receivedComp
	}

	DocumentRepresentationRequestSender {
		id: viewSender

		representationController: viewController
		gqlCommandId: "GetDocumentRepresentation"
		sdlObjectComp: receivedComp
	}

	DocumentRepresentationRequestSender {
		id: completingSender

		property bool currentIsDefault

		representationController: controller
		gqlCommandId: "GetDocumentRepresentation"
		sdlObjectComp: receivedComp

		function getRepresentationModel(receivedModel, currentModel){
			currentIsDefault = currentModel === defaultModel
			receivedModel.m_fieldIds = ["completed"]

			return receivedModel
		}
	}

	DocumentRepresentationRequestSender {
		id: partialSender

		representationController: controller
		gqlCommandId: "GetDocumentRepresentation"
		sdlObjectComp: receivedComp

		function getRepresentationModel(receivedModel, currentModel){
			currentModel.m_text = receivedModel.m_text

			return currentModel
		}
	}

	DocumentRepresentationRequestSender {
		id: partSender

		representationController: controller
		gqlCommandId: "GetDocumentRepresentation"
		sdlObjectComp: filterComp

		function getRepresentationModel(receivedModel, currentModel){
			return receivedModel.m_textFilter
		}
	}

	function init(){
		GqlTestTransport.reset()
		GqlTestTransport.responseStatus = 200
		defaultModel.m_text = ""
		defaultModel.m_fieldIds = []
		controller.representationModel = defaultModel
		updatedSpy.clear()
		viewUpdatedSpy.clear()
		failedSpy.clear()
	}

	function sendAndWait(sender, body, expectedStatus){
		GqlTestTransport.responseBody = body
		finishedSpy.target = sender
		finishedSpy.clear()
		sender.send()
		if (finishedSpy.count === 0){
			finishedSpy.wait()
		}

		compare(finishedSpy.count, 1)
		compare(finishedSpy.signalArguments[0][0], expectedStatus)
		wait(0)
	}

	function answer(text){
		return JSON.stringify({data: {GetDocumentRepresentation: {text: text, fieldIds: ["a", "b"]}}})
	}

	function test_receivedModelBecomesTheModel(){
		sendAndWait(representationSender, answer("from server"), 1)

		let model = controller.representationModel
		verify(model !== defaultModel)
		compare(model, representationSender.usedReceivedModel)
		compare(representationSender.sdlObject, null)
		compare(model.m_text, "from server")
		compare(model.m_fieldIds, ["a", "b"])
		compare(updatedSpy.count, 1)
		compare(updatedSpy.signalArguments[0][0], "doc-1")
		compare(updatedSpy.signalArguments[0][1], model)
		compare(failedSpy.count, 0)
		compare(defaultModel.m_text, "")
	}

	function test_previousReceivedModelIsDestroyed(){
		sendAndWait(representationSender, answer("first"), 1)
		let destroyedBefore = testCase.receivedDestroyed

		sendAndWait(representationSender, answer("second"), 1)

		compare(controller.representationModel.m_text, "second")
		compare(testCase.receivedDestroyed, destroyedBefore + 1)
		verify(!testCase.defaultDestroyed)
		compare(defaultModel.m_text, "")
	}

	function test_viewGetsTheModel(){
		sendAndWait(viewSender, answer("for view"), 1)

		compare(view.model, viewController.representationModel)
		compare(view.model.m_text, "for view")
		compare(viewUpdatedSpy.count, 1)
		compare(viewUpdatedSpy.signalArguments[0][0], "doc-2")
		compare(viewUpdatedSpy.signalArguments[0][1], view.model)
	}

	function test_completedReceivedModel(){
		sendAndWait(completingSender, answer("from server"), 1)

		verify(completingSender.currentIsDefault)
		compare(controller.representationModel.m_text, "from server")
		compare(controller.representationModel.m_fieldIds, ["completed"])
		compare(updatedSpy.count, 1)
	}

	function test_partOfTheAnswerKeepsTheModel(){
		let destroyedBefore = testCase.receivedDestroyed

		sendAndWait(partialSender, answer("only text"), 1)

		compare(controller.representationModel, defaultModel)
		compare(defaultModel.m_text, "only text")
		compare(defaultModel.m_fieldIds, [])
		compare(updatedSpy.count, 1)
		compare(updatedSpy.signalArguments[0][1], defaultModel)
		compare(partialSender.usedReceivedModel, null)
		compare(testCase.receivedDestroyed, destroyedBefore + 1)
	}

	function test_partOfTheReceivedModelIsTheModel(){
		let body = JSON.stringify({data: {GetDocumentRepresentation: {textFilter: {text: "inner", fieldIds: ["x"]}}}})
		let destroyedBefore = testCase.receivedDestroyed

		sendAndWait(partSender, body, 1)

		let model = controller.representationModel
		compare(model.m_text, "inner")
		compare(updatedSpy.count, 1)
		compare(updatedSpy.signalArguments[0][1], model)
		verify(partSender.usedReceivedModel !== null)
		compare(model.owner, partSender.usedReceivedModel)
		compare(testCase.receivedDestroyed, destroyedBefore)

		sendAndWait(partSender, body, 1)

		compare(testCase.receivedDestroyed, destroyedBefore + 1)
		compare(controller.representationModel.m_text, "inner")
	}

	function test_error_data(){
		return [
			{tag: "graphql error", body: '{"errors":[{"message":"Access denied"}]}', message: "Access denied"},
			{tag: "no data", body: '{"data":{"GetDocumentRepresentation":null}}', message: "The server returned no data. Please try again."}
		]
	}

	function test_error(data){
		defaultModel.m_text = "edited"

		sendAndWait(representationSender, data.body, -1)

		compare(controller.representationModel, defaultModel)
		compare(updatedSpy.count, 0)
		compare(failedSpy.count, 1)
		compare(failedSpy.signalArguments[0][0], "doc-1")
		compare(failedSpy.signalArguments[0][1], data.message)
		compare(defaultModel.m_text, "edited")
	}

	function test_request(){
		sendAndWait(representationSender, answer(""), 1)

		let request = GqlTestTransport.parseLastRequest()
		verify(request.parsed, "server-side parser rejected: " + GqlTestTransport.lastRequestBody)
		compare(request.requestType, "query")
		compare(request.commandId, "GetDocumentRepresentation")
		compare(GqlTestTransport.lastRequestHeaders["permissionpath"], "/Documents/View")
	}

	function test_unauthorizedIsNotAnswered(){
		GqlTestTransport.responseStatus = 401

		sendAndWait(representationSender, "", -1)

		compare(controller.representationModel, defaultModel)
		compare(updatedSpy.count, 0)
		compare(failedSpy.count, 0)
	}

	function test_forbiddenFailsTheRead(){
		GqlTestTransport.responseStatus = 403

		sendAndWait(representationSender, "", -1)

		compare(updatedSpy.count, 0)
		compare(failedSpy.count, 1)
		compare(failedSpy.signalArguments[0][0], "doc-1")
	}
}
