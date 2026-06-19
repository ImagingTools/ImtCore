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
import Qt.labs.settings 1.0

QtObject {
	id: root;
	
	property string productId: "";
	
	signal userModeChanged(string userMode);
	signal superuserExistResult(string status, string error);
	signal loginFailed();
	
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
	signal tenantInvitationReceived(var notification);
	signal tenantInvitationAccepted(var notification);
	signal tenantInvitationRejected(var notification);
	signal tenantOwnershipTransferred(var notification);
	signal tenantMembershipRoleChanged(var notification);
	signal tenantMembershipRemoved(var notification);

	// Properties to store remember me state and credentials
	property bool rememberMe: false
	property string lastUser: ""
	property string storedRefreshToken: ""
	property string currentTenantId: ""
	property string currentTenantName: ""
	property var __permissionsRefreshCallback: null
	property bool __tenantRemovalSwitchInProgress: false

	property Settings storage: Settings {
		category: "AuthorizationController"
	}
	
	// Load settings from LocalStorage on component creation
	Component.onCompleted: {
		loadLoginSettings();
	}
	
	// Watch for changes and save to LocalStorage
	onRememberMeChanged: saveLoginSettings()
	onLastUserChanged: saveLoginSettings()
	onStoredRefreshTokenChanged: saveLoginSettings()

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

		root.__permissionsRefreshCallback = callback || null;
		getPermissionsInput.m_accessToken = root.userTokenProvider.accessToken;
		getPermissionsGqlSender.send(getPermissionsInput);
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
			root.logoutForce();
		}

		onUnauthorized: {
			root.setAccessToken("");
			let cb = function(status){
				if (status >= 0){
					if (gqlRequestRef){
						gqlRequestRef.setGqlQuery(gqlData);
					}
				}
				
				root.refreshTokenGqlSender.finished.disconnect(cb);
			}
			
			root.refreshTokenGqlSender.finished.connect(cb)
			root.refreshTokenGqlSender.send();
		}
	}
	
	property UserManagementProvider userManagementProvider: UserManagementProvider {
		onUserModeChanged: {
			root.userModeChanged(userMode);
		}
		
	}
	
	property SuperuserProvider superuserProvider: SuperuserProvider {
		onResult: {
			if (status === "EXISTS"){
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
			root.loginFailed();
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
		refreshTokenForLoginGqlSender.send();
	}

	function updateSuperuserModel(){
		superuserProvider.superuserExists();
	}
	
	function updateUserManagementModel(){
		userManagementProvider.updateModel();
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
	
	function logoutForce(){
		userTokenProvider.login = ""
		userTokenProvider.userId = ""
		userTokenProvider.accessToken = ""
		userTokenProvider.refreshToken = ""
		userTokenProvider.systemId = ""
		userTokenProvider.permissions = []
		currentTenantId = ""
		currentTenantName = ""
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
		return userManagementProvider.userMode;
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
		ModalDialogManager.showInfoDialog(qsTr("The current organization has been deleted. Switching to no organization."))
		root.selectTenant("")
	}

	function selectTenant(tenantId){
		selectTenantInput.m_tenantId = tenantId
		selectTenantGqlSender.send(selectTenantInput)
	}

	property RemoteCollectionChangeListener tenantCollectionListener: RemoteCollectionChangeListener {
		collectionId: "Tenants"
		currentUserId: root.userTokenProvider.userId

		onRemoved: {
			console.log("RemoteCollectionChangeListener onRemoved", changeInfo)
			root._handleRemovedTenant(changeInfo)
		}

		function getHeaders() {
			return {}
		}
	}

	function isStrongUserManagement(){
		return userManagementProvider.userMode === "STRONG_USER_MANAGEMENT";
	}
	
	function isSimpleUserManagement(){
		return userManagementProvider.userMode === "NO_USER_MANAGEMENT" || userManagementProvider.userMode === "OPTIONAL_USER_MANAGEMENT";
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
		requestType: 1; // Mutation
		gqlCommandId: ImtauthUsersSdlCommandIds.s_registerUser;

		sdlObjectComp: Component {
			RegisterUserPayload {
				onFinished: {
					if (m_id != ""){
						ModalDialogManager.showInfoDialog(qsTr("The user has been successfully registered"));
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
		id: changePasswordRequestSender;
		gqlCommandId: ImtauthUsersSdlCommandIds.s_changePassword;
		
		sdlObjectComp: Component {
			ChangePasswordPayload {
				onFinished: {
					if (m_success){
						ModalDialogManager.showInfoDialog(qsTr("Password changed successfully"));
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
		requestType: 1;
		gqlCommandId: ImtauthAuthorizationSdlCommandIds.s_getPermissions;

		sdlObjectComp: Component {
			PermissionList {
				onFinished: {
					root.userTokenProvider.permissions = root.normalizePermissions(m_permissions);

					if (Qt.platform.os === "web"){
						root.saveDataToStorage();
					}

					if (root.__permissionsRefreshCallback){
						let callback = root.__permissionsRefreshCallback;
						root.__permissionsRefreshCallback = null;
						callback(true);
					}
				}
			}
		}
	}
	
	property GqlSdlRequestSender refreshTokenGqlSender: GqlSdlRequestSender {
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
					if (m_ok){
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

						root.refreshPermissions();
					}
				}
			}
		}
	}

	property GqlSdlRequestSender refreshTokenForLoginGqlSender: GqlSdlRequestSender {
		requestType: 1;
		gqlCommandId: ImtauthSessionsSdlCommandIds.s_refreshToken;
		
		property string userName: ""
		property string refreshToken: ""
		
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
							})
							return
						}

						root.__tenantRemovalSwitchInProgress = false
						root.tenantSelectionFailed(m_errorMessage || "");
					}
				}
			}
		}
	}
}
