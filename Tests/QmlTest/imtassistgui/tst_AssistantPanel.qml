import QtQuick 2.12
import QtTest 1.12
import imtassistgui 1.0

TestCase {
	id: testCase

	name: "AssistantPanel"
	when: windowShown

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
		}
	}

	Component {
		id: panelComp

		AssistantPanel {
			width: 400
			height: 600
		}
	}

	function createPanel(){
		let controller = createTemporaryObject(controllerComp, testCase)
		controller.startConversation()
		controller.handleResponse("AssistantStartConversation", "c-1", "[]")

		// TestCase itself is invisible, views need a visible parent
		let panel = createTemporaryObject(panelComp, testCase.parent, {"controller": controller})
		verify(panel !== null)

		return panel
	}

	function message(kind, fields){
		let result = {"formatVersion": 1, "id": "m-" + kind, "kind": kind}
		for (let key in fields){
			result[key] = fields[key]
		}

		return result
	}

	function deliver(panel, messages){
		panel.controller.handleResponse("AssistantSendMessage", "c-1", JSON.stringify(messages))
		wait(0)
	}

	function listResult(type){
		return message("actionResult", {
			"actionId": "tasks.search",
			"text": "2 tasks found",
			"representation": {
				"type": type,
				"version": 1,
				"data": {"items": [{"id": "task-1", "label": "Prepare release notes"}, {"id": "task-2", "label": "Review pull request"}]}
			}
		})
	}

	function findAll(item, objectName, result){
		if (item.objectName === objectName){
			result.push(item)
		}

		let children = item.children
		for (let i = 0; i < children.length; i++){
			findAll(children[i], objectName, result)
		}

		return result
	}

	function findVisible(item, objectName){
		let items = findAll(item, objectName, [])
		for (let i = 0; i < items.length; i++){
			if (items[i].visible){
				return items[i]
			}
		}

		return null
	}

	function test_registeredRepresentationHandsOverSelection(){
		let panel = createPanel()
		deliver(panel, [listResult("imtassist.selectableList")])

		let list = findVisible(panel, "AssistantSelectableList")
		verify(list !== null, "registered list is shown")
		compare(findVisible(panel, "AssistantRepresentationError"), null)

		list.toggleSelection("task-2")
		let handOverButton = findVisible(list, "AssistantHandOverSelection")
		verify(handOverButton.enabled)
		handOverButton.clicked()

		compare(panel.controller.contextItems.length, 1)
		compare(panel.controller.contextItems[0].kind, "selection")
		compare(panel.controller.contextItems[0].label, "Review pull request")
		compare(panel.controller.contextItems[0].data.id, "task-2")
		verify(findVisible(panel, "AssistantContext") !== null, "released context is visible")
	}

	function test_unknownRepresentationIsRejected(){
		let panel = createPanel()
		deliver(panel, [listResult("qml.inline")])

		compare(findVisible(panel, "AssistantSelectableList"), null)
		verify(findVisible(panel, "AssistantRepresentationError") !== null)
	}

	function test_invalidRepresentationDataIsRejected(){
		let panel = createPanel()
		let result = listResult("imtassist.selectableList")
		result.representation.data = {"items": [{"label": "No id"}]}
		deliver(panel, [result])

		compare(findVisible(panel, "AssistantSelectableList"), null)
		verify(findVisible(panel, "AssistantRepresentationError") !== null)
	}

	function test_contextCanBeRemoved(){
		let panel = createPanel()
		panel.addContext("selection", "Prepare release notes", {"id": "task-1"})
		wait(0)

		let removeButton = findVisible(panel, "AssistantRemoveContext")
		verify(removeButton !== null)
		removeButton.clicked()

		compare(panel.controller.contextItems.length, 0)
		compare(findVisible(panel, "AssistantContext"), null)
	}

	function test_previewIsConfirmedExplicitly(){
		let panel = createPanel()
		let preview = message("actionPreview", {
			"previewId": "p-1",
			"actionId": "tasks.setStatus",
			"text": "Change status of 'Prepare release notes'",
			"data": {"targetLabel": "Prepare release notes", "changes": [{"field": "status", "oldValue": "open", "newValue": "done"}], "impact": "Moved to done"}
		})
		deliver(panel, [preview])

		let card = findVisible(panel, "AssistantPreviewCard")
		verify(card !== null)
		compare(card.changes.length, 1)
		compare(panel.controller.lastRequest().commandId, "AssistantStartConversation")

		let confirmButton = findVisible(card, "AssistantConfirmButton")
		confirmButton.clicked()
		compare(panel.controller.lastRequest().commandId, "AssistantConfirmPreview")
		compare(panel.controller.lastRequest().input.previewId, "p-1")

		// No second confirmation while the decision is pending.
		wait(0)
		compare(findVisible(card, "AssistantConfirmButton"), null)
	}

	function test_busyStateCanBeCancelled(){
		let panel = createPanel()
		compare(findVisible(panel, "AssistantBusy"), null)

		panel.controller.sendMessage("Show open tasks")
		wait(0)

		let busyRow = findVisible(panel, "AssistantBusy")
		verify(busyRow !== null)
		findVisible(busyRow, "AssistantCancel").clicked()
		compare(panel.controller.lastRequest().commandId, "AssistantCancelRequest")
	}

	function test_errorsAreShownByCode(){
		let panel = createPanel()
		deliver(panel, [message("error", {"code": "Timeout", "text": "technical"})])

		let texts = findAll(panel, "AssistantMessageText", [])
		let found = false
		for (let i = 0; i < texts.length; i++){
			if (texts[i].visible && texts[i].text.indexOf("in time") >= 0){
				found = true
			}
		}

		verify(found, "timeout is explained to the user")
	}

	function test_panelCanBeRecreatedWithoutLosingTheConversation(){
		let panel = createPanel()
		let controller = panel.controller
		deliver(panel, [message("assistantText", {"text": "Hello"})])

		panel.destroy()
		wait(0)

		let newPanel = createTemporaryObject(panelComp, testCase.parent, {"controller": controller})
		wait(0)
		compare(controller.messagesModel.count, 1)
		verify(findAll(newPanel, "AssistantMessageDelegate", []).length >= 1)
	}
}
