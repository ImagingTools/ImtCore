import QtQuick 2.12
import QtTest 1.12
import imtassistgui 1.0

TestCase {
	id: testCase

	name: "AssistantController"

	Component {
		id: controllerComp

		AssistantController {
			property var requests: []

			// reimplemented (AssistantController)
			function sendRequest(commandId, input){
				let list = requests.slice()
				list.push({"commandId": commandId, "input": input})
				requests = list
			}

			function lastRequest(){
				return requests.length > 0 ? requests[requests.length - 1] : null
			}

			function countRequests(commandId){
				let count = 0
				for (let i = 0; i < requests.length; i++){
					if (requests[i].commandId === commandId){
						count++
					}
				}

				return count
			}
		}
	}

	function createController(){
		return createTemporaryObject(controllerComp, testCase)
	}

	function message(kind, fields){
		let result = {"formatVersion": 1, "id": "m-" + kind, "kind": kind}
		for (let key in fields){
			result[key] = fields[key]
		}

		return result
	}

	function messagesJson(list){
		return JSON.stringify(list)
	}

	function messageAt(controller, index){
		return JSON.parse(controller.messagesModel.get(index).messageJson)
	}

	function startedController(){
		let controller = createController()
		controller.startConversation()
		controller.handleResponse("AssistantStartConversation", "c-1", "[]")
		compare(controller.conversationId, "c-1")
		compare(controller.busy, false)

		return controller
	}

	function test_firstMessageStartsConversation(){
		let controller = createController()

		verify(controller.sendMessage("  Show open tasks  "))
		compare(controller.lastRequest().commandId, "AssistantStartConversation")
		verify(controller.busy)

		controller.handleResponse("AssistantStartConversation", "c-1", "[]")
		compare(controller.conversationId, "c-1")
		compare(controller.lastRequest().commandId, "AssistantSendMessage")
		compare(controller.lastRequest().input.conversationId, "c-1")
		compare(controller.lastRequest().input.text, "Show open tasks")
		verify(controller.busy)
	}

	function test_disabledAssistantDoesNotSend(){
		let controller = createController()
		controller.sendMessage("Hello")

		let error = message("error", {"code": "AssistantDisabled"})
		controller.handleResponse("AssistantStartConversation", "", messagesJson([error]))

		compare(controller.conversationId, "")
		compare(controller.countRequests("AssistantSendMessage"), 0)
		compare(controller.busy, false)
		compare(messageAt(controller, 0).code, "AssistantDisabled")
	}

	function test_emptyOrBusyMessageIsRefused(){
		let controller = startedController()

		verify(!controller.sendMessage("   "))
		verify(controller.sendMessage("First"))
		verify(!controller.sendMessage("Second"))
		compare(controller.countRequests("AssistantSendMessage"), 1)
	}

	function test_contextIsVisibleRemovableAndSentOnce(){
		let controller = startedController()

		controller.addContextItem("selection", "Task 1", {"id": "task-1"})
		controller.addContextItem("document", "Report", {"id": "doc-1"})
		controller.addContextItem("selection", "Task 2", {"id": "task-2"})
		compare(controller.contextItems.length, 2)
		compare(controller.contextItems[1].label, "Task 2")

		controller.removeContextItem(0)
		compare(controller.contextItems.length, 1)

		controller.sendMessage("Mark it done")
		let items = controller.lastRequest().input.contextItems
		compare(items.length, 1)
		compare(items[0].kind, "selection")
		compare(JSON.parse(items[0].dataJson).id, "task-2")

		// The context belongs to the sent message only.
		compare(controller.contextItems.length, 0)
	}

	function test_messagesAreValidated(){
		let controller = startedController()

		let valid = message("assistantText", {"text": "Hello"})
		let wrongVersion = message("assistantText", {"text": "Hello", "formatVersion": 2})
		let unknownKind = message("qmlComponent", {"text": "Item {}"})

		controller.handleResponse("AssistantSendMessage", "c-1", messagesJson([valid, wrongVersion, unknownKind, "text"]))

		compare(controller.messagesModel.count, 4)
		compare(messageAt(controller, 0).kind, "assistantText")
		compare(messageAt(controller, 1).code, "InvalidMessage")
		compare(messageAt(controller, 2).code, "InvalidMessage")
		compare(messageAt(controller, 3).code, "InvalidMessage")
	}

	function test_invalidResponseIsVisible(){
		let controller = startedController()
		controller.sendMessage("Hello")

		controller.handleResponse("AssistantSendMessage", "c-1", "{not json")
		compare(controller.busy, false)
		compare(messageAt(controller, 0).code, "InvalidResponse")
	}

	function test_confirmationIsSentOnce(){
		let controller = startedController()

		let preview = message("actionPreview", {"previewId": "p-1", "text": "Change status"})
		controller.handleResponse("AssistantSendMessage", "c-1", messagesJson([preview]))
		compare(controller.getPreviewState("p-1"), "")

		controller.confirmPreview("p-1")
		controller.confirmPreview("p-1")
		controller.rejectPreview("p-1")
		compare(controller.countRequests("AssistantConfirmPreview"), 1)
		compare(controller.countRequests("AssistantRejectPreview"), 0)
		compare(controller.lastRequest().input.previewId, "p-1")
		compare(controller.getPreviewState("p-1"), "confirming")

		let executed = message("actionExecuted", {"previewId": "p-1", "code": "succeeded"})
		controller.handleResponse("AssistantConfirmPreview", "c-1", messagesJson([executed]))
		compare(controller.getPreviewState("p-1"), "decided")
	}

	function test_rejectionIsSentOnce(){
		let controller = startedController()

		controller.rejectPreview("p-2")
		controller.rejectPreview("p-2")
		controller.confirmPreview("p-2")
		compare(controller.countRequests("AssistantRejectPreview"), 1)
		compare(controller.countRequests("AssistantConfirmPreview"), 0)
	}

	function test_lostConfirmationIsNotReportedAsSuccess(){
		let controller = startedController()

		controller.confirmPreview("p-1")
		controller.handleError("AssistantConfirmPreview", "Network error")

		compare(messageAt(controller, 0).kind, "error")
		compare(messageAt(controller, 0).code, "ConfirmationStatusUnknown")
		compare(controller.getPreviewState("p-1"), "confirming")

		for (let i = 0; i < controller.messagesModel.count; i++){
			verify(messageAt(controller, i).kind !== "actionExecuted")
		}
	}

	function test_cancelOnlyWhileBusy(){
		let controller = startedController()

		controller.cancel()
		compare(controller.countRequests("AssistantCancelRequest"), 0)

		controller.sendMessage("Hello")
		controller.cancel()
		compare(controller.countRequests("AssistantCancelRequest"), 1)
		compare(controller.lastRequest().input.conversationId, "c-1")

		let canceled = message("error", {"code": "Canceled"})
		controller.handleResponse("AssistantSendMessage", "c-1", messagesJson([canceled]))
		compare(controller.busy, false)
	}

	function test_networkErrorEndsProcessing(){
		let controller = startedController()

		controller.sendMessage("Hello")
		controller.handleError("AssistantSendMessage", "Network error")
		compare(controller.busy, false)
		compare(messageAt(controller, 0).code, "NetworkError")
	}

	function test_historyReplacesMessages(){
		let controller = startedController()

		controller.handleResponse("AssistantSendMessage", "c-1", messagesJson([message("assistantText", {"text": "Old"})]))
		controller.confirmPreview("p-1")

		controller.refreshHistory()
		compare(controller.lastRequest().commandId, "AssistantGetHistory")

		let preview = message("actionPreview", {"previewId": "p-1"})
		let executed = message("actionExecuted", {"previewId": "p-1", "code": "stale"})
		controller.handleResponse("AssistantGetHistory", "c-1", messagesJson([preview, executed]))

		compare(controller.messagesModel.count, 2)
		compare(controller.getPreviewState("p-1"), "decided")
	}

	function test_newConversationClearsState(){
		let controller = startedController()
		controller.handleResponse("AssistantSendMessage", "c-1", messagesJson([message("assistantText", {"text": "Hi"})]))
		controller.addContextItem("selection", "Task 1", {"id": "task-1"})

		controller.newConversation()
		compare(controller.lastRequest().commandId, "AssistantCloseConversation")
		compare(controller.lastRequest().input.conversationId, "c-1")
		compare(controller.conversationId, "")
		compare(controller.messagesModel.count, 0)
		compare(controller.contextItems.length, 0)
	}
}
