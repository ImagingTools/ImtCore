import QtQuick 2.12
import Acf 1.0
import imtguigql 1.0
import imtauthUsersSdl 1.0

QtObject {
	id: root;

	signal result(bool exists, string type, string error);

	signal modelStateChanged(string state);

	function superuserExists(){
		request.send();
	}

	function getHeaders(){
		return {};
	}

	property GqlSdlRequestSender request : GqlSdlRequestSender {
		gqlCommandId: ImtauthUsersSdlCommandIds.s_checkSuperuserExists;

		inputObjectComp: Component {
			CheckSuperuserInput {
			}
		}

		sdlObjectComp: Component {
			CheckSuperuserPayload {
				onFinished: {
					root.result(m_exists, m_errorType, m_message);
				}
			}
		}

		function getHeaders(){
			return root.getHeaders();
		}

		function onError(){
			root.result(false, "");
		}
	}
}


