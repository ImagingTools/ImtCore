pragma Singleton

import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtguigql 1.0
import imtappApplicationSdl 1.0

/**
	Startup bootstrap data source. A single GetApplicationInfo request folds in what
	used to be four separate round-trips (GetApplicationInfo, GetWebSocketUrl,
	GetUserMode, CheckSuperuserExists) - see CApplicationInfoControllerComp on the
	server. Exposed as a singleton so both ApplicationMain (app metadata / WebSocket
	port) and AuthorizationController (user mode / superuser status) share one
	in-flight request instead of duplicating it.
*/
QtObject {
	id: root;

	property string permissionPath: ""
	property ApplicationInfo serverApplicationInfo: null;
	property int webSocketPort: -1;
	property string userMode: "";
	property string superuserStatus: "";
	property string superuserMessage: "";

	signal updated();

	signal userModeReceived(string userMode);
	signal superuserExistResult(string status, string error);

	/**
		True while the bootstrap request is in flight.
	*/
	readonly property bool loading: request.state === "Loading";

	function updateModel(){
		// Single-flight. This one request is driven from three places at startup -
		// ApplicationMain's retry Timer (every few seconds until the WebSocket port
		// arrives), AuthorizationController.updateSuperuserModel() and
		// updateUserManagementModel() - which used to stack several identical
		// GetApplicationInfo round-trips on top of each other on a slow first
		// response. GqlRequest guarantees a terminal state, so this cannot deadlock.
		if (root.loading){
			return;
		}

		request.send();
	}

	function getHeaders(){
		return {};
	}

	property GqlSdlRequestSender request : GqlSdlRequestSender {
		permissionPath: root.permissionPath
		gqlCommandId: ImtappApplicationSdlCommandIds.s_getApplicationInfo;
		sdlObjectComp: Component { ApplicationInfo {
			} }

		onFinished: {
			if (status !== 1){
				return;
			}

			root.serverApplicationInfo = sdlObject;

			if (sdlObject.m_webSocketUrl){
				root.webSocketPort = sdlObject.m_webSocketUrl.m_port;
			}

			root.updated();

			// userMode / superuserStatus are optional - only wired servers populate
			// them (see CApplicationInfoControllerComp). Unwired apps simply keep
			// their previous values instead of being reset to "".
			if (sdlObject.m_userMode){
				root.userMode = sdlObject.m_userMode;
				root.userModeReceived(root.userMode);
			}

			if (sdlObject.m_superuser && sdlObject.m_superuser.m_status){
				root.superuserStatus = sdlObject.m_superuser.m_status;
				root.superuserMessage = sdlObject.m_superuser.m_message || "";
				root.superuserExistResult(root.superuserStatus, root.superuserMessage);
			}
		}

		function getHeaders(){
			return root.getHeaders();
		}

		function onError(message, type){
			root.superuserStatus = "UNKNOWN";
			root.superuserMessage = message || qsTr("Unable to reach server");
			root.superuserExistResult(root.superuserStatus, root.superuserMessage);
		}
	}
}
