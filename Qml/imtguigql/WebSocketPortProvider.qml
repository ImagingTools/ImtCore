import QtQuick 2.0
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtbaseSettingsSdl 1.0
import imtbaseImtBaseTypesSdl 1.0

QtObject {
	id: root
	property int port: -1;
	
	function updateModel(){
		gqlSender.send();
	}
	
	property GqlSdlRequestSender gqlSender: GqlSdlRequestSender {
		gqlCommandId: ImtbaseSettingsSdlCommandIds.s_getWebSocketUrl
		sdlObjectComp: Component {
			UrlParam {
				onFinished: {
					root.port = m_port
				}
			}
		}
	}
}

