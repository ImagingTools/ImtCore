import QtQuick 2.12
import QtTest 1.12
import imtauthgui 1.0
import imtqmltest 1.0

TestCase {
	id: testCase

	name: "AuthorizationLogout"

	property int loggedOutCount: 0

	Connections {
		target: AuthorizationController

		function onLoggedOut(){
			testCase.loggedOutCount++
		}
	}

	function init(){
		GqlTestTransport.reset()
		testCase.loggedOutCount = 0
	}

	function cleanup(){
		AuthorizationController.logoutForce()
		GqlTestTransport.reset()
	}

	function signIn(token){
		AuthorizationController.userTokenProvider.login = "user"
		AuthorizationController.userTokenProvider.userId = "user-id"
		AuthorizationController.userTokenProvider.accessToken = token
		AuthorizationController.setAccessToken(token)
	}

	function requestCount(commandId){
		return GqlTestTransport.requestedCommands().filter(function(command){ return command === commandId }).length
	}

	// The session ends at the server: its refresh token and its subscriptions stop working there
	function test_logoutClosesTheSessionAtTheServer(){
		signIn("access-token-of-the-session")
		GqlTestTransport.queueResponse("Logout", JSON.stringify({"data": {"Logout": {"ok": true}}}))

		AuthorizationController.logout()

		tryCompare(GqlTestTransport, "requestCount", 1, 3000)
		compare(requestCount("Logout"), 1)
		compare(GqlTestTransport.lastRequestHeaders["x-authentication-token"], "access-token-of-the-session", "The token of the session goes with the request")
		let body = GqlTestTransport.lastRequestBody
		verify(body.indexOf("mutation") >= 0, body)
		verify(body.indexOf("accessToken") >= 0 && body.indexOf("access-token-of-the-session") >= 0, "The required input names the token: " + body)
	}

	function test_logoutDoesNotWaitForTheServer(){
		signIn("access-token-of-the-session")
		GqlTestTransport.responseStatus = 500

		AuthorizationController.logout()

		compare(AuthorizationController.userTokenProvider.accessToken, "", "Signed out at once")
		compare(testCase.loggedOutCount, 1)

		tryCompare(GqlTestTransport, "requestCount", 1, 3000)
		wait(300)
		compare(testCase.loggedOutCount, 1, "The answer does not sign out twice")
	}

	function test_answerDoesNotSignOutAgain(){
		signIn("access-token-of-the-session")
		GqlTestTransport.queueResponse("Logout", JSON.stringify({"data": {"Logout": {"ok": true}}}))

		AuthorizationController.logout()

		tryCompare(GqlTestTransport, "requestCount", 1, 3000)
		wait(300)
		compare(testCase.loggedOutCount, 1)
	}

	function requestsOf(commandId){
		return GqlTestTransport.requests().filter(function(request){ return request.commandId === commandId })
	}

	function answerRefresh(isAccepted){
		let session = isAccepted ? {"userId": "user-id", "tenantId": "", "accessToken": "fresh-token-of-the-session", "refreshToken": "refresh-2"} : null
		GqlTestTransport.queueResponse("RefreshToken", JSON.stringify({"data": {"RefreshToken": {"ok": isAccepted, "userSession": session}}}))
	}

	// The token has expired: the session is refreshed with its refresh token and closed with the new token
	function test_logoutWithAnExpiredTokenStillClosesTheSession(){
		signIn("expired-token")
		AuthorizationController.userTokenProvider.refreshToken = "refresh-1"
		GqlTestTransport.queueStatus("Logout", 401)
		// The first body goes with the refusal
		GqlTestTransport.queueResponse("Logout", JSON.stringify({"data": {"Logout": {"ok": true}}}))
		answerRefresh(true)
		GqlTestTransport.queueResponse("Logout", JSON.stringify({"data": {"Logout": {"ok": true}}}))

		AuthorizationController.logout()

		tryVerify(function(){ return requestsOf("Logout").length === 2 }, 3000, "Closed with the new token")
		compare(requestsOf("Logout")[0].headers["x-authentication-token"], "expired-token")
		let refresh = requestsOf("RefreshToken")
		compare(refresh.length, 1)
		verify(refresh[0].body.indexOf("refresh-1") >= 0, refresh[0].body)
		verify(!refresh[0].headers["x-authentication-token"], "The expired token is not sent to refresh it")
		compare(requestsOf("Logout")[1].headers["x-authentication-token"], "fresh-token-of-the-session")

		wait(300)
		compare(GqlTestTransport.requestCount, 3, "Nothing more")
		compare(testCase.loggedOutCount, 1)
		compare(AuthorizationController.userTokenProvider.accessToken, "", "The refresh does not sign in again")
		compare(AuthorizationController.getAccessToken(), "")
	}

	function test_logoutWithARefusedRefreshTokenEnds(){
		signIn("expired-token")
		AuthorizationController.userTokenProvider.refreshToken = "refresh-1"
		GqlTestTransport.queueStatus("Logout", 401)
		// The first body goes with the refusal
		GqlTestTransport.queueResponse("Logout", JSON.stringify({"data": {"Logout": {"ok": true}}}))
		answerRefresh(false)

		AuthorizationController.logout()

		tryVerify(function(){ return requestsOf("RefreshToken").length === 1 }, 3000)
		wait(300)
		compare(requestsOf("Logout").length, 1, "The session has already ended")
		compare(testCase.loggedOutCount, 1)
	}

	// Another user signs in before the server refuses the logout: the logout closes only its own session
	function test_lateLogoutRefusalDoesNotCloseTheNextSession(){
		signIn("expired-token")
		AuthorizationController.userTokenProvider.refreshToken = "refresh-1"
		GqlTestTransport.queueStatus("Logout", 401)
		// The first body goes with the refusal
		GqlTestTransport.queueResponse("Logout", JSON.stringify({"data": {"Logout": {"ok": true}}}))
		GqlTestTransport.queueDelay("Logout", 300)
		answerRefresh(true)
		GqlTestTransport.queueResponse("Logout", JSON.stringify({"data": {"Logout": {"ok": true}}}))

		AuthorizationController.logout()
		signIn("token-of-the-next-session")
		AuthorizationController.userTokenProvider.refreshToken = "refresh-of-the-next-session"

		tryVerify(function(){ return requestsOf("Logout").length === 2 }, 3000)
		compare(requestsOf("Logout")[1].headers["x-authentication-token"], "fresh-token-of-the-session", "The closed session, not the next one")
		verify(requestsOf("RefreshToken")[0].body.indexOf("refresh-1") >= 0, "Refreshed with the token of the closed session")
		compare(AuthorizationController.userTokenProvider.accessToken, "token-of-the-next-session", "The next session goes on")
		compare(AuthorizationController.userTokenProvider.refreshToken, "refresh-of-the-next-session")
		compare(testCase.loggedOutCount, 1)
	}

	function test_noRequestWithoutASession(){
		AuthorizationController.logout()

		wait(300)
		compare(GqlTestTransport.requestCount, 0)
		compare(testCase.loggedOutCount, 1)
	}
}
