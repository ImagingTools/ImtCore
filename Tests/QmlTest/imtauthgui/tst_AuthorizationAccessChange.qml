import QtQuick 2.12
import QtTest 1.12
import imtcontrols 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtqmltest 1.0

// A change of the access of the session (granted or taken permissions) applies at once: the client reads its permissions again
TestCase {
	id: testCase

	name: "AuthorizationAccessChange"

	property int accessChangedCount: 0
	property var registeredCommands: []
	property var unregisteredCommands: []

	Connections {
		target: AuthorizationController

		function onAccessChanged(){
			testCase.accessChangedCount++
		}
	}

	function onRegister(parameters){
		let commands = testCase.registeredCommands.slice()
		commands.push(parameters["Client"].gqlCommandId)
		testCase.registeredCommands = commands
	}

	function onUnregister(client){
		let commands = testCase.unregisteredCommands.slice()
		commands.push(client.gqlCommandId)
		testCase.unregisteredCommands = commands
	}

	function init(){
		GqlTestTransport.reset()
		testCase.accessChangedCount = 0
		testCase.registeredCommands = []
		testCase.unregisteredCommands = []
		Events.subscribeEvent("RegisterSubscription", testCase.onRegister)
		Events.subscribeEvent("UnregisterSubscription", testCase.onUnregister)
	}

	function cleanup(){
		Events.unSubscribeEvent("RegisterSubscription", testCase.onRegister)
		Events.unSubscribeEvent("UnregisterSubscription", testCase.onUnregister)
		AuthorizationController.logoutForce()
		GqlTestTransport.reset()
	}

	function signIn(){
		AuthorizationController.userTokenProvider.login = "user"
		AuthorizationController.userTokenProvider.userId = "user-id"
		AuthorizationController.userTokenProvider.accessToken = "token"
		AuthorizationController.userTokenProvider.refreshToken = "refresh"
		AuthorizationController.setAccessToken("token")
		AuthorizationController.userTokenProvider.permissions = ["/Old"]
	}

	function answerPermissions(permissions){
		GqlTestTransport.queueResponse("GetPermissions", JSON.stringify({"data": {"GetPermissions": {"permissions": permissions}}}))
	}

	function requestCount(commandId){
		return GqlTestTransport.requestedCommands().filter(function(command){ return command === commandId }).length
	}

	function test_signedInSessionWatchesItsAccess(){
		signIn()
		AuthorizationController.loggedIn()

		verify(testCase.registeredCommands.indexOf("OnAccessChanged") >= 0, "Registered: " + testCase.registeredCommands)

		AuthorizationController.logoutForce()
		verify(testCase.unregisteredCommands.indexOf("OnAccessChanged") >= 0, "Ended with the session: " + testCase.unregisteredCommands)
	}

	function test_changeReadsThePermissionsAgain_data(){
		return [
			{"tag": "the server tells a change", "signalName": "messageReceived", "permissions": "/Old;/Granted", "told": 1},
			{"tag": "the server tells a change of other access", "signalName": "messageReceived", "permissions": "/Old", "told": 1},
			{"tag": "changes missed while disconnected", "signalName": "refreshRequired", "permissions": "/Old;/Granted", "told": 1},
			// A reconnect with nothing changed rebuilds nothing
			{"tag": "reconnected, nothing changed", "signalName": "refreshRequired", "permissions": "/Old", "told": 0}
		]
	}

	function test_changeReadsThePermissionsAgain(data){
		signIn()
		answerPermissions(data.permissions)

		if (data.signalName === "messageReceived"){
			AuthorizationController.__accessSubscription.messageReceived(null)
		}
		else{
			AuthorizationController.__accessSubscription.refreshRequired()
		}

		tryVerify(function(){ return requestCount("GetPermissions") === 1 }, 3000)
		tryCompare(testCase, "accessChangedCount", data.told, 3000)
		wait(200)
		compare(testCase.accessChangedCount, data.told)
		compare(AuthorizationController.getPermissions(), data.permissions.split(";"), "The new permissions before it is told")
	}

	function test_noReadWithoutASession(){
		AuthorizationController.logoutForce()

		AuthorizationController.__accessSubscription.messageReceived(null)

		wait(200)
		compare(requestCount("GetPermissions"), 0)
		compare(testCase.accessChangedCount, 0)
	}

	function test_pendingPermissionReadEndsWithItsSession(){
		signIn()
		answerPermissions("/OldSession")
		GqlTestTransport.queueDelay("GetPermissions", 250)
		AuthorizationController.__accessSubscription.messageReceived(null)
		tryVerify(function(){ return requestCount("GetPermissions") === 1 })

		AuthorizationController.logoutForce()
		signIn()
		answerPermissions("/Current")
		AuthorizationController.__accessSubscription.messageReceived(null)
		tryCompare(testCase, "accessChangedCount", 1)
		wait(350)
		compare(AuthorizationController.getPermissions(), ["/Current"])
		compare(testCase.accessChangedCount, 1)
	}
}
