import QtQuick 2.12
import QtTest 1.12
import imtdocgui 1.0
import imtguigql 1.0
import imtqmltest 1.0

TestCase {
	id: testCase

	name: "DocumentUpdateRequestSender"

	DocumentRepresentationController {
		id: controller

		property int readBackCount: 0

		documentId: "doc-1"

		function updateRepresentationFromDocument(){
			readBackCount++
		}
	}

	SignalSpy {
		id: updatedSpy

		target: controller
		signalName: "documentUpdated"
	}

	SignalSpy {
		id: failedSpy

		target: controller
		signalName: "updateDocumentFailed"
	}

	SignalSpy {
		id: finishedSpy

		signalName: "finished"
	}

	DocumentUpdateRequestSender {
		id: updateSender

		representationController: controller
		permissionPath: "/Documents/Edit"
		gqlCommandId: "UpdateDocumentFromRepresentation"
	}

	DocumentUpdateRequestSender {
		id: customSender

		property var refusals: []

		representationController: controller
		gqlCommandId: "UpdateDocumentFromRepresentation"

		function refuse(message){
			refusals.push(message)
		}
	}

	function init(){
		GqlTestTransport.reset()
		controller.changeId = ""
		controller.readBackCount = 0
		updateSender.refusalMessage = "The changes could not be saved"
		GqlTestTransport.responseStatus = 200
		customSender.refusals = []
		updatedSpy.clear()
		failedSpy.clear()
	}

	function sendAndWait(sender, body){
		GqlTestTransport.responseBody = body
		finishedSpy.target = sender
		finishedSpy.clear()
		sender.send()
		if (finishedSpy.count === 0){
			finishedSpy.wait()
		}
	}

	function answer(status, message){
		return JSON.stringify({data: {UpdateDocumentFromRepresentation: {status: status, message: message}}})
	}

	function test_success(){
		sendAndWait(updateSender, answer("Success", ""))

		compare(updatedSpy.count, 1)
		compare(updatedSpy.signalArguments[0][0], "doc-1")
		compare(failedSpy.count, 0)
		compare(controller.readBackCount, 0)
	}

	function test_refusal_data(){
		return [
			{tag: "server message", message: "Name is taken", refusalMessage: "The changes could not be saved", expected: "Name is taken"},
			{tag: "default message", message: "", refusalMessage: "The changes could not be saved", expected: "The changes could not be saved"},
			{tag: "own default message", message: "", refusalMessage: "The organization could not be saved", expected: "The organization could not be saved"},
			{tag: "no default message", message: "", refusalMessage: "", expected: ""}
		]
	}

	function test_refusal(data){
		updateSender.refusalMessage = data.refusalMessage

		sendAndWait(updateSender, answer("Failed", data.message))

		compare(updatedSpy.count, 0)
		compare(failedSpy.count, 1)
		compare(failedSpy.signalArguments[0][0], "doc-1")
		compare(failedSpy.signalArguments[0][1], data.expected)
		compare(controller.readBackCount, 0, "What follows a refusal is decided by DocumentDecorator")
	}

	function test_error(){
		sendAndWait(updateSender, '{"errors":[{"message":"Access denied"}]}')

		compare(updatedSpy.count, 0)
		compare(failedSpy.count, 1)
		compare(failedSpy.signalArguments[0][1], "Access denied")
		compare(controller.readBackCount, 0)
	}

	function test_overriddenRefusal(){
		sendAndWait(customSender, answer("Failed", ""))
		sendAndWait(customSender, '{"errors":[{"message":"Access denied"}]}')

		compare(customSender.refusals, ["The changes could not be saved", "Access denied"])
		compare(failedSpy.count, 0)
		compare(controller.readBackCount, 0)
	}

	function test_request(){
		controller.changeId = "change-1"

		sendAndWait(updateSender, answer("Success", ""))

		let request = GqlTestTransport.parseLastRequest()
		verify(request.parsed, "server-side parser rejected: " + GqlTestTransport.lastRequestBody)
		compare(request.requestType, "mutation")
		compare(request.commandId, "UpdateDocumentFromRepresentation")
		compare(GqlTestTransport.lastRequestHeaders["documentchangeid"], "change-1")
		compare(GqlTestTransport.lastRequestHeaders["permissionpath"], "/Documents/Edit")
	}

	function test_noChangeIdHeader(){
		sendAndWait(updateSender, answer("Success", ""))

		verify(!("documentchangeid" in GqlTestTransport.lastRequestHeaders))
	}

	// A 401 request is sent again after the token refresh: it is answered by its repeated request
	function test_unauthorizedIsNotAnswered(){
		GqlTestTransport.responseStatus = 401

		sendAndWait(updateSender, "")

		compare(updatedSpy.count, 0)
		compare(failedSpy.count, 0)
	}

	function test_forbiddenIsRefused(){
		GqlTestTransport.responseStatus = 403

		sendAndWait(updateSender, "")

		compare(updatedSpy.count, 0)
		compare(failedSpy.count, 1)
		compare(failedSpy.signalArguments[0][1], "The changes could not be saved")
	}

	function test_errorsAreAnsweredOnce_data(){
		return [
			{tag: "graphql error", status: 200, body: '{"errors":[{"message":"Locked"}]}', message: "Locked"},
			{tag: "no data", status: 200, body: '{"data":{"UpdateDocumentFromRepresentation":null}}', message: "The server returned no data. Please try again."},
			{tag: "server error", status: 500, body: "", message: "Network error"},
			{tag: "invalid json", status: 200, body: '{"data":', message: "Unable to read the server response"}
		]
	}

	function test_errorsAreAnsweredOnce(data){
		GqlTestTransport.responseStatus = data.status

		sendAndWait(updateSender, data.body)
		wait(0)

		compare(updatedSpy.count, 0)
		compare(failedSpy.count, 1)
		compare(failedSpy.signalArguments[0][1], data.message)
	}
}
