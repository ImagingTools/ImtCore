pragma Singleton

import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtauthgui 1.0
import imtauthUsersSdl 1.0
import imtauthAuthorizationSdl 1.0
import imtauthSessionsSdl 1.0

QtObject {
	id: root;

	property string productId: "";

	signal userModeChanged(string userMode);
	signal superuserExistResult(bool exists, string type, string error);
	signal loginFailed();

	signal loggedIn();
	signal loggedOut();

	property XmlHttpRequestProxy requestProxy: XmlHttpRequestProxy {
		onForbidden: {
			console.log("onForbidden", gqlData);

			root.logoutForce();
		}

		onUnauthorized: {
			console.log("onUnauthorized", gqlData);

			if (Qt.platform.os == "web"){
				refreshToken(function(success){
					if (success){statusCode == 401
						let xhr = new XMLHttpRequest()
						xhr.open("POST", "../../graphql")
						xhr.send(gqlData)
					}
				})
			}
			else{
				root.setAccessToken("");
				let cb = function(status){
					console.log("refreshTokenGqlSender finished", gqlData, status);
					if (status >= 0){
						if (gqlRequestRef){
							gqlRequestRef.setGqlQuery(gqlData, root.getHeaders());
						}
					}

					root.refreshTokenGqlSender.finished.disconnect(cb);
				}

				root.refreshTokenGqlSender.finished.connect(cb)
				root.refreshTokenGqlSender.send();
			}
		}

		function refreshToken(callback) {
			XMLHttpRequest.QMLAuthToken = "";

			let xhr = new XMLHttpRequest()
			xhr.open("POST", "../../graphql")

			var query = Gql.GqlRequest("mutation", "RefreshToken");
			var inputParams = Gql.GqlObject("input");
			inputParams.InsertField("refreshToken", XMLHttpRequest.QMLAuthRefreshToken);
			query.AddParam(inputParams);

			xhr.send(query.GetQuery())

			xhr.onreadystatechange = function(){
				if (xhr.readyState === XMLHttpRequest.DONE){
					if (xhr.status === 200){
						var response = JSON.parse(xhr.responseText)

						if ("data" in response){
							let data = response.data
							if ("RefreshToken" in data){
								let refreshToken = data.RefreshToken
								if (!refreshToken.ok){
									callback(false)
								}
								else{
									let userSession = refreshToken.userSession;

									XMLHttpRequest.QMLAuthToken = userSession.accessToken
									XMLHttpRequest.QMLAuthRefreshToken = userSession.refreshToken

									root.userTokenProvider.accessToken = XMLHttpRequest.QMLAuthToken;
									root.userTokenProvider.refreshToken = XMLHttpRequest.QMLAuthRefreshToken;

									callback(true)
								}
							}
						}
					}
					else{
						callback(false)
					}
				}
			}
		}
	}

	property UserManagementProvider userManagementProvider: UserManagementProvider {
		onUserModeChanged: {
			root.userModeChanged(userMode);
		}

		function getHeaders(){
			return root.getHeaders();
		}
	}

	property SuperuserProvider superuserProvider: SuperuserProvider {
		onResult: {
			if (exists){
				if (Qt.platform.os == "web"){
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
			}

			root.superuserExistResult(exists, type, error)
		}

		function getHeaders(){
			return root.getHeaders();
		}
	}

	property UserTokenProvider userTokenProvider: UserTokenProvider {
		productId: root.productId;
		onAccepted: {
			if (Qt.platform.os == "web"){
				root.saveDataToStorage();
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

		function getHeaders(){
			return root.getHeaders();
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

	function getHeaders(){
		return {"ProductId": root.productId}
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

		if (Qt.platform.os == "web"){
			removeDataFromStorage();
		}

		loggedOut();
	}

	function getAccessToken(){
		return userTokenProvider.accessToken;
	}

	function setAccessToken(token){
		userTokenProvider.authorizationGqlModel.SetGlobalAccessToken(token);
	}

	function setRefreshToken(token){
		userTokenProvider.authorizationGqlModel.SetRefreshToken(token);
	}

	function login(login, password){
		userTokenProvider.authorization(login, password)
	}

	function getUserMode(){
		return userManagementProvider.userMode;
	}

	function isStrongUserManagement(){
		return userManagementProvider.userMode === "STRONG_USER_MANAGEMENT";
	}

	function isSimpleUserManagement(){
		return userManagementProvider.userMode === "NO_USER_MANAGEMENT" || userManagementProvider.userMode === "OPTIONAL_USER_MANAGEMENT";
	}

	function changePassword(userId, oldPassword, newPassword){
		changePasswordGqlSender.send({"m_login":userId,"m_oldPassword":oldPassword,"m_newPassword":newPassword});
	}

	function registerUser(userData){
		registerUserRequestSender.send({"m_userData": userData})
	}

	property GqlSdlRequestSender registerUserRequestSender: GqlSdlRequestSender {
		requestType: 1; // Mutation
		gqlCommandId: ImtauthUsersSdlCommandIds.s_registerUser;

		inputObjectComp: Component {
			RegisterUserInput {
				m_productId: root.productId;
			}
		}

		sdlObjectComp: Component {
			RegisterUserPayload {
				onFinished: {
					ModalDialogManager.showInfoDialog(qsTr("The user has been successfully registered"));
				}
			}
		}
	}

	property GqlSdlRequestSender changePasswordGqlSender: GqlSdlRequestSender {
		id: changePasswordRequestSender;
		gqlCommandId: ImtauthUsersSdlCommandIds.s_changePassword;

		inputObjectComp: Component {
			ChangePasswordInput {
			}
		}

		sdlObjectComp: Component {
			ChangePasswordPayload {
				onFinished: {
					if (m_success){
						ModalDialogManager.showInfoDialog(qsTr("Password changed successfully"));
					}
				}
			}
		}
	}

	property GqlSdlRequestSender logoutGqlSender: GqlSdlRequestSender {
		gqlCommandId: ImtauthAuthorizationSdlCommandIds.s_logout;
		inputObjectComp: Component {
			LogoutInput {
			}
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

						if (Qt.platform.os == "web"){
							XMLHttpRequest.QMLAuthToken = m_userSession.m_accessToken
							XMLHttpRequest.QMLAuthRefreshToken = m_userSession.m_refreshToken
						}
					}
				}
			}
		}
	}
}
