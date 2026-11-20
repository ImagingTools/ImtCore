import QtQuick 2.12
import QtTest 1.12
import imtgui 1.0
import imtcontrols 1.0

// A state other than connected is shown only when it lasts: a connection opened again at once shows nothing
TestCase {
	id: testCase

	name: "ConnectionStateMachine"

	property int lostCount: 0
	property var app
	property var machine

	Component {
		id: appComp
		ApplicationMain {}
	}

	function onLost(){
		testCase.lostCount++
	}

	function init(){
		testCase.app = createTemporaryObject(appComp, testCase)
		verify(testCase.app !== null)
		testCase.machine = findChild(testCase.app, "connectionState")
		verify(testCase.machine !== null)
		testCase.lostCount = 0
		Events.subscribeEvent("ServerConnectionError", testCase.onLost)
	}

	function cleanup(){
		Events.unSubscribeEvent("ServerConnectionError", testCase.onLost)
	}

	function test_shortDropShowsNothing(){
		testCase.machine.appConnectionState = 3
		wait(100)
		testCase.machine.appConnectionState = 1
		wait(3200)
		compare(testCase.lostCount, 0)
	}

	function test_lastingDropIsShown(){
		testCase.machine.appConnectionState = 3
		wait(1100)
		compare(testCase.lostCount, 0)
		tryCompare(testCase, "lostCount", 1, 3500)
		testCase.machine.appConnectionState = 1
		compare(testCase.machine.totalStatus, testCase.machine.status.CONNECTION_SUCCESFUL)
	}

	function test_unknownPortIsConnecting(){
		testCase.app.useWebSocketSubscription = true
		testCase.app.webSocketPort = -1
		compare(testCase.machine.totalStatus, testCase.machine.status.TRY_CONNECTING_TO_APP_SERVER)
	}
}
