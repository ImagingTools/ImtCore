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
	
	// Load settings from LocalStorage on component creation
	Component.onCompleted: {
		loadLoginSettings();
	}
	
	// Watch for changes and save to LocalStorage
	onRememberMeChanged: saveLoginSettings()
	onLastUserChanged: saveLoginSettings()
	onStoredRefreshTokenChanged: saveLoginSettings()
	
	function loadLoginSettings() {
		let rememberMeStr = LocalStorage.getItem("Login_rememberMe");
		root.rememberMe = (rememberMeStr === "true");
		root.lastUser = LocalStorage.getItem("Login_lastUser") || "";
		root.storedRefreshToken = LocalStorage.getItem("Login_storedRefreshToken") || "";
	}
	
	function saveLoginSettings() {
		LocalStorage.setItem("Login_rememberMe", root.rememberMe ? "true" : "false");
		LocalStorage.setItem("Login_lastUser", root.lastUser);
		LocalStorage.setItem("Login_storedRefreshToken", root.storedRefreshToken);
	}
	
	function clearLoginSettings() {
		root.rememberMe = false;
		root.lastUser = "";
		root.storedRefreshToken = "";
		
		LocalStorage.removeItem("Login_rememberMe");
		LocalStorage.removeItem("Login_lastUser");
		LocalStorage.removeItem("Login_storedRefreshToken");
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
					// For web, also check LocalStorage for existing session tokens
					let token = LocalStorage.getItem("accessToken");
					let refreshToken = LocalStorage.getItem("refreshToken");
					if (token && token !== ""){
						AuthorizationController.readDataFromStorage();
						AuthorizationController.setAccessToken(token);
						AuthorizationController.setRefreshToken(refreshToken);
						AuthorizationController.loggedIn();
						
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
		userTokenProvider.accessToken = LocalStorage.getItem("accessToken");
		userTokenProvider.refreshToken = LocalStorage.getItem("refreshToken");
		userTokenProvider.userId = LocalStorage.getItem("userId");
		userTokenProvider.login = LocalStorage.getItem("login");
		userTokenProvider.systemId = LocalStorage.getItem("systemId");
		userTokenProvider.productId = LocalStorage.getItem("productId");
		userTokenProvider.permissions = LocalStorage.getItem("permissions");
	}
	
	function saveDataToStorage(){
		LocalStorage.setItem("accessToken", userTokenProvider.accessToken);
		LocalStorage.setItem("refreshToken", userTokenProvider.refreshToken);
		LocalStorage.setItem("userId", userTokenProvider.userId);
		LocalStorage.setItem("login", userTokenProvider.login);
		LocalStorage.setItem("systemId", userTokenProvider.systemId);
		LocalStorage.setItem("productId", userTokenProvider.productId);
		LocalStorage.setItem("permissions", userTokenProvider.permissions);
	}
	
	function removeDataFromStorage(){
		LocalStorage.removeItem("refreshToken");
		LocalStorage.removeItem("accessToken");
		LocalStorage.removeItem("userId");
		LocalStorage.removeItem("login");
		LocalStorage.removeItem("systemId");
		LocalStorage.removeItem("productId");
		LocalStorage.removeItem("permissions");
	}

	function saveRefreshTokenIfRememberMe(){
		if (root.rememberMe) {
			root.storedRefreshToken = userTokenProvider.refreshToken;
			root.lastUser = userTokenProvider.login;
			
			if (Qt.platform.os === "web"){
				// For web, also save to the legacy localStorage for session tokens
				saveDataToStorage();
			}
			// PlatformSettings handles persistence automatically for both platforms
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
			// For web, clear from LocalStorage
			LocalStorage.removeItem("refreshToken");
		}
		// PlatformSettings handles persistence automatically for both platforms
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
}
