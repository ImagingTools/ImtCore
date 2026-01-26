import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtbaseImtBaseTypesSdl 1.0

ParamsSetController {
	id: root
	
	signal patTokensChanged()
	
	property PatTokenListParam patTokenListParam: PatTokenListParam {}
	
	function saveParam(){
		// Save tokens to local storage or user settings
		let patTokenParameter = getParameterById("PatTokens")
		if (patTokenParameter.m_data !== patTokenListParam.toJson()){
			patTokenListParam.createFromJson(patTokenParameter.m_data)
		}
		
		// Trigger save operation
		patTokensChanged()
	}
	
	onParamModelCreated: {
		// Load existing PAT tokens
		let patTokenParameter = getParameterById("PatTokens")
		if (patTokenParameter && patTokenParameter.m_data) {
			patTokenListParam.createFromJson(patTokenParameter.m_data)
		}
	}
	
	function getPatTokens() {
		return patTokenListParam
	}
}
