import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtbaseImtBaseTypesSdl 1.0

ParamsSetController {
	id: root
	
	readonly property string emptyTokensJson: "{\"tokens\":[]}"
	paramModel: ParamsSet {}
	
	Component.onCompleted: {
		addParam("PatTokenList", "PatTokens", qsTr("PAT Tokens"), qsTr("Personal Access Tokens for API authentication"), emptyTokensJson)
	}

	function saveParam(){
		// Save is handled by the parameter system automatically
	}
}
