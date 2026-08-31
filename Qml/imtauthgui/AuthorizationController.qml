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
import imtauthTenantMembershipsSdl 1.0
import Qt.labs.settings 1.0

QtObject {
	id: root;
	
	property string context: ""
	property string productId: "";
	
	signal userModeChanged(string userMode);
	signal superuserExistResult(string status, string error);
	signal loginFailed(string message);
	
	signal loggedIn();
	signal loggedOut();
	signal tenantSelected(string tenantId);
	signal tenantSelectionFailed(string error);
	signal changePasswordSuccessfully();
	signal changePasswordFailed();
	signal registerSuccessfully();
	signal registerFailed();

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

	// Properties to store remember me state and credentials
	property bool rememberMe: false
	property string lastUser: ""
	property string storedRefreshToken: ""
	property string currentTenantId: ""
	property string currentTenantName: ""
	property var __permissionsRefreshCallbacks: []
	property bool __tenantRemovalSwitchInProgress: false
	property bool __tenantSwitchInProgress: false
	property string __pendingTenantId: ""
	property bool __hasPendingTenantSwitch: false

	property bool __refreshInProgress: false
	property var __pendingRetryQueue: []
	readonly property string authenticationTokenHeaderId: "x-authentication-token"

	// --- Pending invitations tracking ---
	property int pendingInvitationsCount: 0
	property var pendingInvitations: []

	function refreshPendingInvitations() {
		if (!root.userTokenProvider.accessToken || root.userTokenProvider.accessToken === "")
			return
		__pendingInvitationsRequest.send(__pendingInvitationsInput)
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
		context: root.context
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
	onRememberMeChanged: saveLoginSettings()
	onLastUserChanged: saveLoginSettings()
	onStoredRefreshTokenChanged: saveLoginSettings()

	// Refresh pending invitations on relevant events (including when sender revokes)
	onLoggedIn: {
		refreshPendingInvitations()
		__membershipSubscription.registerSubscription()
		tenantCollectionListener.registerSubscription()
	}
	onLoggedOut: {
		__membershipSubscription.unRegisterSubscription()
		tenantCollectionListener.unRegisterSubscription()
		__refreshInProgress = false
		__pendingRetryQueue = []
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
			var tName = tenantName ? tenantName : qsTr("a tenant")

			if (notificationType === "InvitationReceived" || notificationType === 0) {
				PopupManager.addInfoMessage(qsTr("You have been invited to join \"%1\"").arg(tName), false)
				root.tenantInvitationReceived(tenantId, tenantName, role)
			} else if (notificationType === "InvitationAccepted" || notificationType === 1) {
				PopupManager.addInfoMessage(qsTr("Invitation accepted for \"%1\"").arg(tName), false)
				root.tenantInvitationAccepted(tenantId, membershipId)
			} else if (notificationType === "InvitationRejected" || notificationType === 2) {
				PopupManager.addInfoMessage(qsTr("Invitation rejected for \"%1\"").arg(tName), false)
				root.tenantInvitationRejected(tenantId, membershipId)
			} else if (notificationType === "OwnershipTransferred" || notificationType === 3) {
				PopupManager.addInfoMessage(qsTr("Ownership transferred for \"%1\"").arg(tName), false)
				root.tenantOwnershipTransferred(tenantId)
			} else if (notificationType === "MembershipRoleChanged" || notificationType === 4) {
				PopupManager.addInfoMessage(qsTr("Role changed in \"%1\"").arg(tName), false)
				root.tenantMembershipRoleChanged(tenantId, userId, role)
			} else if (notificationType === "MembershipRemoved" || notificationType === 5) {
				PopupManager.addInfoMessage(qsTr("Removed from \"%1\"").arg(tName), false)
				root.tenantMembershipRemoved(tenantId, userId)
			} else if (notificationType === "InvitationRevoked" || notificationType === 6) {
				PopupManager.addInfoMessage(qsTr("Invitation to join \"%1\" was revoked").arg(tName), false)
				root.tenantInvitationRevoked(tenantId, membershipId)
			}
		}
	}

	onProductIdChanged: {
		if (Qt.platform.os !== "web" && productId !== ""){
			userTokenProvider.authorizationGqlModel.SetProductId(productId)
		}
	}
	
	function loadLoginSettings() {
		let rememberMeStr = storage.value("Login_rememberMe", "");
		root.rememberMe = (rememberMeStr === "true");
		root.lastUser = storage.value("Login_lastUser", "");
		root.storedRefreshToken = storage.value("Login_storedRefreshToken", "");
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

		getPermissionsInput.m_accessToken = root.userTokenProvider.accessToken;
		getPermissionsGqlSender.send(getPermissionsInput);
	}

	function __completePermissionsRefresh(){
		let callbacks = root.__permissionsRefreshCallbacks;
		root.__permissionsRefreshCallbacks = [];

		for (let i = 0; i < callbacks.length; i++){
			callbacks[i](true);
		}
	}
	
	function saveLoginSettings() {
		storage.setValue("Login_rememberMe", root.rememberMe ? "true" : "false");
		storage.setValue("Login_lastUser", root.lastUser);
		storage.setValue("Login_storedRefreshToken", root.storedRefreshToken);
	}
	
	function clearLoginSettings() {
		root.rememberMe = false;
		root.lastUser = "";
		root.storedRefreshToken = "";
		
		storage.remove("Login_rememberMe");
		storage.remove("Login_lastUser");
		storage.remove("Login_storedRefreshToken");
	}
	
	property XmlHttpRequestProxy requestProxy: XmlHttpRequestProxy {
		onForbidden: {
			// Access denied by permissions/role — session itself is not stale, so a
			// token refresh cannot fix it. Log out immediately, no retry.
			console.warn("Auth: HTTP 403 Forbidden — logout")
			root.logoutForce(qsTr("Access denied. Please sign in again."))
		}

		onUnauthorized: {
			// Access token expired/rejected by server — reactive recovery via refresh token.
			console.warn("Auth: HTTP 401 Unauthorized")
			root.handleAuthFailure(gqlData, gqlRequestRef)
		}

		onInternalError: {
			console.warn("Auth: HTTP 500 Internal Error on GraphQL request")
		}
	}

	// WebSocket subscription path reports the same Unauthorized/Forbidden prefixes.
	function onAccessTokenUnauthorizedEvent(parameters) {
		console.warn("Auth: WebSocket auth failure (401) — attempting refresh")
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
		var savedToken = userTokenProvider.authorizationGqlModel.GetGlobalAccessToken()
		userTokenProvider.authorizationGqlModel.SetGlobalAccessToken("")
		sender.send(input)
		userTokenProvider.authorizationGqlModel.SetGlobalAccessToken(savedToken)
	}

	function handleAuthFailure(gqlData, gqlRequestRef) {
		if (gqlRequestRef) {
			var queue = root.__pendingRetryQueue.slice()
			queue.push({
				"gqlData": gqlData,
				"gqlRequestRef": gqlRequestRef
			})
			root.__pendingRetryQueue = queue
		}

		if (root.__refreshInProgress)
			return

		if (!root.userTokenProvider.refreshToken || root.userTokenProvider.refreshToken === "") {
			if (!root.isLoggedIn()) {
				root.__pendingRetryQueue = []
				return
			}

			console.warn("Auth: no refresh token — logout")
			root.__pendingRetryQueue = []
			root.logoutForce(qsTr("Your session is no longer valid. Please sign in again."))
			return
		}

		root.__refreshInProgress = true

		root.__sendWithoutAccessToken(root.refreshTokenGqlSender)
	}

	function completeTokenRefresh(ok) {
		if (!root.__refreshInProgress)
			return

		root.__refreshInProgress = false

		if (!ok) {
			// Session/refresh rejected by server → logout.
			console.warn("Auth: refresh failed — logout")
			root.__pendingRetryQueue = []
			root.logoutForce(qsTr("Your session has expired. Please sign in again."))
			return
		}

		console.log("Auth: refresh succeeded")
		// Re-register WS subscriptions that failed with the old access token.
		Events.sendEvent("AccessTokenRefreshed", {})

		var queue = root.__pendingRetryQueue
		root.__pendingRetryQueue = []
		for (var i = 0; i < queue.length; i++) {
			var item = queue[i]
			try {
				if (item.gqlRequestRef && item.gqlData !== undefined && item.gqlData !== null) {
					item.gqlRequestRef.setGqlQuery(item.gqlData)
				}
			} catch (e) {
				// One stale/destroyed sender (e.g. the view that issued it was closed while
				// the refresh was in flight) must not abort the rest of this batch's retries.
				console.warn("Auth: dropped a queued retry — original request sender no longer valid:", e)
			}
		}
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
					let refreshToken = root.storage.value("refreshToken", "");
					if (token && token !== ""){
						AuthorizationController.readDataFromStorage();
						AuthorizationController.setAccessToken(token);
						AuthorizationController.setRefreshToken(refreshToken);
							AuthorizationController.refreshPermissions(function(){
								AuthorizationController.loggedIn();
							});

						return;
					}

					AuthorizationController.removeDataFromStorage();
				}

				// For both platforms, settings are loaded automatically via PlatformSettings
				// Try to restore session with refresh token if available
				if (root.rememberMe && root.storedRefreshToken !== "" && root.lastUser !== "") {
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
			root.saveRefreshTokenIfRememberMe();

			if (isTokenGlobal){
				root.setAccessToken(accessToken);
				root.setRefreshToken(refreshToken);
			}

			root.refreshPermissions(function(){
				root.loggedIn();
			});
		}
		
		onFailed: {
			root.loginFailed(message || "");
		}
	}
	
	function readDataFromStorage(){
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
	
	function saveDataToStorage(){
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
			root.storedRefreshToken = userTokenProvider.refreshToken;
			root.lastUser = userTokenProvider.login;
			
			if (Qt.platform.os === "web"){
				saveDataToStorage();
			}
		}
		else {
			clearRefreshToken();
		}
	}

	function clearRefreshToken(){
		root.storedRefreshToken = "";
		root.lastUser = "";
		root.rememberMe = false;
		
		if (Qt.platform.os === "web"){
			storage.remove("refreshToken");
		}
	}

	function loginWithRefreshToken(userName, refreshToken){
		refreshTokenForLoginGqlSender.userName = userName;
		refreshTokenForLoginGqlSender.refreshToken = refreshToken;
		root.__sendWithoutAccessToken(refreshTokenForLoginGqlSender);
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
		logoutForce();
		// logoutGqlSender.send();
	}
	
	// reasonMessage: optional user-facing explanation for forced logout (session expiry, etc.).
	// Voluntary logout() leaves it empty so no popup is shown.
	function logoutForce(reasonMessage){
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
		// Drop, do not invoke: the queued continuations emit loggedIn().
		__permissionsRefreshCallbacks = []
		setAccessToken("");
		setRefreshToken("");
		
		if (Qt.platform.os === "web"){
			removeDataFromStorage();
		}
		
		// Clear rememberMe state via PlatformSettings
		clearRefreshToken();
		
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
		if (__tenantSwitchInProgress) {
			__pendingTenantId = normalizedTenantId
			__hasPendingTenantSwitch = true
			return
		}

		__tenantSwitchInProgress = true
		selectTenantInput.m_tenantId = normalizedTenantId
		selectTenantGqlSender.send(selectTenantInput)
	}

	function __completeTenantSwitch() {
		__tenantSwitchInProgress = false
		if (!__hasPendingTenantSwitch)
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
		context: root.context
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
						root.registerFailed();
					}

				}
			}
		}
	}
	
	property ChangePasswordInput changePasswordInput : ChangePasswordInput {}
	property GqlSdlRequestSender changePasswordGqlSender: GqlSdlRequestSender {
		context: root.context
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
						root.changePasswordFailed();
					}
				}
			}
		}
	}
	
	property GqlSdlRequestSender logoutGqlSender: GqlSdlRequestSender {
		context: root.context
		gqlCommandId: ImtauthAuthorizationSdlCommandIds.s_logout;
		inputObjectComp: Component {
			TokenInput {}
		}
		sdlObjectComp: Component {
			LogoutPayload {
				onFinished: {
					root.logoutForce();
				}
			}
		}
	}

	property TokenInput getPermissionsInput: TokenInput {}
	property GqlSdlRequestSender getPermissionsGqlSender: GqlSdlRequestSender {
		context: root.context
		requestType: 1;
		gqlCommandId: ImtauthAuthorizationSdlCommandIds.s_getPermissions;

		sdlObjectComp: Component {
			PermissionList {
				onFinished: {
					root.userTokenProvider.permissions = root.normalizePermissions(m_permissions);

					if (Qt.platform.os === "web"){
						root.saveDataToStorage();
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
		context: root.context
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
					if (m_ok && m_userSession){
						root.userTokenProvider.accessToken = m_userSession.m_accessToken;
						root.userTokenProvider.refreshToken = m_userSession.m_refreshToken;
						root.currentTenantId = m_userSession.m_tenantId || "";
						root.currentTenantName = m_userSession.m_tenantName || "";

						root.setAccessToken(m_userSession.m_accessToken);
						root.setRefreshToken(m_userSession.m_refreshToken);

						if (Qt.platform.os === "web"){
							XMLHttpRequest.QMLAuthToken = m_userSession.m_accessToken
							XMLHttpRequest.QMLAuthRefreshToken = m_userSession.m_refreshToken
							root.saveDataToStorage()
						}

						root.saveRefreshTokenIfRememberMe();
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

		function onError(message, type) {
			console.warn("Auth: refreshToken request error:", message, type)
			// Suppress modal dialogs for refresh failures — completeTokenRefresh handles logout.
			root.completeTokenRefresh(false);
		}
	}

	property GqlSdlRequestSender refreshTokenForLoginGqlSender: GqlSdlRequestSender {
		context: root.context
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
						// Refresh token login failed, clear stored token
						root.clearRefreshToken();
					}
				}
			}
		}
	}

	property SelectTenantInput selectTenantInput: SelectTenantInput {}
	property GqlSdlRequestSender selectTenantGqlSender: GqlSdlRequestSender {
		context: root.context
		requestType: 1;
		gqlCommandId: ImtauthSessionsSdlCommandIds.s_selectTenant;

		sdlObjectComp: Component {
			SelectTenantPayload {
				onFinished: {
					if (m_ok && m_userSession){
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
							root.saveDataToStorage()
						}

						root.refreshPermissions(function(){
							root.tenantSelected(root.currentTenantId);
							root.__completeTenantSwitch()
						});
					}
					else{
						if (root.__tenantRemovalSwitchInProgress && root.selectTenantInput.m_tenantId === "") {
							root.__tenantRemovalSwitchInProgress = false
							root.currentTenantId = ""
							root.currentTenantName = ""
							if (Qt.platform.os === "web")
								root.saveDataToStorage()
							root.refreshPermissions(function(){
								root.tenantSelected("")
								root.__completeTenantSwitch()
							})
							return
						}

						root.__tenantRemovalSwitchInProgress = false
						root.tenantSelectionFailed(m_errorMessage || "");
						root.__completeTenantSwitch()
					}
				}
			}
		}

		function onError(message, type) {
			root.__tenantRemovalSwitchInProgress = false
			root.tenantSelectionFailed(message || "")
			root.__completeTenantSwitch()
		}
	}
}
