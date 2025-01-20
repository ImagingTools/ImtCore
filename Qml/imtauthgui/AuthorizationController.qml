pragma Singleton

import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtauthgui 1.0
import imtauthUsersSdl 1.0

QtObject {
	id: root;

	property string productId: "";

	signal logoutSignal();
	signal userModeChanged(string userMode);
	signal superuserExistResult(bool exists, string type, string error);
	signal loginSuccessful();
	signal loginFailed();

	Component.onCompleted: {
		Events.subscribeEvent("Login", root.login)
		Events.subscribeEvent("Logout", root.userLogout)
	}

	Component.onDestruction: {
		Events.unSubscribeEvent("Login", root.login)
		Events.unSubscribeEvent("Logout", root.userLogout)
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
			root.superuserExistResult(exists, type, error)
		}

		function getHeaders(){
			return root.getHeaders();
		}
	}

	property UserTokenProvider userTokenProvider: UserTokenProvider {
		productId: root.productId;
		onAccepted: {
			root.loginSuccessful();
		}

		onFailed: {
			root.loginFailed();
		}

		function getHeaders(){
			return root.getHeaders();
		}
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

	function userLogout(param){
		userTokenProvider.login = ""
		userTokenProvider.userId = ""
		userTokenProvider.token = ""
		userTokenProvider.permissions = []

		userTokenProvider.authorizationGqlModel.SetGlobalAccessToken("");

		root.logoutSignal();
	}

	function login(param){
		if (!param){
			return;
		}

		let login = param["Login"];
		let password = param["Password"];

		userTokenProvider.authorization(login, password)
	}

	function getToken(callback){
		if (callback){
			callback(userTokenProvider.token);
		}
	}

	function getLogin(callback){
		if (callback){
			callback(userTokenProvider.login);
		}
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
}
