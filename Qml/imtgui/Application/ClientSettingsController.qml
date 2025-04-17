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
	
	property UrlParam urlParam: UrlParam {
		onFinished: {
			container.urlChanged()
		}
	}
	
	function saveParam(){
		updateUrlParam()
		saveSettings()
	}
	
	onJsonChanged: {
		if (!createParamFromJson(json)){
			console.error("Unable to create client settings from JSON")
		}
	}
	
	onParamModelCreated: {
		updateUrlParam()
	}
	
	function updateUrlParam(){
		let urlParamter = getParamJsonByPath(["ServerUrl"]);
		if (urlParamter != container.urlParam.toJson()){
			container.urlParam.createFromJson(urlParamter)
		}
	}

	function getServerUrl(){
		return urlParam.m_scheme + "://" + urlParam.m_host + ":" + urlParam.m_port
	}
}


