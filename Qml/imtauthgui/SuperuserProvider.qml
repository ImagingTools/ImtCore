import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtguigql 1.0
import imtauthUsersSdl 1.0

QtObject {
	id: root;

	signal result(string status, string error);

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
					root.result(m_status, m_message);
				}
			}
		}

		function getHeaders(){
			return root.getHeaders();
		}

		function onError(){
			root.result(false, "", "");
		}
	}
}


