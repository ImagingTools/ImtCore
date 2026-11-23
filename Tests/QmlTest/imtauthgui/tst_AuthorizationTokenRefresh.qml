import QtQuick 2.12
import QtTest 1.12
import imtcontrols 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtguigql 1.0
import imtqmltest 1.0
import imtauthAuthorizationSdl 1.0

// The access token expires (401): the client refreshes it and sends again what was refused.
// The refresh token expires (refresh refused): the client signs out. And what can go wrong between the two.
TestCase {
	id: testCase

	name: "AuthorizationTokenRefresh"

	property int loggedOutCount: 0
	property int loggedInCount: 0
	property int tokenRefreshedCount: 0
	property var superuserResults: []
	// Signs out the moment the client asks to refresh, so the answer comes after the sign-out
	property bool isSignOutOnRefreshRequest: false
	// Another tab rotates the shared refresh token the moment this one asks for a refresh
	property bool isRotatedElsewhereOnRefreshRequest: false

	Connections {
		target: AuthorizationController

		function onLoggedOut(){
			testCase.loggedOutCount++
		}

		function onLoggedIn(){
			testCase.loggedInCount++
		}

		function onSuperuserExistResult(status, error){
			let results = testCase.superuserResults.slice()
			results.push(status)
			testCase.superuserResults = results
		}
	}

	Connections {
		target: GqlTestTransport

		function onRequestReceived(){
			let commands = GqlTestTransport.requestedCommands()
			if (testCase.isSignOutOnRefreshRequest && commands.length > 0 && commands[commands.length - 1] === "RefreshToken"){
				testCase.isSignOutOnRefreshRequest = false
				AuthorizationController.logoutForce()
			}
			if (testCase.isRotatedElsewhereOnRefreshRequest && commands.length > 0 && commands[commands.length - 1] === "RefreshToken"){
				testCase.isRotatedElsewhereOnRefreshRequest = false
				AuthorizationController.storage.setValue("refreshToken", "refresh-of-another-tab")
			}
		}
	}

	function onTokenRefreshed(parameters){
		testCase.tokenRefreshedCount++
	}

	Component {
		id: devicesSenderComp

		GqlSdlRequestSender {
			gqlCommandId: "DevicesList"
			permissionPath: "/Devices"
		}
	}

	Component {
		id: ordersSenderComp

		GqlSdlRequestSender {
			gqlCommandId: "OrdersList"
			permissionPath: "/Orders"
		}
	}

	// A request whose caller waits for its end
	Component {
		id: endingSenderComp

		GqlSdlRequestSender {
			id: endingSender

			property var statuses: []

			gqlCommandId: "DevicesList"
			permissionPath: "/Devices"

			sdlObjectComp: Component {
				LogoutPayload {}
			}

			onFinished: {
				let statuses = endingSender.statuses.slice()
				statuses.push(status)
				endingSender.statuses = statuses
			}
		}
	}

	function init(){
		GqlTestTransport.reset()
		AuthorizationController.refreshRetryInterval = 100
		signIn()
		testCase.loggedOutCount = 0
		testCase.loggedInCount = 0
		testCase.tokenRefreshedCount = 0
		testCase.superuserResults = []
		testCase.isSignOutOnRefreshRequest = false
		Events.subscribeEvent("AccessTokenRefreshed", testCase.onTokenRefreshed)
	}

	function cleanup(){
		Events.unSubscribeEvent("AccessTokenRefreshed", testCase.onTokenRefreshed)
		testCase.isSignOutOnRefreshRequest = false
		testCase.isRotatedElsewhereOnRefreshRequest = false
		AuthorizationController.logoutForce()
		AuthorizationController.clearRefreshToken()
		// What another tab of a browser stores; the desktop has no remove()
		AuthorizationController.storage.setValue("refreshToken", "")
		AuthorizationController.storage.setValue("accessToken", "")
		GqlTestTransport.reset()
	}

	function signIn(){
		AuthorizationController.userTokenProvider.login = "user"
		AuthorizationController.userTokenProvider.userId = "user-id"
		AuthorizationController.userTokenProvider.accessToken = "expired-token"
		AuthorizationController.userTokenProvider.refreshToken = "refresh-1"
		AuthorizationController.setAccessToken("expired-token")
	}

	function answerRefresh(isAccepted, accessToken, tenantId){
		let session = isAccepted ? {"userId": "user-id", "tenantId": tenantId || "", "accessToken": accessToken || "fresh-token", "refreshToken": "refresh-2"} : null
		GqlTestTransport.queueResponse("RefreshToken", JSON.stringify({"data": {"RefreshToken": {"ok": isAccepted, "userSession": session}}}))
	}

	function requestsOf(commandId){
		return GqlTestTransport.requests().filter(function(request){ return request.commandId === commandId })
	}

	function test_refusedRequestIsSentAgainWithTheNewToken(){
		GqlTestTransport.queueStatus("DevicesList", 401)
		answerRefresh(true)
		let sender = createTemporaryObject(devicesSenderComp, testCase)

		sender.send()

		tryVerify(function(){ return requestsOf("DevicesList").length === 2 }, 5000, "The refused request is sent again")
		let refused = requestsOf("DevicesList")[0]
		let repeated = requestsOf("DevicesList")[1]
		compare(refused.headers["x-authentication-token"], "expired-token")
		compare(repeated.body, refused.body, "The same request")
		compare(repeated.headers["permissionpath"], "/Devices", "With the permission path of its sender")
		compare(repeated.headers["x-request-id"], refused.headers["x-request-id"], "With the id of its sender")
		compare(repeated.headers["x-authentication-token"], "fresh-token", "With the new token")

		let refresh = requestsOf("RefreshToken")
		compare(refresh.length, 1)
		verify(!refresh[0].headers["x-authentication-token"], "The expired token is not sent to refresh it")
		let commands = GqlTestTransport.requestedCommands()
		verify(commands.indexOf("RefreshToken") < commands.lastIndexOf("DevicesList"), "Sent again after the refresh: " + commands)
		compare(testCase.loggedOutCount, 0)
	}

	function test_requestsRefusedTogetherShareOneRefresh(){
		GqlTestTransport.queueStatus("DevicesList", 401)
		GqlTestTransport.queueStatus("OrdersList", 401)
		answerRefresh(true)
		let devices = createTemporaryObject(devicesSenderComp, testCase)
		let orders = createTemporaryObject(ordersSenderComp, testCase)

		devices.send()
		orders.send()

		tryVerify(function(){ return requestsOf("DevicesList").length === 2 && requestsOf("OrdersList").length === 2 }, 5000, "Both are sent again")
		compare(requestsOf("RefreshToken").length, 1, "One refresh for both")
		compare(requestsOf("DevicesList")[1].headers["permissionpath"], "/Devices")
		compare(requestsOf("OrdersList")[1].headers["permissionpath"], "/Orders")

		wait(300)
		compare(requestsOf("DevicesList").length, 2, "Sent again once")
		compare(requestsOf("OrdersList").length, 2, "Sent again once")
	}

	function test_refusedRefreshSignsOut(){
		GqlTestTransport.queueStatus("DevicesList", 401)
		answerRefresh(false)
		let sender = createTemporaryObject(devicesSenderComp, testCase)

		sender.send()

		tryCompare(testCase, "loggedOutCount", 1, 5000)
		wait(300)
		compare(requestsOf("DevicesList").length, 1, "Nothing is sent again without a session")
		compare(AuthorizationController.userTokenProvider.accessToken, "")
		compare(AuthorizationController.userTokenProvider.refreshToken, "")
		compare(testCase.tokenRefreshedCount, 0, "No subscription starts again")
	}

	// A request sent while the refresh is under way carries the expired token too
	function test_requestSentDuringTheRefreshWaitsForIt(){
		GqlTestTransport.queueStatus("DevicesList", 401)
		GqlTestTransport.queueStatus("OrdersList", 401)
		answerRefresh(true)
		let devices = createTemporaryObject(devicesSenderComp, testCase)
		let orders = createTemporaryObject(ordersSenderComp, testCase)

		// The refresh is answered late: the second request goes out while it is under way
		GqlTestTransport.queueDelay("RefreshToken", 500)
		devices.send()
		tryVerify(function(){ return requestsOf("RefreshToken").length === 1 }, 5000)
		orders.send()
		tryVerify(function(){ return requestsOf("OrdersList").length === 1 }, 5000)
		compare(requestsOf("OrdersList").length, 1, "Not sent again before the refresh ends")

		tryVerify(function(){ return requestsOf("OrdersList").length === 2 }, 5000, "Sent again after the refresh")
		compare(requestsOf("OrdersList")[0].headers["x-authentication-token"], "expired-token", "It went out before the refresh ended")
		compare(requestsOf("RefreshToken").length, 1, "No second refresh")
		compare(requestsOf("OrdersList")[1].headers["x-authentication-token"], "fresh-token")
		compare(requestsOf("DevicesList").length, 2)
	}

	// Signed out while the refresh was under way: its answer must not sign in again
	function test_refreshAnsweredAfterSignOutIsIgnored(){
		GqlTestTransport.queueStatus("DevicesList", 401)
		answerRefresh(true)
		testCase.isSignOutOnRefreshRequest = true
		let sender = createTemporaryObject(devicesSenderComp, testCase)

		sender.send()

		tryCompare(testCase, "loggedOutCount", 1, 5000)
		wait(300)
		compare(AuthorizationController.userTokenProvider.accessToken, "", "No token after the sign-out")
		compare(AuthorizationController.userTokenProvider.refreshToken, "", "No refresh token after the sign-out")
		compare(AuthorizationController.getAccessToken(), "")
		compare(requestsOf("DevicesList").length, 1, "Nothing is sent again")
		compare(testCase.tokenRefreshedCount, 0)
	}

	// The server could not be reached for the refresh: the session is kept and the refresh asked again
	function test_unreachableRefreshKeepsTheSession(){
		GqlTestTransport.queueStatus("DevicesList", 401)
		GqlTestTransport.queueStatus("RefreshToken", 500)
		answerRefresh(false)
		answerRefresh(true)
		let sender = createTemporaryObject(devicesSenderComp, testCase)

		sender.send()

		tryVerify(function(){ return requestsOf("DevicesList").length === 2 }, 5000, "Sent again after the second refresh")
		compare(requestsOf("RefreshToken").length, 2)
		compare(requestsOf("DevicesList")[1].headers["x-authentication-token"], "fresh-token")
		compare(testCase.loggedOutCount, 0, "Not signed out for a server out of reach")
	}

	// Refused again with the new token: the token is not the cause, it is not refreshed and sent again in a loop
	function test_refusedAgainAfterTheRefreshIsNotRepeated(){
		GqlTestTransport.queueStatus("DevicesList", 401)
		GqlTestTransport.queueStatus("DevicesList", 401)
		GqlTestTransport.queueStatus("DevicesList", 401)
		answerRefresh(true)
		answerRefresh(true)
		let sender = createTemporaryObject(devicesSenderComp, testCase)

		sender.send()

		tryVerify(function(){ return requestsOf("DevicesList").length === 2 }, 5000)
		wait(500)
		compare(requestsOf("DevicesList").length, 2, "Sent again once")
		compare(requestsOf("RefreshToken").length, 1, "Refreshed once")
	}

	// A new send on the same sender has a new request ID, even with an identical body.
	function test_newSendOfTheSameQueryCanRefreshAgain(){
		GqlTestTransport.queueStatus("DevicesList", 401)
		answerRefresh(true)
		let sender = createTemporaryObject(devicesSenderComp, testCase)
		sender.send()
		tryVerify(function(){ return requestsOf("DevicesList").length === 2 }, 5000)

		GqlTestTransport.queueStatus("DevicesList", 401)
		GqlTestTransport.queueResponse("RefreshToken", JSON.stringify({"data": {"RefreshToken": {"ok": true,
			"userSession": {"userId": "user-id", "tenantId": "", "accessToken": "fresher-token", "refreshToken": "refresh-3"}}}}))
		sender.send()

		tryVerify(function(){ return requestsOf("DevicesList").length === 4 }, 5000, "Refreshed and sent again")
		compare(requestsOf("RefreshToken").length, 2)
		compare(requestsOf("RefreshToken")[1].body.indexOf("refresh-2") >= 0, true, "With the rotated refresh token")
		compare(requestsOf("DevicesList")[3].headers["x-authentication-token"], "fresher-token")
	}

	function test_lateRefusalDoesNotActOnNewSession_data(){
		return [{"tag": "Unauthorized", "code": 401}, {"tag": "Forbidden", "code": 403}]
	}

	function test_lateRefusalDoesNotActOnNewSession(data){
		GqlTestTransport.queueStatus("DevicesList", data.code)
		GqlTestTransport.queueDelay("DevicesList", 200)
		let sender = createTemporaryObject(endingSenderComp, testCase)
		sender.send()
		AuthorizationController.logoutForce()
		AuthorizationController.userTokenProvider.userId = "second-user"
		AuthorizationController.userTokenProvider.accessToken = "second-token"
		AuthorizationController.userTokenProvider.refreshToken = "second-refresh"
		AuthorizationController.setAccessToken("second-token")

		tryCompare(sender, "statuses", [-1], 3000)
		compare(requestsOf("RefreshToken").length, 0)
		compare(requestsOf("DevicesList").length, 1)
		compare(AuthorizationController.getAccessToken(), "second-token")
	}

	function test_withoutRefreshTokenSignsOutAtOnce(){
		AuthorizationController.userTokenProvider.refreshToken = ""
		GqlTestTransport.queueStatus("DevicesList", 401)
		let sender = createTemporaryObject(devicesSenderComp, testCase)

		sender.send()

		tryCompare(testCase, "loggedOutCount", 1, 5000)
		compare(requestsOf("RefreshToken").length, 0)
	}

	// A stale request of a signed-out client is refused: nothing to refresh, nobody to sign out
	function test_refusalWhileSignedOutDoesNothing(){
		AuthorizationController.logoutForce()
		testCase.loggedOutCount = 0
		GqlTestTransport.queueStatus("DevicesList", 401)
		let sender = createTemporaryObject(devicesSenderComp, testCase)

		sender.send()

		tryVerify(function(){ return requestsOf("DevicesList").length === 1 }, 5000)
		wait(300)
		compare(requestsOf("RefreshToken").length, 0)
		compare(testCase.loggedOutCount, 0)
	}

	// The remembered refresh token follows the rotation, or the next start signs in with a used one
	function test_rememberedRefreshTokenFollowsTheRotation(){
		AuthorizationController.rememberMe = true
		AuthorizationController.storedRefreshToken = "refresh-1"
		GqlTestTransport.queueStatus("DevicesList", 401)
		answerRefresh(true)
		let sender = createTemporaryObject(devicesSenderComp, testCase)

		sender.send()

		tryCompare(AuthorizationController, "storedRefreshToken", "refresh-2", 5000)
		compare(AuthorizationController.rememberMe, true)
	}

	function test_refusedRefreshForgetsTheRememberedToken(){
		AuthorizationController.rememberMe = true
		AuthorizationController.storedRefreshToken = "refresh-1"
		GqlTestTransport.queueStatus("DevicesList", 401)
		answerRefresh(false)
		let sender = createTemporaryObject(devicesSenderComp, testCase)

		sender.send()

		tryCompare(testCase, "loggedOutCount", 1, 5000)
		compare(AuthorizationController.storedRefreshToken, "")
	}

	// A subscription refused as Unauthorized asks for a new token: the subscriptions start again with it
	function test_subscriptionRefusalRefreshesTheToken(){
		answerRefresh(true)

		Events.sendEvent("AccessTokenUnauthorized", {})

		tryCompare(testCase, "tokenRefreshedCount", 1, 5000)
		compare(requestsOf("RefreshToken").length, 1)
		compare(AuthorizationController.getAccessToken(), "fresh-token")
		compare(testCase.loggedOutCount, 0)
	}

	// The server refuses the subscription with the new token too: the token is not the cause, it is not refreshed in a loop
	function test_repeatedSubscriptionRefusalIsNotRefreshedAgain(){
		answerRefresh(true)
		answerRefresh(true)

		Events.sendEvent("AccessTokenUnauthorized", {})
		tryCompare(testCase, "tokenRefreshedCount", 1, 5000)
		Events.sendEvent("AccessTokenUnauthorized", {})

		wait(300)
		compare(requestsOf("RefreshToken").length, 1)
		compare(testCase.loggedOutCount, 0)
	}

	function test_subscriptionRefusalWithAnEndedSessionSignsOut(){
		answerRefresh(false)

		Events.sendEvent("AccessTokenUnauthorized", {})

		tryCompare(testCase, "loggedOutCount", 1, 5000)
		compare(testCase.tokenRefreshedCount, 0)
	}

	// The caller of a refused request learns its end once: the answer to the request sent again
	function test_refusedRequestEndsOnceWithItsAnswer(){
		GqlTestTransport.queueStatus("DevicesList", 401)
		answerRefresh(true)
		// The first body goes with the refusal
		GqlTestTransport.queueResponse("DevicesList", JSON.stringify({"data": {"DevicesList": {"ok": true}}}))
		GqlTestTransport.queueResponse("DevicesList", JSON.stringify({"data": {"DevicesList": {"ok": true}}}))
		let sender = createTemporaryObject(endingSenderComp, testCase)

		sender.send()

		tryVerify(function(){ return sender.statuses.length > 0 }, 5000)
		wait(300)
		compare(sender.statuses, [1], "Not ended failed by the refusal first")
	}

	function test_refusedRequestEndsOnce_data(){
		return [
			{"tag": "refresh refused", "refusals": 1, "isRefreshAccepted": false},
			{"tag": "refused again with the new token", "refusals": 2, "isRefreshAccepted": true}
		]
	}

	// Not sent again: the caller learns it failed, once
	function test_refusedRequestEndsOnce(data){
		for (let i = 0; i < data.refusals; i++){
			GqlTestTransport.queueStatus("DevicesList", 401)
		}
		answerRefresh(data.isRefreshAccepted)
		let sender = createTemporaryObject(endingSenderComp, testCase)

		sender.send()

		tryVerify(function(){ return sender.statuses.length > 0 }, 5000)
		wait(300)
		compare(sender.statuses, [-1])
	}

	// The refresh refused as Unauthorized, not by its answer: the session ended all the same
	function test_refreshRefusedAsUnauthorizedSignsOut(){
		GqlTestTransport.queueStatus("DevicesList", 401)
		GqlTestTransport.queueStatus("RefreshToken", 401)
		let sender = createTemporaryObject(endingSenderComp, testCase)

		sender.send()

		tryCompare(testCase, "loggedOutCount", 1, 5000)
		tryCompare(sender, "statuses", [-1], 5000)
		compare(requestsOf("RefreshToken").length, 1, "Not refreshed in a loop")
	}

	// Tabs of a browser share the stored session; another tab rotated the refresh token before: its token is used,
	// the rotated own one is never sent (the server closes a session whose rotated token comes again)
	function sessionToken(sessionId, version){
		return "header." + Qt.btoa(JSON.stringify({"userId": "user-id", "sessionId": sessionId, "version": version})) + ".test"
	}

	function useSessionToken(){
		let token = sessionToken("session", 1)
		AuthorizationController.userTokenProvider.accessToken = token
		AuthorizationController.setAccessToken(token)
	}

	function test_refreshTokenRotatedByAnotherTab(){
		useSessionToken()
		AuthorizationController.storage.setValue("accessToken", sessionToken("session", 2))
		AuthorizationController.storage.setValue("refreshToken", "refresh-of-another-tab")
		GqlTestTransport.queueStatus("DevicesList", 401)
		answerRefresh(true, sessionToken("session", 3))
		let sender = createTemporaryObject(devicesSenderComp, testCase)

		sender.send()

		tryVerify(function(){ return requestsOf("DevicesList").length === 2 }, 5000, "Sent again after the refresh")
		compare(requestsOf("RefreshToken").length, 1)
		verify(requestsOf("RefreshToken")[0].body.indexOf("refresh-of-another-tab") >= 0, "With the stored token")
		verify(requestsOf("RefreshToken")[0].body.indexOf("refresh-1") < 0, "Never with the rotated own one")
		verify(!requestsOf("RefreshToken")[0].headers["x-authentication-token"])
		compare(requestsOf("DevicesList")[1].headers["x-authentication-token"], sessionToken("session", 3))
		compare(testCase.loggedOutCount, 0, "Not signed out for a token another tab rotated")
	}

	// Instances of a desktop application remembering one session: another instance rotated the remembered token
	function test_rememberedTokenOfAnotherInstanceIsTaken(){
		useSessionToken()
		AuthorizationController.rememberMe = true
		AuthorizationController.lastUser = "user"
		AuthorizationController.storedRefreshToken = "refresh-1"
		AuthorizationController.storage.setValue("Login_storedRefreshToken", "refresh-of-another-instance")
		GqlTestTransport.queueStatus("DevicesList", 401)
		answerRefresh(true, sessionToken("session", 3))
		let sender = createTemporaryObject(devicesSenderComp, testCase)

		sender.send()

		tryVerify(function(){ return requestsOf("DevicesList").length === 2 }, 5000)
		compare(requestsOf("RefreshToken").length, 1)
		verify(requestsOf("RefreshToken")[0].body.indexOf("refresh-of-another-instance") >= 0, requestsOf("RefreshToken")[0].body)
	}

	// Two tabs refresh at once: the one that lost the rotation takes the token of the other one
	function test_refreshTokenRotatedDuringTheRefresh(){
		useSessionToken()
		AuthorizationController.storage.setValue("accessToken", sessionToken("session", 2))
		testCase.isRotatedElsewhereOnRefreshRequest = true
		GqlTestTransport.queueStatus("DevicesList", 401)
		answerRefresh(false)
		answerRefresh(true, sessionToken("session", 3))
		let sender = createTemporaryObject(devicesSenderComp, testCase)

		sender.send()

		tryVerify(function(){ return requestsOf("DevicesList").length === 2 }, 5000, "Sent again after the second refresh")
		compare(requestsOf("RefreshToken").length, 2)
		verify(requestsOf("RefreshToken")[0].body.indexOf("refresh-1") >= 0, "First with its own token")
		verify(requestsOf("RefreshToken")[1].body.indexOf("refresh-of-another-tab") >= 0, "Then with the one of the other tab")
		compare(testCase.loggedOutCount, 0)
	}

	function test_refusedRefreshTokenOfAnotherTabSignsOut_data(){
		return [
			{"tag": "the stored token is refused", "storedToken": "refresh-of-another-tab", "refreshCount": 1},
			{"tag": "the stored token is the refused one", "storedToken": "refresh-1", "refreshCount": 1}
		]
	}

	function test_refusedRefreshTokenOfAnotherTabSignsOut(data){
		useSessionToken()
		AuthorizationController.storage.setValue("accessToken", sessionToken("session", 2))
		AuthorizationController.storage.setValue("refreshToken", data.storedToken)
		GqlTestTransport.queueStatus("DevicesList", 401)
		answerRefresh(false)
		answerRefresh(false)
		let sender = createTemporaryObject(devicesSenderComp, testCase)

		sender.send()

		tryCompare(testCase, "loggedOutCount", 1, 5000)
		wait(300)
		compare(requestsOf("RefreshToken").length, data.refreshCount, "Each token is tried once")
		compare(requestsOf("DevicesList").length, 1)
	}

	function test_replacedRememberedSessionIsLeftAlone(){
		useSessionToken()
		AuthorizationController.rememberMe = true
		AuthorizationController.lastUser = "user"
		AuthorizationController.storedRefreshToken = "refresh-1"
		answerRefresh(true, sessionToken("session", 2))
		GqlTestTransport.queueDelay("RefreshToken", 250)
		Events.sendEvent("AccessTokenUnauthorized", {})
		tryVerify(function(){ return requestsOf("RefreshToken").length === 1 })

		AuthorizationController.storage.setValue("Login_sessionKey", JSON.stringify(["next", "user-id", "", ""]))
		AuthorizationController.storage.setValue("Login_storedRefreshToken", "next-refresh")
		tryCompare(testCase, "loggedOutCount", 1)
		compare(AuthorizationController.storage.value("Login_storedRefreshToken"), "next-refresh")
		compare(testCase.tokenRefreshedCount, 0)
	}

	function test_switchWithExpiredToken_data(){
		return [{"tag": "selection", "command": "SelectTenant"}, {"tag": "permissions after selection", "command": "GetPermissions"}]
	}

	function test_switchWithExpiredToken(data){
		let tenant = "new-organization"
		let selected = {"ok": true, "userSession": {"userId": "user-id", "tenantId": tenant,
				"accessToken": "selected-access", "refreshToken": "selected-refresh"}}
		let response = JSON.stringify({"data": {"SelectTenant": selected}})
		GqlTestTransport.queueStatus(data.command, 401)
		GqlTestTransport.queueResponse("SelectTenant", response)
		GqlTestTransport.queueResponse("SelectTenant", response)
		answerRefresh(true, "renewed-access", data.command === "GetPermissions" ? tenant : "")
		let permissions = JSON.stringify({"data": {"GetPermissions": {"permissions": "/Current"}}})
		GqlTestTransport.queueResponse("GetPermissions", permissions)
		GqlTestTransport.queueResponse("GetPermissions", permissions)
		AuthorizationController.selectTenant(tenant)
		tryCompare(AuthorizationController, "currentTenantId", tenant)
		tryCompare(AuthorizationController, "__tenantSwitchInProgress", false)
		compare(AuthorizationController.getPermissions(), ["/Current"])
		compare(requestsOf("RefreshToken").length, 1)
		if (data.command === "GetPermissions")
			verify(requestsOf("GetPermissions")[1].body.indexOf("renewed-access") >= 0)
	}

	function test_workFromPreviousOrganizationIsNotRepeated(){
		GqlTestTransport.queueDelay("SelectTenant", 250)
		GqlTestTransport.queueResponse("SelectTenant", JSON.stringify({"data": {"SelectTenant": {"ok": true,
			"userSession": {"userId": "user-id", "tenantId": "next", "accessToken": "selected", "refreshToken": "next-refresh"}}}}))
		GqlTestTransport.queueStatus("DevicesList", 401)
		let sender = createTemporaryObject(endingSenderComp, testCase)
		AuthorizationController.selectTenant("next")
		sender.send()
		tryCompare(AuthorizationController, "currentTenantId", "next")
		tryCompare(sender, "statuses", [-1])
		compare(requestsOf("DevicesList").length, 1)
	}

	// A token holding its lifetime: issued now, expiring in lifetimeSecs
	function lifetimeToken(lifetimeSecs, issuedAgoSecs){
		let now = Math.floor(Date.now() / 1000) - (issuedAgoSecs ? issuedAgoSecs : 0)
		let payload = Qt.btoa(JSON.stringify({"userId": "user-id", "iat": now, "exp": now + lifetimeSecs}))
		return Qt.btoa(JSON.stringify({"alg": "HS256", "typ": "JWT"})) + "." + payload.replace(/=+$/, "").replace(/[+]/g, "-").replace(/[/]/g, "_") + ".signature"
	}

	// Refreshed ahead of its expiry: nothing is refused for an expired token first
	function test_accessTokenIsRefreshedAhead(){
		answerRefresh(true)

		AuthorizationController.userTokenProvider.accessToken = lifetimeToken(2)

		tryVerify(function(){ return requestsOf("RefreshToken").length === 1 }, 5000, "Refreshed before the expiry")
		verify(requestsOf("RefreshToken")[0].body.indexOf("refresh-1") >= 0)
		tryCompare(AuthorizationController.userTokenProvider, "accessToken", "fresh-token", 3000)
		compare(testCase.loggedOutCount, 0)
		compare(testCase.tokenRefreshedCount, 1, "The subscriptions take the new token")
	}

	// A token kept from before, e.g. of a reloaded page, has little of its lifetime left: refreshed soon, not after most of the lifetime
	function test_tokenWithLittleLeftIsRefreshedSoon(){
		answerRefresh(true)

		AuthorizationController.userTokenProvider.accessToken = lifetimeToken(60, 50)

		tryVerify(function(){ return requestsOf("RefreshToken").length === 1 }, 3000, "Refreshed before its expiry in 10 s")
	}

	function test_tokenWithoutLifetimeIsNotRefreshedAhead(){
		AuthorizationController.userTokenProvider.accessToken = "opaque-token"

		wait(1500)
		compare(requestsOf("RefreshToken").length, 0)
	}

	// Signed out before it was due: nothing is refreshed
	function test_signOutCancelsTheRefreshAhead(){
		AuthorizationController.userTokenProvider.accessToken = lifetimeToken(2)
		AuthorizationController.logoutForce()

		wait(2200)
		compare(requestsOf("RefreshToken").length, 0)
	}

	// At the start the remembered session is restored with its refresh token, or the sign-in is shown
	function test_startWithTheRememberedSession_data(){
		return [
			{"tag": "restored", "status": 200, "isAccepted": true},
			{"tag": "refresh token expired", "status": 200, "isAccepted": false},
			{"tag": "refresh token refused as unauthorized", "status": 401, "isAccepted": false},
			{"tag": "server out of reach", "status": 500, "isAccepted": false}
		]
	}

	function test_startWithTheRememberedSession(data){
		AuthorizationController.logoutForce()
		testCase.loggedOutCount = 0
		AuthorizationController.rememberMe = true
		AuthorizationController.lastUser = "user"
		AuthorizationController.storedRefreshToken = "refresh-1"
		GqlTestTransport.queueStatus("RefreshToken", data.status)
		answerRefresh(data.isAccepted)

		ApplicationInfoProvider.superuserExistResult("EXISTS", "")

		if (data.isAccepted){
			tryCompare(testCase, "loggedInCount", 1, 5000)
			compare(AuthorizationController.getAccessToken(), "fresh-token")
			compare(AuthorizationController.storedRefreshToken, "refresh-2", "The rotated token is remembered")
		}
		else{
			// The application shows the sign-in on this result
			tryCompare(testCase, "superuserResults", ["EXISTS"], 5000)
			compare(testCase.loggedInCount, 0)
			// A refused token is forgotten; one the server could not check is kept for the next start
			compare(AuthorizationController.storedRefreshToken, data.status === 500 ? "refresh-1" : "")
		}
		compare(requestsOf("RefreshToken")[0].body.indexOf("refresh-1") >= 0, true)
	}
}
