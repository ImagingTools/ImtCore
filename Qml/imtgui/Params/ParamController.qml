import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtbaseImtBaseTypesSdl 1.0

QtObject {
	id: root
	
	property Component paramFactoryComp: null
	property var paramModel: null
	
	signal paramModelCreated()
	
	function createParamFromJson(json, beQuiet){
		if (!paramFactoryComp){
			return
		}
		
		if (!beQuiet){
			beQuiet = false
		}

		if (paramModel){
			paramModel.destroy()
		}

		paramModel = paramFactoryComp.createObject(root)
		if (!paramModel){
			console.error("Unable to create parameter. Create object failed")
			return false
		}

		if (json && json !== ""){
			paramModel.createFromJson(json)
		}
		
		if (!beQuiet){
			paramModelCreated()
		}
		
		return true
	}
	
	function getParamComp(){
		return paramFactoryComp
	}
	
	function getParam(){
		return paramModel
	}
	
	function saveParam(){}
}
