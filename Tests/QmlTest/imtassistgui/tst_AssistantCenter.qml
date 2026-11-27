import QtQuick 2.12
import QtTest 1.12
import imtcontrols 1.0
import imtauthgui 1.0
import imtassistgui 1.0

TestCase {
	id: testCase

	name: "AssistantCenter"

	property var states: []

	Component {
		id: controllerComp

		AssistantController {
			property var requests: []

			// reimplemented (AssistantController)
			function sendRequest(commandId, input){
				let list = requests.slice()
				list.push(commandId)
				requests = list
			}
		}
	}

	function onState(state){
		let list = testCase.states.slice()
		list.push(state)
		testCase.states = list
	}

	function initTestCase(){
		Events.subscribeEvent("AssistantStateChanged", testCase.onState)
	}

	function cleanupTestCase(){
		Events.unSubscribeEvent("AssistantStateChanged", testCase.onState)
	}

	function init(){
		AssistantCenter.stop()
		AssistantCenter.controller = null
		testCase.states = []
	}

	function cleanup(){
		AssistantCenter.stop()
		AssistantCenter.controller = null
	}

	function availableController(){
		let controller = createTemporaryObject(controllerComp, testCase)
		AssistantCenter.controller = controller
		controller.handleStatus(true)

		return controller
	}

	function test_availabilityPublishesState(){
		let controller = availableController()

		verify(AssistantCenter.active)
		verify(!AssistantCenter.panelVisible)
		verify(testCase.states.length > 0)
		verify(testCase.states[testCase.states.length - 1].active)

		// Views created later ask for the state
		testCase.states = []
		Events.sendEvent("AssistantStateRequested")
		compare(testCase.states.length, 1)
		verify(testCase.states[0].active)

		controller.handleStatus(false)
		verify(!AssistantCenter.active)
		verify(!testCase.states[testCase.states.length - 1].active)
	}

	function test_openWithContextOfPage(){
		let controller = availableController()

		Events.sendEvent("OpenAssistant", [{"kind": "administration", "label": "Administration: Users", "data": {"page": "Administration/Users", "selectedIds": ["u1"]}}])

		verify(AssistantCenter.panelVisible)
		compare(controller.contextItems.length, 1)
		compare(controller.contextItems[0].kind, "administration")
		compare(controller.contextItems[0].data.selectedIds[0], "u1")
		verify(testCase.states[testCase.states.length - 1].panelVisible)

		// An item without kind is not released
		Events.sendEvent("OpenAssistant", [{"label": "no kind"}])
		compare(controller.contextItems.length, 1)

		AssistantCenter.toggle()
		verify(!AssistantCenter.panelVisible)

		AssistantCenter.toggle()
		verify(AssistantCenter.panelVisible)
	}

	function test_notOpenedWhileDisabled(){
		let controller = createTemporaryObject(controllerComp, testCase)
		AssistantCenter.controller = controller
		controller.handleStatus(false)

		Events.sendEvent("OpenAssistant", [{"kind": "administration", "label": "Users", "data": {}}])

		verify(!AssistantCenter.panelVisible)
		compare(controller.contextItems.length, 0)

		// A disabled assistant closes the open panel
		controller.handleStatus(true)
		AssistantCenter.open([])
		verify(AssistantCenter.panelVisible)
		controller.handleStatus(false)
		verify(!AssistantCenter.panelVisible)
	}

	function test_statusErrorDisables(){
		let controller = availableController()

		controller.handleError("AssistantGetStatus", "network")

		verify(!AssistantCenter.active)
		compare(controller.messagesModel.count, 0)
	}

	function test_stopEndsConversation(){
		let controller = availableController()
		controller.startConversation()
		controller.handleResponse("AssistantStartConversation", "c-1", "[]")
		compare(controller.conversationId, "c-1")

		AssistantCenter.open([{"kind": "administration", "label": "Users", "data": {}}])
		AssistantCenter.stop()

		verify(!AssistantCenter.active)
		verify(!AssistantCenter.panelVisible)
		compare(controller.conversationId, "")
		compare(controller.contextItems.length, 0)
		verify(controller.requests.indexOf("AssistantCloseConversation") >= 0)
	}

	function test_refreshAsksTheServer(){
		if (!AuthorizationController.isLoggedIn()){
			skip("The availability is checked for a signed-in user only")
		}

		let controller = createTemporaryObject(controllerComp, testCase)
		AssistantCenter.controller = controller

		verify(controller.requests.indexOf("AssistantGetStatus") >= 0)
	}
}
