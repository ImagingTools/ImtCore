pragma Singleton

import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtauthgui 1.0
import imtauthUsersSdl 1.0
import imtauthAuthorizationSdl 1.0
import imtauthSessionsSdl 1.0
import imtbaseSettingsSdl 1.0

QtObject {
	id: root;
	
	property string productId: "";
	
	signal userModeChanged(string userMode);
	signal superuserExistResult(string status, string error);
	signal loginFailed();
	
	signal loggedIn();
	signal loggedOut();
	signal changePasswordSuccessfully();
	signal changePasswordFailed();
	signal registerSuccessfully();
	signal registerFailed();

	// Properties to store remember me state and credentials
	property bool rememberMe: false
	property string lastUser: ""
	property string storedRefreshToken: ""
	
	// Flag to track if settings have been loaded from server
	property bool settingsLoaded: false
	
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
					let token = localStorage.getItem("accessToken");
					let refreshToken = localStorage.getItem("refreshToken");
					if (token && token !== ""){
						AuthorizationController.readDataFromStorage();
						AuthorizationController.setAccessToken(token);
						AuthorizationController.setRefreshToken(refreshToken);
						AuthorizationController.loggedIn();
						
						return;
					}
					
					AuthorizationController.removeDataFromStorage();
				}
				else {
					// For non-web platforms, try to load settings from server first
					// The actual login attempt will happen in loadSettingsQuery.onFinished
					root.loadLoginSettingsFromServer();
					return;
				}
			}
			
			root.superuserExistResult(status, error)
		}
	}
	
	property UserTokenProvider userTokenProvider: UserTokenProvider {
		productId: root.productId;
		onAccepted: {
			console.log("UserTokenProvider onAccepted")
			if (Qt.platform.os == "web"){
				root.saveDataToStorage();
			}
			else {
				// For non-web platforms, save refresh token if "Remember me" is enabled
				root.saveRefreshTokenIfRememberMe();
			}
			
			if (isTokenGlobal){
				root.setAccessToken(accessToken);
				root.setRefreshToken(refreshToken);
			}
			
			root.loggedIn();
		}
		
		onFailed: {
			root.loginFailed();
		}
	}
	
	function readDataFromStorage(){
		userTokenProvider.accessToken = localStorage.getItem("accessToken");
		userTokenProvider.refreshToken = localStorage.getItem("refreshToken");
		userTokenProvider.userId = localStorage.getItem("userId");
		userTokenProvider.login = localStorage.getItem("login");
		userTokenProvider.systemId = localStorage.getItem("systemId");
		userTokenProvider.productId = localStorage.getItem("productId");
		userTokenProvider.permissions = localStorage.getItem("permissions");
	}
	
	function saveDataToStorage(){
		localStorage.setItem("accessToken", userTokenProvider.accessToken);
		localStorage.setItem("refreshToken", userTokenProvider.refreshToken);
		localStorage.setItem("userId", userTokenProvider.userId);
		localStorage.setItem("login", userTokenProvider.login);
		localStorage.setItem("systemId", userTokenProvider.systemId);
		localStorage.setItem("productId", userTokenProvider.productId);
		localStorage.setItem("permissions", userTokenProvider.permissions);
	}
	
	function removeDataFromStorage(){
		localStorage.removeItem("refreshToken");
		localStorage.removeItem("accessToken");
		localStorage.removeItem("userId");
		localStorage.removeItem("login");
		localStorage.removeItem("systemId");
		localStorage.removeItem("productId");
		localStorage.removeItem("permissions");
	}

	function saveRefreshTokenIfRememberMe(){
		if (root.rememberMe) {
			root.storedRefreshToken = userTokenProvider.refreshToken;
			root.lastUser = userTokenProvider.login;
			// Save to server
			saveLoginSettingsToServer();
		}
		else {
			clearRefreshToken();
		}
	}

	function clearRefreshToken(){
		root.storedRefreshToken = "";
		root.lastUser = "";
		// Clear from server
		saveLoginSettingsToServer();
	}

	function saveLoginSettingsToServer(){
		var settings = {
			"rememberMe": root.rememberMe,
			"lastUser": root.lastUser,
			"refreshToken": root.storedRefreshToken
		};
		
		saveSettingsMutation.settingsData = JSON.stringify(settings);
		saveSettingsMutation.userId = userTokenProvider.userId !== "" ? userTokenProvider.userId : "anonymous";
		saveSettingsMutation.send();
	}

	function loadLoginSettingsFromServer(){
		if (root.settingsLoaded){
			return; // Already loaded
		}
		
		loadSettingsQuery.send();
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
		setAccessToken("");
		setRefreshToken("");
		
		if (Qt.platform.os === "web"){
			removeDataFromStorage();
		}
		else {
			// Clear refresh token on logout for non-web platforms
			clearRefreshToken();
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
		userTokenProvider.authorization(login, password)
	}
	
	function getUserMode(){
		return userManagementProvider.userMode;
	}
	
	function getUserId(){
		return userTokenProvider.userId
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
						
						root.setAccessToken(m_userSession.m_accessToken);
						
						if (Qt.platform.os === "web"){
							XMLHttpRequest.QMLAuthToken = m_userSession.m_accessToken
							XMLHttpRequest.QMLAuthRefreshToken = m_userSession.m_refreshToken
							root.saveDataToStorage()
						}
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
				m_refreshToken: refreshTokenForLoginGqlSender.refreshToken;
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
						root.userTokenProvider.login = refreshTokenForLoginGqlSender.userName;
						
						root.setAccessToken(m_userSession.m_accessToken);
						root.setRefreshToken(m_userSession.m_refreshToken);
						
						// Save updated refresh token
						root.saveRefreshTokenIfRememberMe();
						
						root.loggedIn();
					}
					else {
						// Refresh token login failed, clear stored token
						root.clearRefreshToken();
					}
				}
			}
		}
	}

	// GQL sender for loading login settings from server
	property GqlSdlRequestSender loadSettingsQuery: GqlSdlRequestSender {
		gqlCommandId: ImtbaseSettingsSdlCommandIds.s_getSettings
		
		inputObjectComp: Component {
			GetSettingsInput {
				// Use a special anonymous user ID for pre-login settings
				m_userId: "anonymous"
			}
		}

		sdlObjectComp: Component {
			ParamsSet {
				onFinished: {
					root.settingsLoaded = true;
					
					// Parse the settings JSON
					try {
						var settingsJson = this.toJson();
						if (settingsJson && settingsJson !== "") {
							var settings = JSON.parse(settingsJson);
							
							if (settings.rememberMe !== undefined) {
								root.rememberMe = settings.rememberMe;
							}
							if (settings.lastUser !== undefined) {
								root.lastUser = settings.lastUser;
							}
							if (settings.refreshToken !== undefined) {
								root.storedRefreshToken = settings.refreshToken;
							}
							
							// Now try to restore session with refresh token if available
							if (root.rememberMe && root.storedRefreshToken !== "" && root.lastUser !== "") {
								root.loginWithRefreshToken(root.lastUser, root.storedRefreshToken);
								return;
							}
						}
					}
					catch (e) {
						console.log("Failed to parse login settings from server. JSON: " + settingsJson + ", Error: " + e);
					}
					
					// If no auto-login, show the login page
					root.superuserExistResult("EXISTS", "");
				}
			}
		}
	}

	// GQL sender for saving login settings to server
	property GqlSdlRequestSender saveSettingsMutation: GqlSdlRequestSender {
		requestType: 1 // Mutation
		gqlCommandId: ImtbaseSettingsSdlCommandIds.s_setSettings
		
		property string settingsData: ""
		property string userId: "anonymous"
		
		inputObjectComp: Component {
			SetSettingsInput {
				m_userId: saveSettingsMutation.userId
				m_settings: saveSettingsMutation.settingsData
			}
		}

		sdlObjectComp: Component {
			SetSettingsPayload {
				onFinished: {
					if (!m_ok) {
						console.log("Failed to save login settings to server for userId: " + saveSettingsMutation.userId);
					}
				}
			}
		}
	}
}
