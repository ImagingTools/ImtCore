pragma Singleton

import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtguigql 1.0
import imtauthgui 1.0
import imtauthUsersSdl 1.0
import imtauthAuthorizationSdl 1.0
import imtauthSessionsSdl 1.0
import imtauthTenantsSdl 1.0
import imtauthTenantMembershipsSdl 1.0
import Qt.labs.settings 1.0

QtObject {
	id: root;
	
	property string permissionPath: ""
	property string productId: "";
	// The product whose data a console administers now; requests go in its context while it is set.
	property string administeredProductId: "";
	
	signal userModeChanged(string userMode);
	signal superuserExistResult(string status, string error);
	signal loginFailed(string message);
	// The credentials are right, but the password has expired and must be changed before the login
	signal passwordChangeRequired();
	
	signal loggedIn();
	signal loggedOut();
	signal tenantSelected(string tenantId);
	signal tenantSelectionFailed(string error);
	signal changePasswordSuccessfully();
	signal changePasswordFailed(string message, var violatedRules);
	signal registerSuccessfully();
	signal registerFailed(string message, var violatedRules);

	// --- Sign-in with external accounts (Google, Apple, Facebook) ---
	signal externalLoginProvidersLoaded();
	signal externalAccountLinked(string providerId);
	signal externalLoginFailed(string message);

	// --- Tenant membership lifecycle events ---
	// Emitted by membership-aware views (e.g. TenantCollectionView) so other
	// components anywhere in the app (e.g. an open TenantEditor) can react and
	// reload without being directly coupled to the originating view.
	signal tenantInvitationReceived(string tenantId, string tenantName, string role);
	signal tenantInvitationAccepted(string tenantId, string membershipId);
	signal tenantInvitationRejected(string tenantId, string membershipId);
	signal tenantOwnershipTransferred(string tenantId);
	signal tenantMembershipRoleChanged(string tenantId, string userId, string role);
	signal tenantMembershipRemoved(string tenantId, string userId);
	signal tenantInvitationRevoked(string tenantId, string invitationId);
	// The members or invitations of the organization of the session changed, whoever changed them
	signal tenantMembersChanged(string tenantId);
	// Membership notifications were lost while the connection was down; membership views re-read their data.
	signal membershipNotificationsResumed();
	// The organizations the user may switch to changed: what shows them reads them again
	signal organizationsChanged();
	// The permissions of the session changed (granted or taken) and are read again: what shows them is rebuilt
	signal accessChanged();

	// Properties to store remember me state and credentials
	property bool rememberMe: false
	property string lastUser: ""
	property string storedRefreshToken: ""
	property bool __loginSettingsLoading: false
	property string currentTenantId: ""
	property string currentTenantName: ""
	property var __permissionsRefreshCallbacks: []
	property bool __tenantRemovalSwitchInProgress: false
	property bool __tenantSwitchInProgress: false
	property string __pendingTenantId: ""
	property bool __hasPendingTenantSwitch: false
	// An inactive organization cannot be selected; it is activated outside of any organization first.
	property string __activationTenantId: ""

	property bool __refreshInProgress: false
	property int __sessionGeneration: 0
	property bool __switchAwaitingPermissions: false
	property bool __permissionsInFlight: false
	property bool __permissionsRetried: false
	property bool __browserSessionStored: false
	property var __pendingRetryQueue: []
	// Part of the lifetime of an access token after which it is refreshed ahead
	property real refreshAheadRatio: 0.8
	// A refresh the server could not answer is asked again after this pause (ms); the session is kept meanwhile
	property int refreshRetryInterval: 5000
	// A request refused again this soon (ms) after it was sent again with a new token is not refreshed and sent again once more
	property int repeatedRefusalWindow: 30000
	property var __recentRetries: []
	// What the server answered about the superuser before a remembered session was restored
	property string __restoreStatus: ""
	property string __restoreError: ""
	// The token of the session a logout closes at the server
	property string __logoutToken: ""
	property bool __authFailedDuringSwitch: false
	// When a refusal of a subscription asked for a refresh last
	property real __subscriptionRefreshAt: 0

	// Sign-in with external accounts: set per application in ACC (ExternalLoginEnabled), delivered with GetApplicationInfo.
	readonly property bool externalLoginEnabled: ApplicationInfoProvider.externalLoginEnabled
	// Providers offered by the server: [{ "id", "name", "type" }]
	property var externalLoginProviders: []
	property bool externalLoginInProgress: false
	property string externalLoginProviderId: ""
	property string __externalLoginId: ""
	property bool __externalLoginIsLink: false
	property real __externalLoginDeadline: 0
	// Prepared logins: [{ "providerId", "isLink", "loginId", "authorizationUrl", "preparedAt" }]
	property var __preparedLogins: []
	property var __prepareQueue: []
	property var __preparingProvider: null
	readonly property string authenticationTokenHeaderId: "x-authentication-token"

	// Set per application in ACC and delivered with GetApplicationInfo; without tenants
	// no tenant or organization request is sent at all.
	readonly property bool tenantsEnabled: ApplicationInfoProvider.tenantsEnabled

	// --- Pending invitations tracking ---
	property int pendingInvitationsCount: 0
	property var pendingInvitations: []

	function refreshPendingInvitations() {
		if (!root.tenantsEnabled)
			return
		if (!root.userTokenProvider.accessToken || root.userTokenProvider.accessToken === "")
			return
		root.__sendBound(root.__pendingInvitationsRequest, root.__pendingInvitationsInput)
	}

	function __startTenantTracking() {
		if (!root.tenantsEnabled)
			return
		root.refreshPendingInvitations()
		__membershipSubscription.registerSubscription()
		__organizationMembersSubscription.registerSubscription()
		tenantCollectionListener.registerSubscription()
	}

	function __stopTenantTracking() {
		__membershipSubscription.unRegisterSubscription()
		__organizationMembersSubscription.unRegisterSubscription()
		tenantCollectionListener.unRegisterSubscription()
	}

	function __updatePendingInvitations(invitationsList) {
		var list = []
		if (invitationsList) {
			for (var i = 0; i < invitationsList.count; i++) {
				var inv = invitationsList.get(i).item
				if (inv) {
					list.push({
						"id": inv.m_id || "",
						"tenantId": inv.m_tenantId || "",
						"tenantName": inv.m_tenantName || "",
						"role": inv.m_role || "",
						"invitedByUserId": inv.m_invitedByUserId || "",
						"createdAt": inv.m_createdAt || ""
					})
				}
			}
		}
		root.pendingInvitations = list
		root.pendingInvitationsCount = list.length
	}

	property GetMyTenantInvitationsInput __pendingInvitationsInput: GetMyTenantInvitationsInput {
		m_statuses: ["Pending"]
	}

	property GqlSdlRequestSender __pendingInvitationsRequest: GqlSdlRequestSender {
		permissionPath: root.permissionPath
		gqlCommandId: ImtauthTenantMembershipsSdlCommandIds.s_getMyTenantInvitations

		sdlObjectComp: Component {
			GetMyTenantInvitationsPayload {
				onFinished: {
					root.__updatePendingInvitations(m_invitations)
				}
			}
		}

		function onError(message, type) {
			// Silently ignore errors for invitation polling
		}
	}

	property Settings storage: Settings {
		category: "AuthorizationController"
	}
	
	// Load settings from LocalStorage on component creation
	Component.onCompleted: {
		loadLoginSettings();
		Events.subscribeEvent("AccessTokenUnauthorized", root.onAccessTokenUnauthorizedEvent);
		Events.subscribeEvent("AccessTokenForbidden", root.onAccessTokenForbiddenEvent);
	}

	Component.onDestruction: {
		Events.unSubscribeEvent("AccessTokenUnauthorized", root.onAccessTokenUnauthorizedEvent);
		Events.unSubscribeEvent("AccessTokenForbidden", root.onAccessTokenForbiddenEvent);
	}
	
	// Watch for changes and save to LocalStorage
	onRememberMeChanged: {
		if (!root.__loginSettingsLoading)
			storage.setValue("Login_rememberMe", root.rememberMe ? "true" : "false")
	}
	onLastUserChanged: saveLoginSettings()
	onStoredRefreshTokenChanged: saveLoginSettings()

	// Refresh pending invitations on relevant events (including when sender revokes)
	onExternalLoginEnabledChanged: {
		root.loadExternalLoginProviders()
	}

	onLoggedIn: {
		root.__clearPreparedLogins()
		root.__startTenantTracking()
		root.__accessSubscription.registerSubscription()
	}
	onLoggedOut: {
		root.__clearPreparedLogins()
		root.__stopTenantTracking()
		root.__accessSubscription.unRegisterSubscription()
		__subscriptionRefreshAt = 0
	}

	property Timer __refreshAheadTimer: Timer {
		repeat: false
		onTriggered: {
			root.__refreshAhead()
		}
	}

	property Connections __accessTokenConnections: Connections {
		target: root.userTokenProvider

		function onAccessTokenChanged() {
			root.__scheduleRefreshAhead(root.userTokenProvider.accessToken)
		}
	}

	property Timer __refreshRetryTimer: Timer {
		interval: root.refreshRetryInterval
		onTriggered: {
			if (!root.__refreshInProgress && root.userTokenProvider.refreshToken)
				root.__startRefresh()
		}
	}
	// The application info can arrive after a restored session has already logged in.
	onTenantsEnabledChanged: {
		if (!root.userTokenProvider.accessToken || root.userTokenProvider.accessToken === "")
			return
		if (root.tenantsEnabled)
			root.__startTenantTracking()
		else
			root.__stopTenantTracking()
	}
	onTenantInvitationReceived: refreshPendingInvitations()
	onTenantInvitationAccepted: refreshPendingInvitations()
	onTenantInvitationRejected: refreshPendingInvitations()
	onTenantInvitationRevoked: refreshPendingInvitations()

	// --- Subscription client for membership notifications ---
	property SubscriptionClient __membershipSubscription: SubscriptionClient {
		gqlCommandId: "OnMembershipNotification"
		autoSubscribe: false

		function getHeaders() { return {} }

		onMessageReceived: {
			if (!data) return
			var notificationType = data.containsKey("notificationType") ? data.getData("notificationType") : ""
			var membershipId = data.containsKey("membershipId") ? data.getData("membershipId") : ""
			var userId = data.containsKey("userId") ? data.getData("userId") : ""
			var tenantId = data.containsKey("tenantId") ? data.getData("tenantId") : ""
			var tenantName = data.containsKey("tenantName") ? data.getData("tenantName") : ""
			var role = data.containsKey("role") ? data.getData("role") : ""
			// The notification center keeps the message for the user; here only the state of the views follows
			if (notificationType === "InvitationReceived" || notificationType === 0) {
				root.tenantInvitationReceived(tenantId, tenantName, role)
			} else if (notificationType === "InvitationAccepted" || notificationType === 1) {
				root.tenantInvitationAccepted(tenantId, membershipId)
			} else if (notificationType === "InvitationRejected" || notificationType === 2) {
				root.tenantInvitationRejected(tenantId, membershipId)
			} else if (notificationType === "OwnershipTransferred" || notificationType === 3) {
				root.tenantOwnershipTransferred(tenantId)
			} else if (notificationType === "MembershipRoleChanged" || notificationType === 4) {
				root.tenantMembershipRoleChanged(tenantId, userId, role)
			} else if (notificationType === "MembershipRemoved" || notificationType === 5) {
				root.tenantMembershipRemoved(tenantId, userId)
			} else if (notificationType === "InvitationRevoked" || notificationType === 6) {
				root.tenantInvitationRevoked(tenantId, membershipId)
			}
		}

		onRefreshRequired: {
			root.refreshPendingInvitations()
			root.membershipNotificationsResumed()
		}
	}

	// The server tells a change of the access of the session: the permissions apply at once, not with the next refresh
	property SubscriptionClient __accessSubscription: SubscriptionClient {
		gqlCommandId: "OnAccessChanged"
		autoSubscribe: false

		function getHeaders() { return {} }

		onMessageReceived: {
			root.__onAccessChanged(true)
		}

		// Changes may have been missed while the connection was down: told only when the permissions differ
		onRefreshRequired: {
			root.__onAccessChanged(false)
		}
	}

	function __onAccessChanged(isChangeTold) {
		if (!root.isLoggedIn())
			return

		let permissionsBefore = root.normalizePermissions(root.userTokenProvider.permissions).join(";")
		root.refreshPermissions(function(){
			let permissionsAfter = root.normalizePermissions(root.userTokenProvider.permissions).join(";")
			if (isChangeTold || permissionsAfter !== permissionsBefore) {
				root.accessChanged()
			}
		})
	}

	// The members of the organization of the session; the server sends it to everybody working inside it
	property SubscriptionClient __organizationMembersSubscription: SubscriptionClient {
		gqlCommandId: "OnOrganizationMembersChanged"
		autoSubscribe: false

		function getHeaders() { return {} }

		onMessageReceived: {
			if (data && data.containsKey("tenantId"))
				root.tenantMembersChanged(data.getData("tenantId"))
		}

		onRefreshRequired: {
			root.tenantMembersChanged(root.currentTenantId)
		}
	}

	onProductIdChanged: {
		if (Qt.platform.os !== "web" && productId !== ""){
			userTokenProvider.authorizationGqlModel.SetProductId(productId)
		}
	}
	
	function loadLoginSettings() {
		// The change handlers save the login settings: while they are read, nothing is saved, otherwise the values that are not read yet are
		// written over the stored ones
		root.__loginSettingsLoading = true;
		let rememberMeStr = storage.value("Login_rememberMe", "");
		root.rememberMe = (rememberMeStr === "true");
		root.lastUser = storage.value("Login_lastUser", "");
		root.storedRefreshToken = storage.value("Login_storedRefreshToken", "");
		root.__loginSettingsLoading = false;
	}

	function normalizePermissions(rawPermissions) {
		if (rawPermissions === undefined || rawPermissions === null)
			return [];

		if (Array.isArray(rawPermissions))
			return rawPermissions;

		if (typeof rawPermissions === "string") {
			let value = rawPermissions.trim();
			if (value === "")
				return [];

			if (value.indexOf(";") >= 0)
				return value.split(";").filter(function(item){ return item !== ""; });

			if (value.indexOf(",") >= 0)
				return value.split(",").filter(function(item){ return item !== ""; });

			return [value];
		}

		return [];
	}

	function refreshPermissions(callback) {
		if (!root.userTokenProvider.accessToken || root.userTokenProvider.accessToken === "") {
			root.userTokenProvider.permissions = [];
			if (callback)
				callback(true);
			return;
		}

		if (callback){
			let callbacks = root.__permissionsRefreshCallbacks.slice();
			callbacks.push(callback);
			root.__permissionsRefreshCallbacks = callbacks;
		}

		if (!root.__permissionsInFlight) {
			root.__permissionsInFlight = true
			getPermissionsInput.m_accessToken = root.userTokenProvider.accessToken
			root.__sendBound(getPermissionsGqlSender, getPermissionsInput)
		}
	}

	function __completePermissionsRefresh(){
		root.__permissionsInFlight = false
		root.__permissionsRetried = false
		let generation = root.__sessionGeneration
		let callbacks = root.__permissionsRefreshCallbacks;
		root.__permissionsRefreshCallbacks = [];

		for (let i = 0; i < callbacks.length && generation === root.__sessionGeneration; i++){
			callbacks[i](true);
		}
	}
	
	function saveLoginSettings() {
		if (root.__loginSettingsLoading){
			return;
		}

		storage.setValue("Login_rememberMe", root.rememberMe ? "true" : "false");
		storage.setValue("Login_lastUser", root.lastUser);
		storage.setValue("Login_storedRefreshToken", root.storedRefreshToken);
		let ownToken = root.storedRefreshToken && root.storedRefreshToken === root.userTokenProvider.refreshToken &&
				root.lastUser === root.userTokenProvider.login
		storage.setValue("Login_sessionKey", ownToken ? root.__sessionKey(root.getAccessToken()) : "");
	}
	
	property XmlHttpRequestProxy requestProxy: XmlHttpRequestProxy {
		onForbidden: {
			if (credentialsChanged && !(gqlRequestRef && gqlRequestRef.sessionOwner))
				return
			if (gqlRequestRef && gqlRequestRef.sessionOwner &&
					(gqlRequestRef.detachedLogout || !gqlRequestRef.isCurrent())) {
				return
			}
			// Access denied by permissions/role — session itself is not stale, so a
			// token refresh cannot fix it. Log out immediately, no retry.
			console.warn("Auth: HTTP 403 Forbidden — logout")
			root.logoutForce(qsTr("Access denied. Please sign in again."))
		}

		onUnauthorized: {
			if (credentialsChanged && !(gqlRequestRef && gqlRequestRef.sessionOwner)) {
				root.__dropRequest(gqlRequestRef)
				return
			}
			// Access token expired/rejected by server — reactive recovery via refresh token.
			console.warn("Auth: HTTP 401 Unauthorized")
			root.handleAuthFailure(gqlData, gqlRequestRef, gqlHeaders)
		}

		onInternalError: {
			console.warn("Auth: HTTP 500 Internal Error on GraphQL request")
		}
	}

	// WebSocket subscription path reports the same Unauthorized/Forbidden prefixes.
	function onAccessTokenUnauthorizedEvent(parameters) {
		// Refused again soon after the refresh it asked for: the token is not the cause, no refresh in a loop
		if (Date.now() - root.__subscriptionRefreshAt < root.repeatedRefusalWindow) {
			console.warn("Auth: WebSocket auth failure (401) again after a refresh — not refreshed again")
			return
		}

		console.warn("Auth: WebSocket auth failure (401) — attempting refresh")
		root.__subscriptionRefreshAt = Date.now()
		root.handleAuthFailure("", null)
	}

	function onAccessTokenForbiddenEvent(parameters) {
		console.warn("Auth: WebSocket auth failure (403) — logout")
		root.logoutForce(qsTr("Access denied. Please sign in again."))
	}

	function isLoggedIn() {
		return root.userTokenProvider.login !== ""
				|| root.userTokenProvider.userId !== ""
				|| root.userTokenProvider.accessToken !== ""
				|| root.userTokenProvider.refreshToken !== ""
	}

	// Headers for RefreshToken: clear auth token header so expired JWT is not sent.
	function __emptyAuthTokenHeaders() {
		var headers = ({})
		headers[root.authenticationTokenHeaderId] = ""
		return headers
	}

	function __sendWithoutAccessToken(sender, input) {
		let generation = root.__sessionGeneration
		let accessToken = root.getAccessToken()
		let model = root.userTokenProvider.authorizationGqlModel
		let savedToken = model.GetGlobalAccessToken()
		model.SetGlobalAccessToken("")
		// JQML v3 has neither throw nor try without catch
		try {
			sender.send(input)
		} catch (error) {
			console.warn("Auth: the request could not be sent:", error)
		}
		if (generation === root.__sessionGeneration && accessToken === root.getAccessToken())
			model.SetGlobalAccessToken(savedToken)
	}

	// A request sent again with a fresh token and refused again: the token is not the cause, it fails
	function __requestId(headers) {
		let names = Object.keys(headers || {})
		for (let i = 0; i < names.length; i++) {
			if (names[i].toLowerCase() === "x-request-id")
				return headers[names[i]]
		}
		return ""
	}

	function __isRepeatedRefusal(gqlData, gqlRequestRef, requestId) {
		let now = Date.now()
		let isRepeated = false
		let retries = []
		for (let i = 0; i < root.__recentRetries.length; i++) {
			let retry = root.__recentRetries[i]
			if (now - retry.time > root.repeatedRefusalWindow) {
				continue
			}
			if (!isRepeated && retry.gqlRequestRef === gqlRequestRef &&
					(requestId || retry.requestId ? requestId === retry.requestId : retry.gqlData === gqlData)) {
				isRepeated = true
				continue
			}
			retries.push(retry)
		}
		root.__recentRetries = retries

		return isRepeated
	}

	// The refused requests that are not sent again end failed
	function __dropPendingRequests() {
		var queue = root.__pendingRetryQueue
		root.__pendingRetryQueue = []
		for (var i = 0; i < queue.length; i++) {
			root.__dropRequest(queue[i].gqlRequestRef)
		}
	}

	function __dropRequest(gqlRequestRef) {
		try {
			if (gqlRequestRef && gqlRequestRef.onAuthorizationLost) {
				gqlRequestRef.onAuthorizationLost()
			}
		} catch (e) {
			console.warn("Auth: a dropped request could not be ended:", e)
		}
	}

	// Each transport owner lives until its own reply; the SDL decoder is shared,
	// but a late reply never dispatches into a new operation on that decoder.
	property Component __boundRequest: Component {
		GqlSdlRequestSender {
			id: request
			property var sessionOwner
			property int generation
			property string userId
			property string tenantId
			property string accessToken
			property string closingRefreshToken
			readonly property bool detachedLogout: request.sessionOwner === root.logoutGqlSender || request.sessionOwner === root.logoutRefreshGqlSender
			gqlCommandId: request.sessionOwner.gqlCommandId
			requestType: request.sessionOwner.requestType
			permissionPath: request.sessionOwner.permissionPath
			inputObjectComp: request.sessionOwner.inputObjectComp

			function isCurrent() {
				return request.detachedLogout || (request.generation === root.__sessionGeneration &&
						request.userId === root.userTokenProvider.userId && request.tenantId === root.currentTenantId &&
						request.accessToken === root.getAccessToken())
			}
			function canDispatch() {
				if (!request.isCurrent())
					return false
				if (request.sessionOwner === root.refreshTokenForLoginGqlSender)
					return root.__checkRestoreSettings()
				return request.detachedLogout || root.__checkSharedSession()
			}
			function getHeaders() { return request.sessionOwner.getHeaders() }
			function onResult(data) {
				if (request.canDispatch()) {
					if (request.sessionOwner === root.logoutRefreshGqlSender) {
						if (data && data.ok && data.userSession && data.userSession.accessToken)
							root.__sendLogout(data.userSession.accessToken, "")
					}
					else if (!request.detachedLogout)
						request.sessionOwner.onResult(data)
				}
				request.destroy()
			}
			function onError(message, type) {
				if (!request.detachedLogout && request.canDispatch())
					request.sessionOwner.onError(message, type, request.state)
				request.destroy()
			}
			function onAuthorizationLost() {
				if (request.canDispatch()) {
					if (request.sessionOwner === root.logoutGqlSender && request.closingRefreshToken !== "") {
						root.logoutRefreshGqlSender.refreshToken = request.closingRefreshToken
						root.__sendBound(root.logoutRefreshGqlSender)
					}
					else if (!request.detachedLogout)
						request.sessionOwner.onAuthorizationLost()
				}
				request.destroy()
			}
			onFinished: {
				if (request.state === "Forbidden")
					request.destroy()
			}
		}
	}

	function __sendBound(sender, input, closingRefreshToken) {
		let request = root.__boundRequest.createObject(root, {
			"sessionOwner": sender, "generation": root.__sessionGeneration, "userId": root.userTokenProvider.userId,
			"tenantId": root.currentTenantId, "accessToken": root.getAccessToken(),
			"closingRefreshToken": closingRefreshToken || ""
		})
		if (request.detachedLogout || sender === root.refreshTokenGqlSender || sender === root.refreshTokenForLoginGqlSender)
			root.__sendWithoutAccessToken(request, input)
		else
			request.send(input)
	}

	function handleAuthFailure(gqlData, gqlRequestRef, gqlHeaders) {
		// The requests of the session itself are not refreshed and sent again: each handles its refusal
		let owner = gqlRequestRef ? gqlRequestRef.sessionOwner : null
		if (owner && (gqlRequestRef.detachedLogout || !gqlRequestRef.isCurrent() ||
				owner === root.refreshTokenGqlSender || owner === root.refreshTokenForLoginGqlSender)) {
			root.__dropRequest(gqlRequestRef)
			return
		}

		let requestId = root.__requestId(gqlHeaders)
		if (gqlRequestRef && root.__isRepeatedRefusal(gqlData, gqlRequestRef, requestId)) {
			console.warn("Auth: refused again with the refreshed token — not sent again")
			root.__dropRequest(gqlRequestRef)
			return
		}

		if (owner === root.getPermissionsGqlSender) {
			// Its input also names the access token. Rebuild it after refresh, not from the old body.
			root.__permissionsInFlight = false
			gqlRequestRef.destroy()
			if (root.__permissionsRetried) {
				root.getPermissionsGqlSender.onError("", "")
				return
			}
			root.__permissionsRetried = true
		}
		else if (gqlRequestRef) {
			var queue = root.__pendingRetryQueue.slice()
			queue.push({
				"gqlData": gqlData,
				"gqlRequestRef": gqlRequestRef,
				"generation": root.__sessionGeneration,
				"requestId": requestId,
				"gqlHeaders": gqlHeaders ? gqlHeaders : {}
			})
			root.__pendingRetryQueue = queue
		}

		if (root.__refreshInProgress)
			return

		// Wait for selection. Success drops old work; failure leaves the old tenant current.
		if (root.__tenantSwitchInProgress && !root.__switchAwaitingPermissions && owner !== root.selectTenantGqlSender) {
			root.__authFailedDuringSwitch = true
			return
		}

		if (!root.userTokenProvider.refreshToken || root.userTokenProvider.refreshToken === "") {
			if (!root.isLoggedIn()) {
				root.__dropPendingRequests()
				return
			}

			console.warn("Auth: no refresh token — logout")
			root.logoutForce(qsTr("Your session is no longer valid. Please sign in again."))
			return
		}

		root.__startRefresh()
	}

	function __sessionKey(accessToken) {
		let claims = root.__readTokenClaims(accessToken)
		if (!claims || !claims.sessionId || !claims.userId)
			return ""
		return JSON.stringify([claims.sessionId, claims.userId, claims.tenantId || "", claims.productId || ""])
	}

	// Native Settings needs an explicit sync at credential boundaries to read
	// another application's writes. Ordinary replies use the current view.
	function __syncLoginSettings() {
		if (Qt.platform.os !== "web" && root.storage.sync)
			root.storage.sync()
	}

	// null denotes a different or unidentifiable writer. A login name alone
	// cannot distinguish two sessions of the same account.
	function __sharedRefreshToken() {
		let local = root.userTokenProvider.refreshToken
		let localAccess = root.getAccessToken()
		let browserToken = String(root.storage.value("refreshToken", ""))
		let browserAccess = String(root.storage.value("accessToken", ""))
		if (root.__browserSessionStored && !browserAccess)
			return null
		if ((browserToken || browserAccess) &&
				(root.storage.value("userId", root.getUserId()) !== root.getUserId() ||
				root.storage.value("tenantId", root.currentTenantId) !== root.currentTenantId))
			return null

		let browserChanged = (browserAccess && browserAccess !== localAccess) || (browserToken && browserToken !== local)
		let key = browserChanged ? root.__sessionKey(localAccess) : ""
		if (browserChanged && (!key || key !== root.__sessionKey(browserAccess)))
			return null

		if (!root.rememberMe)
			return browserToken || local

		let remembered = String(root.storage.value("Login_storedRefreshToken", ""))
		if (!remembered)
			return root.storedRefreshToken ? null : browserToken || local

		let login = String(root.storage.value("Login_lastUser", ""))
		if (login && login !== root.userTokenProvider.login)
			return null
		if (remembered !== local) {
			key = key || root.__sessionKey(localAccess)
			if (!key || key !== root.storage.value("Login_sessionKey", ""))
				return null
		}
		return browserToken || remembered
	}

	function __checkSharedSession() {
		if (!root.isLoggedIn() || root.__sharedRefreshToken() !== null)
			return true
		root.__endLocalSession(qsTr("The signed-in session has changed. Please sign in again."), false)
		return false
	}

	function __readRefreshToken() {
		root.__syncLoginSettings()
		let token = root.__sharedRefreshToken()
		if (token === null)
			root.__endLocalSession(qsTr("The signed-in session has changed. Please sign in again."), false)
		return token
	}

	function __adoptStoredRefreshToken(token) {
		if (!token || token === root.userTokenProvider.refreshToken)
			return false
		root.userTokenProvider.refreshToken = token
		root.setRefreshToken(token)
		return true
	}

	function __startRefresh() {
		let token = root.__readRefreshToken()
		if (token === null)
			return
		root.__refreshInProgress = true
		root.__adoptStoredRefreshToken(token)
		root.__sendBound(root.refreshTokenGqlSender)
	}

	// The access token is refreshed before it expires: requests and subscriptions are not refused for it first
	function __refreshAhead() {
		if (root.__refreshInProgress || root.__tenantSwitchInProgress || !root.userTokenProvider.refreshToken || root.userTokenProvider.refreshToken === "") {
			return
		}

		root.__startRefresh()
	}

	// The lifetime (s) of an access token as its issuer wrote it, 0 when it does not tell it
	// The claims of an access token, null when it is no JWT
	function __readTokenClaims(token) {
		let parts = String(token).split(".")
		if (parts.length < 2) {
			return null
		}

		let payload = parts[1].replace(/-/g, "+").replace(/_/g, "/")
		while (payload.length % 4 !== 0) {
			payload += "="
		}

		try {
			return JSON.parse(Qt.atob(payload))
		}
		catch (e) {
			return null
		}
	}

	// Due when the part of its lifetime is used. A token kept from before (a reloaded page) may have little left:
	// then soon. Never later than the part of the lifetime from now, whatever the clock of the client says
	function __scheduleRefreshAhead(token) {
		let claims = root.__readTokenClaims(token)
		let lifetime = (claims && claims.exp && claims.iat) ? claims.exp - claims.iat : 0
		if (lifetime <= 0) {
			root.__refreshAheadTimer.stop()
			return
		}

		let dueMs = (claims.exp - lifetime * (1 - root.refreshAheadRatio)) * 1000 - Date.now()
		root.__refreshAheadTimer.interval = Math.max(1000, Math.min(lifetime * 1000 * root.refreshAheadRatio, dueMs))
		root.__refreshAheadTimer.restart()
	}

	function completeTokenRefresh(ok) {
		if (!root.__refreshInProgress)
			return

		if (!ok) {
			let token = root.__readRefreshToken()
			if (token === null)
				return
			// Tabs of a browser share the stored session: another tab rotated the refresh token, the refresh is asked again with its one
			if (root.__adoptStoredRefreshToken(token)) {
				console.warn("Auth: refresh token rotated elsewhere meanwhile — refreshed again with the stored one")
				root.__sendBound(root.refreshTokenGqlSender)
				return
			}

			// Session/refresh rejected by server → logout.
			console.warn("Auth: refresh failed — logout")
			root.__endLocalSession(qsTr("Your session has expired. Please sign in again."), true)
			return
		}

		root.__refreshInProgress = false
		console.log("Auth: refresh succeeded")
		// Re-register WS subscriptions that failed with the old access token.
		Events.sendEvent("AccessTokenRefreshed", {})

		root.__retryPendingRequests()
		if (!root.__tenantSwitchInProgress)
			root.__startPendingTenantSwitch()
	}

	function __retryPendingRequests() {
		var queue = root.__pendingRetryQueue
		root.__pendingRetryQueue = []
		let retries = root.__recentRetries.slice()
		for (var i = 0; i < queue.length; i++) {
			var item = queue[i]
			if (item.generation !== root.__sessionGeneration) {
				root.__dropRequest(item.gqlRequestRef)
				continue
			}
			retries.push({"gqlData": item.gqlData, "requestId": item.requestId, "gqlRequestRef": item.gqlRequestRef, "time": Date.now()})
			try {
				// Sent again as it was, with the headers of its sender (the permission path decides the answer) and the new token
				if (item.gqlRequestRef && item.gqlData !== undefined && item.gqlData !== null) {
					if (item.gqlRequestRef.sessionOwner)
						item.gqlRequestRef.accessToken = root.getAccessToken()
					item.gqlRequestRef.setGqlQuery(item.gqlData, item.gqlHeaders)
				}
			} catch (e) {
				// One stale/destroyed sender (e.g. the view that issued it was closed while
				// the refresh was in flight) must not abort the rest of this batch's retries.
				console.warn("Auth: dropped a queued retry — original request sender no longer valid:", e)
			}
		}
		root.__recentRetries = retries
	}

	// The server could not answer the refresh: the session is kept, the refusals wait, the refresh is asked again
	function __onRefreshUnreachable() {
		if (!root.__refreshInProgress)
			return

		root.__refreshInProgress = false
		console.warn("Auth: refresh could not reach the server — asked again in", root.refreshRetryInterval, "ms")
		root.__refreshRetryTimer.restart()
	}

	// A remembered session could not be restored: the sign-in is shown; a refused refresh token is forgotten
	function __finishRestoreWithoutSession(isRefused) {
		if (isRefused) {
			root.clearRefreshToken()
		}

		root.superuserExistResult(root.__restoreStatus, root.__restoreError)
	}

	property Connections __applicationInfoConnections: Connections {
		target: ApplicationInfoProvider;

		function onUserModeReceived(userMode){
			root.userModeChanged(userMode);
		}

		function onSuperuserExistResult(status, error){
			// The session-restore probe below must only run for a fresh, logged-out
			// check. ApplicationInfoProvider can also refresh in the background while
			// already logged in (e.g. on WebSocket reconnect); re-running it here
			// would risk clobbering an active session.
			if (status === "EXISTS" && !root.isLoggedIn()){
				if (Qt.platform.os === "web"){
					// Check storage for existing session tokens
					let token = root.storage.value("accessToken", "");
					if (token && token !== ""){
						root.readDataFromStorage()
						root.setAccessToken(root.getAccessToken())
						root.setRefreshToken(root.userTokenProvider.refreshToken)
						root.refreshPermissions(function(){ root.loggedIn() })

						return;
					}

					root.removeDataFromStorage();
				}

				// For both platforms, settings are loaded automatically via PlatformSettings
				// Try to restore session with refresh token if available
				if (root.rememberMe && root.storedRefreshToken !== "" && root.lastUser !== "") {
					// Told on when the session cannot be restored: the application shows the sign-in on it
					root.__restoreStatus = status
					root.__restoreError = error
					root.loginWithRefreshToken(root.lastUser, root.storedRefreshToken);
					return;
				}
			}

			root.superuserExistResult(status, error)
		}
	}
	
	property UserTokenProvider userTokenProvider: UserTokenProvider {
		productId: root.productId;
		onAccepted: {
			root.__endSessionWork()
			let claims = root.__readTokenClaims(accessToken)
			root.currentTenantId = claims && claims.tenantId ? claims.tenantId : ""
			root.currentTenantName = ""
			if (!root.rememberMe)
				root.clearRefreshToken()
			root.saveRefreshTokenIfRememberMe();

			if (isTokenGlobal){
				root.setAccessToken(accessToken);
				root.setRefreshToken(refreshToken);
			}

			root.refreshPermissions(function(){
				root.loggedIn();
			});

			if (passwordExpiresInDays > 0){
				PopupManager.addWarningMessage(qsTr("Your password expires in %1 day(s). Please change it in your profile.").arg(passwordExpiresInDays), true);
				passwordExpiresInDays = 0;
			}
		}
		
		onFailed: {
			root.loginFailed(message || "");
		}

		onPasswordExpired: {
			root.passwordChangeRequired();
		}
	}
	
	function readDataFromStorage(){
		if (root.isLoggedIn())
			return
		root.__browserSessionStored = true
		userTokenProvider.accessToken = storage.value("accessToken", "");
		userTokenProvider.refreshToken = storage.value("refreshToken", "");
		userTokenProvider.userId = storage.value("userId", "");
		userTokenProvider.login = storage.value("login", "");
		userTokenProvider.systemId = storage.value("systemId", "");
		userTokenProvider.productId = storage.value("productId", "");
		userTokenProvider.permissions = normalizePermissions(storage.value("permissions", ""));
		root.currentTenantId = storage.value("tenantId", "");
		root.currentTenantName = storage.value("tenantName", "");
	}
	
	function notifyOrganizationsChanged(){
		root.organizationsChanged();
	}

	// The name of the current organization as the server reports it now, e.g. after a rename
	function updateCurrentTenantName(tenantName){
		if (root.currentTenantId === "" || root.currentTenantName === tenantName){
			return;
		}

		root.currentTenantName = tenantName;
		if (Qt.platform.os === "web" && root.__checkSharedSession())
			storage.setValue("tenantName", tenantName);
	}

	function saveDataToStorage(){
		root.__browserSessionStored = true
		storage.setValue("accessToken", userTokenProvider.accessToken);
		storage.setValue("refreshToken", userTokenProvider.refreshToken);
		storage.setValue("userId", userTokenProvider.userId);
		storage.setValue("login", userTokenProvider.login);
		storage.setValue("systemId", userTokenProvider.systemId);
		storage.setValue("productId", userTokenProvider.productId);
		storage.setValue("permissions", normalizePermissions(userTokenProvider.permissions).join(';'));
		storage.setValue("tenantId", root.currentTenantId);
		storage.setValue("tenantName", root.currentTenantName);
	}
	
	function removeDataFromStorage(){
		storage.remove("refreshToken");
		storage.remove("accessToken");
		storage.remove("userId");
		storage.remove("login");
		storage.remove("systemId");
		storage.remove("productId");
		storage.remove("permissions");
		storage.remove("tenantId");
		storage.remove("tenantName");
	}

	function saveRefreshTokenIfRememberMe(){
		if (root.rememberMe) {
			root.__loginSettingsLoading = true
			root.storedRefreshToken = userTokenProvider.refreshToken;
			root.lastUser = userTokenProvider.login;
			root.__loginSettingsLoading = false
			root.saveLoginSettings()
		}
		if (Qt.platform.os === "web")
			root.saveDataToStorage()
	}

	function clearRefreshToken(){
		root.__loginSettingsLoading = true
		root.storedRefreshToken = "";
		root.lastUser = "";
		root.rememberMe = false;
		root.__loginSettingsLoading = false
		root.saveLoginSettings()
	}

	function loginWithRefreshToken(userName, refreshToken){
		if (root.isLoggedIn())
			return
		root.__sessionGeneration++
		refreshTokenForLoginGqlSender.userName = userName;
		refreshTokenForLoginGqlSender.refreshToken = refreshToken;
		if (root.__checkRestoreSettings())
			root.__sendBound(refreshTokenForLoginGqlSender);
	}

	function __checkRestoreSettings() {
		if (!root.rememberMe)
			return true
		root.__syncLoginSettings()
		if (root.storage.value("Login_storedRefreshToken", "") !== refreshTokenForLoginGqlSender.refreshToken ||
				root.storage.value("Login_lastUser", "") !== refreshTokenForLoginGqlSender.userName) {
			root.__finishRestoreWithoutSession(false)
			return false
		}
		return true
	}

	function updateSuperuserModel(){
		ApplicationInfoProvider.updateModel();
	}

	function updateUserManagementModel(){
		ApplicationInfoProvider.updateModel();
	}
	
	function loggedUserIsSuperuser(){
		return userTokenProvider.login === "su";
	}
	
	function getLoggedUserId(){
		return userTokenProvider.login;
	}
	
	function getPermissions(){
		return userTokenProvider.permissions;
	}
	
	function getSystemId(){
		return userTokenProvider.systemId;
	}
	
	function logout(){
		// The session ends at the server too, or its refresh token and its subscriptions keep working there.
		// Sent before the local token is cleared: every runtime takes the token while the request is made
		if (root.userTokenProvider.accessToken !== ""){
			root.__syncLoginSettings()
			let token = root.__sharedRefreshToken()
			root.__sendLogout(root.getAccessToken(), token || root.userTokenProvider.refreshToken)
		}

		root.logoutForce()
	}

	function __sendLogout(accessToken, refreshToken) {
		root.__logoutToken = accessToken
		if (refreshToken)
			root.__sendBound(root.logoutGqlSender, null, refreshToken)
		else
			root.__sendWithoutAccessToken(root.logoutGqlSender)
		root.__logoutToken = ""
	}

	function __endSessionWork() {
		root.__sessionGeneration++
		root.__browserSessionStored = false
		root.__permissionsRefreshCallbacks = []
		root.__permissionsInFlight = false
		root.__permissionsRetried = false
		root.__refreshInProgress = false
		root.__refreshRetryTimer.stop()
		root.__tenantRemovalSwitchInProgress = false
		root.__tenantSwitchInProgress = false
		root.__switchAwaitingPermissions = false
		root.__authFailedDuringSwitch = false
		root.__hasPendingTenantSwitch = false
		root.__pendingTenantId = ""
		root.__activationTenantId = ""
		root.__recentRetries = []
		root.__dropPendingRequests()
	}
	
	// reasonMessage: optional user-facing explanation for forced logout (session expiry, etc.).
	// Voluntary logout() leaves it empty so no popup is shown.
	function logoutForce(reasonMessage){
		root.__syncLoginSettings()
		root.__endLocalSession(reasonMessage, root.__sharedRefreshToken() !== null)
	}

	function __endLocalSession(reasonMessage, ownsSession) {
		let ownsSettings = ownsSession && (!root.storage.value("Login_storedRefreshToken", "") || root.rememberMe)
		root.__endSessionWork()
		if (reasonMessage && reasonMessage !== ""){
			PopupManager.addWarningMessage(reasonMessage, true)
		}

		userTokenProvider.login = ""
		userTokenProvider.userId = ""
		userTokenProvider.accessToken = ""
		userTokenProvider.refreshToken = ""
		userTokenProvider.systemId = ""
		userTokenProvider.permissions = []
		currentTenantId = ""
		currentTenantName = ""
		pendingInvitations = []
		pendingInvitationsCount = 0
		setAccessToken("");
		setRefreshToken("");
		
		if (ownsSession && Qt.platform.os === "web"){
			removeDataFromStorage();
		}
		
		// Clear rememberMe state via PlatformSettings
		if (ownsSettings)
			clearRefreshToken();
		else {
			root.__loginSettingsLoading = true
			root.rememberMe = false
			root.lastUser = ""
			root.storedRefreshToken = ""
			root.__loginSettingsLoading = false
		}
		
		loggedOut();
	}
	
	function getAccessToken(){
		return userTokenProvider.accessToken;
	}

	function setAccessToken(token){
		if (userTokenProvider.isTokenGlobal){
			userTokenProvider.authorizationGqlModel.SetGlobalAccessToken(token);
		}
	}
	
	function setRefreshToken(token){
		if (userTokenProvider.isTokenGlobal){
			userTokenProvider.authorizationGqlModel.SetRefreshToken(token);
		}
	}
	
	function login(login, password){
		root.__sessionGeneration++
		userTokenProvider.authorization(login, password)
	}
	
	function getUserMode(){
		return ApplicationInfoProvider.userMode;
	}
	
	function getUserId(){
		return userTokenProvider.userId
	}

	function getTenantId(){
		return currentTenantId
	}

	function _isCurrentTenantRemoved(changeInfo) {
		if (!changeInfo || changeInfo.operation !== "removed")
			return false

		if (!root.currentTenantId || root.currentTenantId === "")
			return false

		var removedIds = changeInfo.itemIds || []
		for (var i = 0; i < removedIds.length; i++) {
			if (removedIds[i] === root.currentTenantId)
				return true
		}

		return false
	}

	function _handleRemovedTenant(changeInfo) {
		if (!_isCurrentTenantRemoved(changeInfo))
			return

		if (root.__tenantRemovalSwitchInProgress)
			return

		root.__tenantRemovalSwitchInProgress = true
		PopupManager.addInfoMessage(qsTr("The current organization has been deleted. Switching to no organization."), true)
		root.selectTenant("")
	}

	function selectTenant(tenantId){
		let normalizedTenantId = tenantId || ""
		if (__tenantSwitchInProgress || root.__refreshInProgress) {
			__pendingTenantId = normalizedTenantId
			__hasPendingTenantSwitch = true
			return
		}

		__tenantSwitchInProgress = true
		selectTenantInput.m_tenantId = normalizedTenantId
		root.__sendBound(selectTenantGqlSender, selectTenantInput)
	}

	// Activates the inactive organization and switches to it.
	function activateTenant(tenantId) {
		if (!tenantId)
			return

		root.__activationTenantId = tenantId
		if (root.currentTenantId !== "") {
			root.selectTenant("")
			return
		}

		root.__continueTenantActivation()
	}

	function __continueTenantActivation() {
		if (root.__activationTenantId === "" || root.currentTenantId !== "")
			return

		setTenantActiveInput.m_tenantId = root.__activationTenantId
		setTenantActiveInput.m_isActive = true
		root.__sendBound(setTenantActiveGqlSender, setTenantActiveInput)
	}

	function __reportTenantSelectionFailure(message) {
		root.__activationTenantId = ""
		root.tenantSelectionFailed(message)
		PopupManager.addErrorMessage(message !== "" ? qsTr("Unable to switch the organization: %1").arg(message) : qsTr("Unable to switch the organization"), true)
	}

	function __completeTenantSwitch() {
		__tenantSwitchInProgress = false
		root.__switchAwaitingPermissions = false
		// Refused after the new token was taken over, or the switch failed: the token is refreshed
		if (root.__authFailedDuringSwitch) {
			root.__authFailedDuringSwitch = false
			root.handleAuthFailure("", null)
		}

		root.__startPendingTenantSwitch()
	}

	function __startPendingTenantSwitch() {
		if (!__hasPendingTenantSwitch || root.__refreshInProgress)
			return

		let tenantId = __pendingTenantId
		__pendingTenantId = ""
		__hasPendingTenantSwitch = false
		if (tenantId !== root.currentTenantId)
			root.selectTenant(tenantId)
	}

	property RemoteCollectionChangeListener tenantCollectionListener: RemoteCollectionChangeListener {
		collectionId: "Tenants"
		currentUserId: root.userTokenProvider.userId
		autoSubscribe: false

		onRemoved: {
			console.log("RemoteCollectionChangeListener onRemoved", changeInfo)
			root._handleRemovedTenant(changeInfo)
		}

		function getHeaders() {
			return {}
		}
	}

	function isStrongUserManagement(){
		return ApplicationInfoProvider.userMode === "STRONG_USER_MANAGEMENT";
	}

	function isSimpleUserManagement(){
		return ApplicationInfoProvider.userMode === "NO_USER_MANAGEMENT" || ApplicationInfoProvider.userMode === "OPTIONAL_USER_MANAGEMENT";
	}
	
	function changePassword(userId, oldPassword, newPassword){
		changePasswordInput.m_login = userId;
		changePasswordInput.m_oldPassword = oldPassword;
		changePasswordInput.m_newPassword = newPassword;
		changePasswordGqlSender.send(changePasswordInput);
	}
	
	function registerUser(userData){
		registerUserInput.m_userData = userData;
		registerUserRequestSender.send(registerUserInput)
	}

	function setLoginData(refreshToken, accessToken, userId, login, systemId, permissions){
		userTokenProvider.setLoginData(refreshToken, accessToken, userId, login, systemId, permissions)
	}

	property RegisterUserInput registerUserInput: RegisterUserInput {
		m_productId: root.productId;
	}
	
	property GqlSdlRequestSender registerUserRequestSender: GqlSdlRequestSender {
		permissionPath: root.permissionPath
		requestType: 1; // Mutation
		gqlCommandId: ImtauthUsersSdlCommandIds.s_registerUser;

		sdlObjectComp: Component {
			RegisterUserPayload {
				onFinished: {
					if (m_id != ""){
						PopupManager.addSuccessMessage(qsTr("The user has been successfully registered"), true);
						root.registerSuccessfully();
					}
					else{
						root.registerFailed(m_message, m_violatedRules);
					}

				}
			}
		}

		function onError(message, type) {
			console.warn("Auth: registerUser request error:", message, type);
			root.registerFailed(message, []);
		}
	}
	
	property ChangePasswordInput changePasswordInput : ChangePasswordInput {}
	property GqlSdlRequestSender changePasswordGqlSender: GqlSdlRequestSender {
		permissionPath: root.permissionPath
		id: changePasswordRequestSender;
		gqlCommandId: ImtauthUsersSdlCommandIds.s_changePassword;
		
		sdlObjectComp: Component {
			ChangePasswordPayload {
				onFinished: {
					if (m_success){
						PopupManager.addSuccessMessage(qsTr("Password changed successfully"), true);
						root.changePasswordSuccessfully();
					}
					else{
						root.changePasswordFailed(m_message, m_violatedRules);
					}
				}
			}
		}

		function onError(message, type) {
			console.warn("Auth: changePassword request error:", message, type);
			root.changePasswordFailed(message, []);
		}
	}

	property GqlSdlRequestSender logoutGqlSender: GqlSdlRequestSender {
		id: logoutSender
		permissionPath: root.permissionPath
		requestType: 1;
		gqlCommandId: ImtauthAuthorizationSdlCommandIds.s_logout;
		// Without a refresh fallback, no reply can change the session.
		readonly property var sessionOwner: logoutSender
		readonly property bool detachedLogout: true
		inputObjectComp: Component {
			TokenInput {
				m_accessToken: root.__logoutToken;
			}
		}

		// The session it closes is named by this token, not by the one the client has now
		function getHeaders() {
			var headers = ({})
			headers[root.authenticationTokenHeaderId] = root.__logoutToken
			return headers
		}

	}

	property GqlSdlRequestSender logoutRefreshGqlSender: GqlSdlRequestSender {
		permissionPath: root.permissionPath
		requestType: 1;
		gqlCommandId: ImtauthSessionsSdlCommandIds.s_refreshToken;

		property string refreshToken: ""

		inputObjectComp: Component {
			RefreshTokenInput {
				m_refreshToken: root.logoutRefreshGqlSender.refreshToken;
			}
		}


		function getHeaders() {
			return root.__emptyAuthTokenHeaders()
		}
	}

	property TokenInput getPermissionsInput: TokenInput {}
	property GqlSdlRequestSender getPermissionsGqlSender: GqlSdlRequestSender {
		permissionPath: root.permissionPath
		requestType: 1;
		gqlCommandId: ImtauthAuthorizationSdlCommandIds.s_getPermissions;

		sdlObjectComp: Component {
			PermissionList {
				onFinished: {
					root.userTokenProvider.permissions = root.normalizePermissions(m_permissions);

					if (Qt.platform.os === "web"){
						root.storage.setValue("permissions", root.userTokenProvider.permissions.join(";"));
					}

					root.__completePermissionsRefresh();
				}
			}
		}

		function onError(message, type) {
			console.warn("Auth: getPermissions request error:", message, type)
			root.userTokenProvider.permissions = []
			root.__completePermissionsRefresh()
		}
	}
	
	property GqlSdlRequestSender refreshTokenGqlSender: GqlSdlRequestSender {
		permissionPath: root.permissionPath
		requestType: 1;
		gqlCommandId: ImtauthSessionsSdlCommandIds.s_refreshToken;
		inputObjectComp: Component {
			RefreshTokenInput {
				m_refreshToken: root.userTokenProvider.refreshToken;
			}
		}
		
		sdlObjectComp: Component {
			RefreshTokenPayload {
				onFinished: {
					// Signed out while it was under way: the answer must not sign in again
					if (!root.__refreshInProgress){
						return;
					}

					if (m_ok && m_userSession){
						root.__syncLoginSettings()
						if (!root.__checkSharedSession())
							return
						let key = root.__sessionKey(root.getAccessToken())
						if (m_userSession.m_userId !== root.userTokenProvider.userId ||
								(m_userSession.m_tenantId || "") !== root.currentTenantId ||
								(key && key !== root.__sessionKey(m_userSession.m_accessToken))) {
							root.logoutForce(qsTr("The signed-in session has changed. Please sign in again."))
							return
						}
						root.userTokenProvider.accessToken = m_userSession.m_accessToken;
						root.userTokenProvider.refreshToken = m_userSession.m_refreshToken;
						root.currentTenantId = m_userSession.m_tenantId || "";
						root.currentTenantName = m_userSession.m_tenantName || "";

						root.setAccessToken(m_userSession.m_accessToken);
						root.setRefreshToken(m_userSession.m_refreshToken);

						if (Qt.platform.os === "web"){
							XMLHttpRequest.QMLAuthToken = m_userSession.m_accessToken
							XMLHttpRequest.QMLAuthRefreshToken = m_userSession.m_refreshToken
						}

						root.saveRefreshTokenIfRememberMe();
						root.__permissionsInFlight = false
						root.refreshPermissions();
						root.completeTokenRefresh(true);
					}
					else {
						root.completeTokenRefresh(false);
					}
				}
			}
		}

		function getHeaders() {
			return root.__emptyAuthTokenHeaders()
		}

		onFinished: {
			if (status < 0)
				root.completeTokenRefresh(false);
		}

		// Refused as Unauthorized: the refresh token is not accepted
		function onAuthorizationLost() {
			root.completeTokenRefresh(false);
		}

		function onError(message, type, transportState) {
			console.warn("Auth: refreshToken request error:", message, type)
			// No modal dialog: a server out of reach keeps the session, any other failure ends it
			if (transportState === "Error"){
				root.__onRefreshUnreachable();
			}
			else{
				root.completeTokenRefresh(false);
			}
		}
	}

	property GqlSdlRequestSender refreshTokenForLoginGqlSender: GqlSdlRequestSender {
		permissionPath: root.permissionPath
		requestType: 1;
		gqlCommandId: ImtauthSessionsSdlCommandIds.s_refreshToken;
	
		property string userName: ""
		property string refreshToken: ""

		function getHeaders() {
			return root.__emptyAuthTokenHeaders()
		}
		
		inputObjectComp: Component {
			RefreshTokenInput {
				m_refreshToken: root.refreshTokenForLoginGqlSender.refreshToken;
			}
		}
		
		sdlObjectComp: Component {
			RefreshTokenPayload {
				onFinished: {
					if (m_ok && m_userSession){
						// Successfully restored session with refresh token
						root.userTokenProvider.accessToken = m_userSession.m_accessToken;
						root.userTokenProvider.refreshToken = m_userSession.m_refreshToken;
						root.userTokenProvider.userId = m_userSession.m_userId;
						root.userTokenProvider.login = root.refreshTokenForLoginGqlSender.userName;
						root.currentTenantId = m_userSession.m_tenantId || "";
						root.currentTenantName = m_userSession.m_tenantName || "";
						
						root.setAccessToken(m_userSession.m_accessToken);
						root.setRefreshToken(m_userSession.m_refreshToken);
						
						// Save updated refresh token
						root.saveRefreshTokenIfRememberMe();

						root.refreshPermissions(function(){
							root.loggedIn();
						});
					}
					else {
						// The remembered refresh token expired or was revoked
						root.__finishRestoreWithoutSession(true);
					}
				}
			}
		}

		onFinished: {
			// A failed request: the remembered token is kept when the server could not be reached
			if (status < 0){
				root.__finishRestoreWithoutSession(root.refreshTokenForLoginGqlSender.state !== "Error");
			}
		}

		function onAuthorizationLost() {
			root.__finishRestoreWithoutSession(true);
		}

		function onError(message, type, transportState) {
			root.__finishRestoreWithoutSession(transportState !== "Error")
		}
	}

	property SelectTenantInput selectTenantInput: SelectTenantInput {}
	property GqlSdlRequestSender selectTenantGqlSender: GqlSdlRequestSender {
		permissionPath: root.permissionPath
		requestType: 1;
		gqlCommandId: ImtauthSessionsSdlCommandIds.s_selectTenant;

		sdlObjectComp: Component {
			SelectTenantPayload {
				onFinished: {
					if (m_ok && m_userSession){
						root.__syncLoginSettings()
						if (!root.__checkSharedSession())
							return
						if (m_userSession.m_userId !== root.userTokenProvider.userId ||
								(m_userSession.m_tenantId || "") !== root.selectTenantInput.m_tenantId) {
							root.logoutForce(qsTr("The signed-in session has changed. Please sign in again."))
							return
						}
						root.__sessionGeneration++
						root.__permissionsRefreshCallbacks = []
						root.__permissionsInFlight = false
						root.__permissionsRetried = false
						root.__switchAwaitingPermissions = true
						root.__dropPendingRequests()
						root.userTokenProvider.permissions = []
						root.__tenantRemovalSwitchInProgress = false
						root.userTokenProvider.accessToken = m_userSession.m_accessToken;
						root.userTokenProvider.refreshToken = m_userSession.m_refreshToken;
						root.currentTenantId = m_userSession.m_tenantId || "";
						root.currentTenantName = m_userSession.m_tenantName || "";

						root.setAccessToken(m_userSession.m_accessToken);
						root.setRefreshToken(m_userSession.m_refreshToken);
						root.saveRefreshTokenIfRememberMe();

						if (Qt.platform.os === "web"){
							XMLHttpRequest.QMLAuthToken = m_userSession.m_accessToken
							XMLHttpRequest.QMLAuthRefreshToken = m_userSession.m_refreshToken
						}
						// The refusals so far were for the closed token
						root.__authFailedDuringSwitch = false
						// The subscriptions still hold the closed session and the previous organization
						Events.sendEvent("AccessTokenRefreshed", {})

						root.refreshPermissions(function(){
							root.tenantSelected(root.currentTenantId);
							root.__completeTenantSwitch()
							root.__continueTenantActivation()
						});
					}
					else{
						if (root.__tenantRemovalSwitchInProgress && root.selectTenantInput.m_tenantId === "") {
							// The server did not issue an unscoped session.
							root.logoutForce(qsTr("The current organization was removed. Please sign in again."))
							return
						}

						root.__tenantRemovalSwitchInProgress = false
						root.__reportTenantSelectionFailure(m_errorMessage || "")
						root.__completeTenantSwitch()
					}
				}
			}
		}

		function onError(message, type) {
			root.__tenantRemovalSwitchInProgress = false
			root.__reportTenantSelectionFailure(message || "")
			root.__completeTenantSwitch()
		}

		function onAuthorizationLost() {
			root.selectTenantGqlSender.onError(qsTr("The organization selection was refused."), "")
		}
	}

	property SetTenantActiveInput setTenantActiveInput: SetTenantActiveInput {}
	property GqlSdlRequestSender setTenantActiveGqlSender: GqlSdlRequestSender {
		permissionPath: root.permissionPath
		requestType: 1
		gqlCommandId: ImtauthTenantsSdlCommandIds.s_setTenantActive

		sdlObjectComp: Component {
			SetTenantActivePayload {
				onFinished: {
					let tenantId = root.__activationTenantId
					root.__activationTenantId = ""
					if (m_success){
						root.selectTenant(tenantId)
					}
					else{
						PopupManager.addErrorMessage(m_errorMessage || qsTr("Unable to activate the organization"), true)
					}
				}
			}
		}

		function onError(message, type) {
			root.__activationTenantId = ""
			PopupManager.addErrorMessage(message || qsTr("Unable to activate the organization"), true)
		}
	}

	// --- Sign-in with external accounts ---
	// The provider page opens in the browser; the server receives its answer, this client polls for the result.
	// A browser opens a window only within a click, so the logins are prepared before: the click opens a ready URL.

	function loadExternalLoginProviders(){
		if (!root.externalLoginEnabled){
			root.cancelExternalLogin()
			root.__clearPreparedLogins()
			root.externalLoginProviders = []
			root.externalLoginProvidersLoaded()
			return
		}

		root.__sendWithoutAccessToken(externalLoginProvidersGqlSender)
	}

	function loginWithExternalProvider(providerId){
		root.__beginExternalLogin(providerId, false)
	}

	function linkExternalAccount(providerId){
		root.__beginExternalLogin(providerId, true)
	}

	function cancelExternalLogin(){
		externalLoginPollTimer.stop()
		root.__externalLoginId = ""
		root.externalLoginInProgress = false
		root.externalLoginProviderId = ""
	}

	// Prepares a login for every offered provider: sign-in (isLink false) or linking to the signed-in user.
	function prepareExternalLogins(isLink){
		let queue = root.__prepareQueue.slice()
		for (let i = 0; i < root.externalLoginProviders.length; i++){
			queue.push({ "providerId": String(root.externalLoginProviders[i].id), "isLink": isLink })
		}
		root.__prepareQueue = queue

		root.__prepareNextExternalLogin()
	}

	function __prepareNextExternalLogin(){
		if (prepareExternalLoginGqlSender.state === "Loading" || root.__prepareQueue.length === 0){
			return
		}

		let queue = root.__prepareQueue.slice()
		let next = queue.shift()
		root.__prepareQueue = queue
		root.__preparingProvider = next

		prepareExternalLoginInput.m_providerId = next.providerId
		prepareExternalLoginInput.m_productId = root.productId
		prepareExternalLoginInput.m_link = next.isLink

		if (next.isLink){
			prepareExternalLoginGqlSender.send(prepareExternalLoginInput)
		}
		else{
			root.__sendWithoutAccessToken(prepareExternalLoginGqlSender, prepareExternalLoginInput)
		}
	}

	function __storePreparedLogin(loginId, authorizationUrl){
		let prepared = root.__preparingProvider
		root.__preparingProvider = null

		if (prepared && loginId !== "" && authorizationUrl !== ""){
			let logins = root.__preparedLogins.slice()
			logins.push({
				"providerId": prepared.providerId,
				"isLink": prepared.isLink,
				"loginId": loginId,
				"authorizationUrl": authorizationUrl,
				"preparedAt": Date.now()
			})
			root.__preparedLogins = logins
		}

		root.__prepareNextExternalLogin()
	}

	// Takes the prepared login of the provider; one older than 8 minutes is about to expire on the server.
	function __takePreparedLogin(providerId, isLink){
		let found = null
		let logins = []
		for (let i = 0; i < root.__preparedLogins.length; i++){
			let login = root.__preparedLogins[i]
			let isFresh = Date.now() - login.preparedAt < 8 * 60 * 1000
			if (found === null && isFresh && login.providerId === providerId && login.isLink === isLink){
				found = login
			}
			else if (isFresh){
				logins.push(login)
			}
		}
		root.__preparedLogins = logins

		return found
	}

	function __clearPreparedLogins(){
		root.__preparedLogins = []
		root.__prepareQueue = []
	}

	function __beginExternalLogin(providerId, isLink){
		if (root.externalLoginInProgress){
			root.cancelExternalLogin()
		}

		root.externalLoginInProgress = true
		root.externalLoginProviderId = providerId
		root.__externalLoginIsLink = isLink

		let prepared = root.__takePreparedLogin(providerId, isLink)
		if (prepared !== null){
			root.__startExternalLogin(prepared.loginId, prepared.authorizationUrl)

			// A prepared login is single-use, the next attempt gets a new one.
			let queue = root.__prepareQueue.slice()
			queue.push({ "providerId": providerId, "isLink": isLink })
			root.__prepareQueue = queue
			root.__prepareNextExternalLogin()

			return
		}

		beginExternalLoginInput.m_providerId = providerId
		beginExternalLoginInput.m_productId = root.productId
		beginExternalLoginInput.m_link = isLink

		if (isLink){
			beginExternalLoginGqlSender.send(beginExternalLoginInput)
		}
		else{
			root.__sendWithoutAccessToken(beginExternalLoginGqlSender, beginExternalLoginInput)
		}
	}

	function __startExternalLogin(loginId, authorizationUrl){
		root.__externalLoginId = loginId
		root.__externalLoginDeadline = Date.now() + 10 * 60 * 1000
		externalLoginPollTimer.start()

		Qt.openUrlExternally(authorizationUrl)
	}

	function __failExternalLogin(message){
		let isLink = root.__externalLoginIsLink
		root.cancelExternalLogin()

		let errorText = (message && message !== "") ? message : qsTr("The sign-in could not be completed")
		root.externalLoginFailed(errorText)
		if (!isLink){
			root.loginFailed(errorText)
		}
	}

	function __pollExternalLogin(){
		if (root.__externalLoginId === ""){
			externalLoginPollTimer.stop()
			return
		}

		if (Date.now() > root.__externalLoginDeadline){
			root.__failExternalLogin(qsTr("The sign-in was not completed in time. Please try again."))
			return
		}

		if (completeExternalLoginGqlSender.state === "Loading"){
			return
		}

		completeExternalLoginInput.m_loginId = root.__externalLoginId
		if (root.__externalLoginIsLink){
			completeExternalLoginGqlSender.send(completeExternalLoginInput)
		}
		else{
			root.__sendWithoutAccessToken(completeExternalLoginGqlSender, completeExternalLoginInput)
		}
	}

	function __finishExternalLogin(status, message, authorization){
		if (status === "Pending"){
			return
		}

		let providerId = root.externalLoginProviderId

		if (status === "SignedIn" && authorization){
			root.cancelExternalLogin()
			root.userTokenProvider.setLoginData(
						authorization.m_refreshToken,
						authorization.m_token,
						authorization.m_userId,
						authorization.m_username,
						authorization.m_systemId,
						root.normalizePermissions(authorization.m_permissions))
			return
		}

		if (status === "Linked"){
			root.cancelExternalLogin()
			PopupManager.addSuccessMessage(qsTr("The account has been linked"), true)
			root.externalAccountLinked(providerId)
			return
		}

		root.__failExternalLogin(message)
	}

	property Timer externalLoginPollTimer: Timer {
		interval: 2000
		repeat: true

		onTriggered: {
			root.__pollExternalLogin()
		}
	}

	property GqlSdlRequestSender externalLoginProvidersGqlSender: GqlSdlRequestSender {
		permissionPath: root.permissionPath
		gqlCommandId: ImtauthAuthorizationSdlCommandIds.s_getExternalLoginProviders

		sdlObjectComp: Component {
			ExternalLoginProviderList {
				onFinished: {
					let providers = []
					if (m_providers){
						for (let i = 0; i < m_providers.count; i++){
							let provider = m_providers.get(i).item
							if (provider){
								providers.push({
									"id": provider.m_id,
									"name": provider.m_name,
									"type": provider.m_providerType
								})
							}
						}
					}

					root.externalLoginProviders = providers
					root.externalLoginProvidersLoaded()

					if (!root.isLoggedIn()){
						root.__clearPreparedLogins()
						root.prepareExternalLogins(false)
					}
				}
			}
		}

		function createQueryParams(query){
		}

		function getHeaders(){
			return root.__emptyAuthTokenHeaders()
		}

		function onError(message, type){
			// Without the providers the page only offers the password sign-in.
			console.warn("Auth: external login providers are not available:", message)
			root.externalLoginProviders = []
		}
	}

	property BeginExternalLoginInput beginExternalLoginInput: BeginExternalLoginInput {}
	property GqlSdlRequestSender beginExternalLoginGqlSender: GqlSdlRequestSender {
		permissionPath: root.permissionPath
		requestType: 1
		gqlCommandId: ImtauthAuthorizationSdlCommandIds.s_beginExternalLogin

		sdlObjectComp: Component {
			BeginExternalLoginPayload {
				onFinished: {
					if (m_loginId === "" || m_authorizationUrl === ""){
						root.__failExternalLogin(m_message)
						return
					}

					root.__startExternalLogin(m_loginId, m_authorizationUrl)
				}
			}
		}

		function getHeaders(){
			return root.__externalLoginIsLink ? {} : root.__emptyAuthTokenHeaders()
		}

		function onError(message, type){
			root.__failExternalLogin(message)
		}
	}

	property BeginExternalLoginInput prepareExternalLoginInput: BeginExternalLoginInput {}
	property GqlSdlRequestSender prepareExternalLoginGqlSender: GqlSdlRequestSender {
		permissionPath: root.permissionPath
		requestType: 1
		gqlCommandId: ImtauthAuthorizationSdlCommandIds.s_beginExternalLogin

		sdlObjectComp: Component {
			BeginExternalLoginPayload {
				onFinished: {
					root.__storePreparedLogin(m_loginId, m_authorizationUrl)
				}
			}
		}

		function getHeaders(){
			let isLink = root.__preparingProvider ? root.__preparingProvider.isLink : false
			return isLink ? {} : root.__emptyAuthTokenHeaders()
		}

		function onError(message, type){
			// Without a prepared login the click starts the login itself.
			console.warn("Auth: external login could not be prepared:", message)
			root.__storePreparedLogin("", "")
		}
	}

	// A prepared login expires on the server after 10 minutes.
	property Timer preparedLoginsRefreshTimer: Timer {
		interval: 5 * 60 * 1000
		repeat: true
		running: root.__preparedLogins.length > 0

		onTriggered: {
			let queue = []
			for (let i = 0; i < root.__preparedLogins.length; i++){
				queue.push({ "providerId": root.__preparedLogins[i].providerId, "isLink": root.__preparedLogins[i].isLink })
			}
			root.__preparedLogins = []
			root.__prepareQueue = queue
			root.__prepareNextExternalLogin()
		}
	}

	property ExternalLoginInput completeExternalLoginInput: ExternalLoginInput {}
	property GqlSdlRequestSender completeExternalLoginGqlSender: GqlSdlRequestSender {
		permissionPath: root.permissionPath
		requestType: 1
		gqlCommandId: ImtauthAuthorizationSdlCommandIds.s_completeExternalLogin

		sdlObjectComp: Component {
			ExternalLoginPayload {
				onFinished: {
					root.__finishExternalLogin(m_status, m_message, m_authorization)
				}
			}
		}

		function getHeaders(){
			return root.__externalLoginIsLink ? {} : root.__emptyAuthTokenHeaders()
		}

		function onError(message, type){
			// A failed poll is retried by the timer until the login expires.
			console.warn("Auth: external login poll failed:", message)
		}
	}
}
