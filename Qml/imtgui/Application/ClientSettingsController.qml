import QtQuick 2.0
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtbaseImtBaseTypesSdl 1.0

ParamsSetController {
	id: container;
	
	property string json;
	
	signal saveSettings()
	signal urlChanged();
	
	property ServerConnectionParam serverConnectionParam: ServerConnectionParam {
		onFinished: {
			container.urlChanged()
		}
	}

	function saveParam(){
		updateConnectionParam()
		saveSettings()
	}
	
	onJsonChanged: {
		if (!createParamFromJson(json)){
			console.error("Unable to create client settings from JSON")
		}
	}
	
	onParamModelCreated: {
		updateConnectionParam()
	}
	
	function updateConnectionParam(){
		let urlParamter = getParameterById("ServerConnectionInterface")
		if (urlParamter.m_data !== serverConnectionParam.toJson()){
			serverConnectionParam.createFromJson(urlParamter.m_data)
		}
	}

	function getServerUrl(){
		let scheme = "http"
		if (serverConnectionParam.m_isSecure){
			scheme = "https"
		}

		return scheme + "://" + serverConnectionParam.m_host + ":" + serverConnectionParam.m_httpPort
	}
}


