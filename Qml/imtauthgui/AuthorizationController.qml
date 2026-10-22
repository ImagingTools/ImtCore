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
	
	signal userModeChanged(string userMode);
	signal superuserExistResult(string status, string error);
	signal loginFailed(string message);
	
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
	// Membership notifications were lost while the connection was down; membership views re-read their data.
	signal membershipNotificationsResumed();

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
	// An inactive organization cannot be selected; it is activated outside of any organization first.
	property string __activationTenantId: ""

	property bool __refreshInProgress: false
	property var __pendingRetryQueue: []

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
		__pendingInvitationsRequest.send(__pendingInvitationsInput)
	}

	function __startTenantTracking() {
		if (!root.tenantsEnabled)
			return
		root.refreshPendingInvitations()
		__membershipSubscription.registerSubscription()
		tenantCollectionListener.registerSubscription()
	}

	function __stopTenantTracking() {
		__membershipSubscription.unRegisterSubscription()
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
	onRememberMeChanged: saveLoginSettings()
	onLastUserChanged: saveLoginSettings()
	onStoredRefreshTokenChanged: saveLoginSettings()

	// Refresh pending invitations on relevant events (including when sender revokes)
	onExternalLoginEnabledChanged: {
		root.loadExternalLoginProviders()
	}

	onLoggedIn: {
		root.__clearPreparedLogins()
		root.__startTenantTracking()
	}
	onLoggedOut: {
		root.__clearPreparedLogins()
		root.__stopTenantTracking()
		__refreshInProgress = false
		__pendingRetryQueue = []
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
			var tName = tenantName ? tenantName : qsTr("an organization")

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

		onRefreshRequired: {
			root.refreshPendingInvitations()
			root.membershipNotificationsResumed()
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
		setTenantActiveGqlSender.send(setTenantActiveInput)
	}

	function __reportTenantSelectionFailure(message) {
		root.__activationTenantId = ""
		root.tenantSelectionFailed(message)
		PopupManager.addErrorMessage(message !== "" ? qsTr("Unable to switch the organization: %1").arg(message) : qsTr("Unable to switch the organization"), true)
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
		permissionPath: root.permissionPath
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
		permissionPath: root.permissionPath
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
						// Refresh token login failed, clear stored token
						root.clearRefreshToken();
					}
				}
			}
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
							root.__continueTenantActivation()
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
