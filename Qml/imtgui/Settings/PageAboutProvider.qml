import QtQuick 2.0
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtbaseImtBaseTypesSdl 1.0

ParamsSetController {
	id: root;
	
	paramModel: ParamsSet {}
	
	property Component serverLogProvider: Component {
		ServerLogProvider {}
	}
	
	property string serverVersion
	property string authServerVersion
	
	Component.onCompleted: {
		createRepresentation()
	}
	
	onServerVersionChanged: {
		serverVersionText.m_text = root.serverVersion
		createRepresentation()
	}
	
	onAuthServerVersionChanged: {
		authServerVersionText.m_text = root.authServerVersion
		createRepresentation()
	}
	
	property TextParam serverVersionText: TextParam {
		m_text: root.serverVersion
	}
	
	property TextParam authServerVersionText: TextParam {
		m_text: root.authServerVersion
	}
	
	// "1.0.11563 - Beta Version (03.11.2026 14:25)", buildTime is an ISO date in UTC
	function formatVersion(version, versionKind, buildTime){
		let text = version ? String(version) : ""
		if (versionKind){
			text = text === "" ? String(versionKind) : text + " - " + String(versionKind)
		}
		if (buildTime){
			let buildTimeText = Qt.formatDateTime(new Date(String(buildTime)), "dd.MM.yyyy hh:mm")
			text = text === "" ? buildTimeText : text + " (" + buildTimeText + ")"
		}
	
		return text
	}
	
	function createRepresentation(){
		clearParamsSet()
	
		addParam(ParamTypeIdsTypeMetaInfo.s_textView, "ServerVersion", qsTr("Server Version"), "", serverVersionText.toJson())
		if (root.authServerVersion !== ""){
			addParam(ParamTypeIdsTypeMetaInfo.s_textView, "AuthServerVersion", qsTr("Puma Version"), "", authServerVersionText.toJson())
		}
		addParam("ServerLog", "ServerLog", qsTr("Server Log"), "", "")
	}
}
