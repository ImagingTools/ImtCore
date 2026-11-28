import QtQuick 2.12
import QtTest 1.12
import imtassistgui 1.0
import imtqmltest 1.0

TestCase {
	id: testCase

	name: "GqlBasedAssistantController"
	when: windowShown

	Component {
		id: controllerComp

		GqlBasedAssistantController {
		}
	}

	function init(){
		GqlTestTransport.reset()
	}

	function response(commandId, payload){
		let data = {}
		data[commandId] = payload

		return JSON.stringify({"data": data})
	}

	function waitForCommand(commandId){
		tryVerify(function(){ return GqlTestTransport.requestedCommands().indexOf(commandId) >= 0 }, 5000)

		let request = GqlTestTransport.parseLastRequest()
		verify(request.parsed, "server-side parser rejected: " + GqlTestTransport.lastRequestBody)
		compare(request.commandId, commandId)

		return request
	}

	// CGqlModel reads the body of every answer except 401/403, so a failed request needs an error body.
	function failure(commandId){
		let errors = {}
		errors[commandId] = {"message": "Internal error"}

		return JSON.stringify({"errors": errors})
	}

	function startedController(){
		GqlTestTransport.queueResponse("AssistantStartConversation", response("AssistantStartConversation", {"conversationId": "c-1", "messagesJson": "[]"}))

		let controller = createTemporaryObject(controllerComp, testCase)
		controller.startConversation()
		tryCompare(controller, "conversationId", "c-1", 5000)
		tryCompare(controller, "busy", false, 5000)

		return controller
	}

	function test_statusIsRequested(){
		GqlTestTransport.queueResponse("AssistantGetStatus", response("AssistantGetStatus", {"available": true}))

		let controller = createTemporaryObject(controllerComp, testCase)
		controller.checkAvailability()

		let request = waitForCommand("AssistantGetStatus")
		compare(request.requestType, "query")
		tryCompare(controller, "available", true, 5000)

		GqlTestTransport.queueStatus("AssistantGetStatus", 500)
		GqlTestTransport.queueResponse("AssistantGetStatus", failure("AssistantGetStatus"))
		controller.checkAvailability()
		tryCompare(controller, "available", false, 5000)
		compare(controller.messagesModel.count, 0)
	}

	function test_staleAvailabilityErrorIsIgnored(){
		GqlTestTransport.queueDelay("AssistantGetStatus", 300)
		GqlTestTransport.queueResponse("AssistantGetStatus", failure("AssistantGetStatus"))

		let controller = createTemporaryObject(controllerComp, testCase)
		controller.handleStatus(true)
		controller.checkAvailability()
		waitForCommand("AssistantGetStatus")
		controller.newConversation()

		wait(500)
		verify(controller.available)
		compare(controller.messagesModel.count, 0)
	}

	function test_messageWithContextIsSentAndAnswered(){
		let controller = startedController()

		let answer = [{"formatVersion": 1, "id": "m-1", "kind": "assistantText", "text": "Two open tasks"}]
		GqlTestTransport.queueResponse("AssistantSendMessage", response("AssistantSendMessage", {"conversationId": "c-1", "messagesJson": JSON.stringify(answer)}))

		controller.addContextItem("selection", "Task \"1\"", {"id": "task-1"})
		controller.sendMessage("Mark the selected task as done")

		let request = waitForCommand("AssistantSendMessage")
		compare(request.requestType, "mutation")
		compare(request.params.input.conversationId, "c-1")
		compare(request.params.input.text, "Mark the selected task as done")
		compare(request.params.input.contextItems.length, 1)
		compare(request.params.input.contextItems[0].kind, "selection")
		compare(request.params.input.contextItems[0].label, "Task \"1\"")
		compare(JSON.parse(request.params.input.contextItems[0].dataJson).id, "task-1")

		tryCompare(controller.messagesModel, "count", 1, 5000)
		compare(controller.busy, false)
		compare(JSON.parse(controller.messagesModel.get(0).messageJson).text, "Two open tasks")
	}

	function test_newConversationInvalidatesPendingStart(){
		GqlTestTransport.queueDelay("AssistantStartConversation", 300)
		GqlTestTransport.queueResponse("AssistantStartConversation", response("AssistantStartConversation", {"conversationId": "c-old", "messagesJson": "[]"}))

		let controller = createTemporaryObject(controllerComp, testCase)
		controller.sendMessage("Do not send this after stopping")
		waitForCommand("AssistantStartConversation")
		verify(controller.__pendingText !== "")

		controller.newConversation()
		verify(controller.__pendingText === "")
		wait(500)

		compare(controller.conversationId, "")
		verify(!controller.busy)
		compare(controller.messagesModel.count, 0)
		verify(GqlTestTransport.requestedCommands().indexOf("AssistantSendMessage") < 0)
	}

	function test_confirmationSendsThePreviewId(){
		let controller = startedController()

		let executed = [{"formatVersion": 1, "id": "m-2", "kind": "actionExecuted", "previewId": "p-1", "code": "succeeded"}]
		GqlTestTransport.queueResponse("AssistantConfirmPreview", response("AssistantConfirmPreview", {"conversationId": "c-1", "messagesJson": JSON.stringify(executed)}))

		controller.confirmPreview("p-1")

		let request = waitForCommand("AssistantConfirmPreview")
		compare(request.params.input.previewId, "p-1")
		tryCompare(controller.messagesModel, "count", 1, 5000)
		compare(controller.getPreviewState("p-1"), "decided")
	}

	function test_cancelIsSentWhileMessageIsProcessed(){
		let controller = startedController()

		GqlTestTransport.queueDelay("AssistantSendMessage", 500)
		GqlTestTransport.queueResponse("AssistantSendMessage", response("AssistantSendMessage", {"conversationId": "c-1", "messagesJson": JSON.stringify([{"formatVersion": 1, "id": "m-3", "kind": "error", "code": "Canceled"}])}))
		GqlTestTransport.queueResponse("AssistantCancelRequest", response("AssistantCancelRequest", {"canceled": true}))

		controller.sendMessage("Show open tasks")
		verify(controller.busy)
		controller.cancel()

		waitForCommand("AssistantCancelRequest")
		tryCompare(controller, "busy", false, 5000)
	}

	function test_lostConfirmationAnswerIsNotSuccess(){
		let controller = startedController()

		GqlTestTransport.queueStatus("AssistantConfirmPreview", 500)
		GqlTestTransport.queueResponse("AssistantConfirmPreview", failure("AssistantConfirmPreview"))
		controller.confirmPreview("p-1")

		tryCompare(controller.messagesModel, "count", 1, 5000)
		let message = JSON.parse(controller.messagesModel.get(0).messageJson)
		compare(message.kind, "error")
		compare(message.code, "ConfirmationStatusUnknown")
	}

	function test_lateConfirmationFromClosedConversationIsIgnored(){
		let controller = startedController()
		let executed = [{"formatVersion": 1, "id": "m-old", "kind": "actionExecuted", "previewId": "p-1", "code": "succeeded"}]
		GqlTestTransport.queueDelay("AssistantConfirmPreview", 100)
		GqlTestTransport.queueResponse("AssistantConfirmPreview", response("AssistantConfirmPreview", {"conversationId": "c-1", "messagesJson": JSON.stringify(executed)}))

		controller.confirmPreview("p-1")
		waitForCommand("AssistantConfirmPreview")
		controller.newConversation()
		wait(200)

		compare(controller.conversationId, "")
		compare(controller.messagesModel.count, 0)
	}

	function test_lateHistoryErrorFromClosedConversationIsIgnored(){
		let controller = startedController()
		GqlTestTransport.queueDelay("AssistantGetHistory", 100)
		GqlTestTransport.queueStatus("AssistantGetHistory", 500)
		GqlTestTransport.queueResponse("AssistantGetHistory", failure("AssistantGetHistory"))

		controller.refreshHistory()
		waitForCommand("AssistantGetHistory")
		controller.newConversation()
		wait(200)

		compare(controller.conversationId, "")
		compare(controller.messagesModel.count, 0)
	}
}
