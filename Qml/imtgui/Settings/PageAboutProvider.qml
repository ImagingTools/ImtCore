import QtQuick 2.0
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcontrols 1.0
import imtbaseImtBaseTypesSdl 1.0

ParamsSetController {
	id: root;
	
	paramModel: ParamsSet {}
	
	property Component serverLogProvider: Component {
		ServerLogProvider {}
	}
	
	property string serverVersion
	
	Component.onCompleted: {
		createRepresentation()
	}
	
	onServerVersionChanged: {
		serverVersionText.m_text = root.serverVersion
		createRepresentation()
	}
	
	property TextParam serverVersionText: TextParam {
		m_text: root.serverVersion
	}
	
	function createRepresentation(){
		clearParamsSet()
	
		addParam(ParamTypeIdsTypeMetaInfo.s_textView, "ServerVersion", qsTr("Server Version"), "", serverVersionText.toJson())
		addParam("ServerLog", "ServerLog", qsTr("Server Log"), "", "")
	}
}
