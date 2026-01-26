import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtbaseImtBaseTypesSdl 1.0

ParamsSetController {
	id: root
	
	Component.onCompleted: {
		// Initialize PAT tokens parameter
		paramsSetController.addParam("PatTokenList", "PatTokens", qsTr("PAT Tokens"), qsTr("Personal Access Tokens for API authentication"), "{\"tokens\":[]}")
	}
	
	function saveParam(){
		// Save is handled by the parameter system automatically
	}
}
