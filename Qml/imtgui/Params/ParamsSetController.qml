import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtbaseImtBaseTypesSdl 1.0

ParamController {
	id: root
	
	property ParamsSet paramsSet: paramModel
	paramFactoryComp: Component {
		ParamsSet {}
	}

	function addParam(typeId, id, name, description, parameter, path){
		if (!paramsSet){
			return false
		}

		let nodeObject = JSON.parse(paramsSet.toJson())
		if (path){
			if (!("paramIds" in nodeObject) || !("parameters" in nodeObject)){
				console.error("Unable to add param to params set. Path is invalid")
				return false
			}

			for (let i = 0; i < path.length; i++){
				let paramId = path[i]
				let index = nodeObject.paramIds.indexOf(paramId)
				if (index >= 0){
					let json = nodeObject.parameters[index]
					
					nodeObject = JSON.parse(json)
				}
				else{
					console.error("Unable to add param to params set. Path is invalid")
					return false
				}
			}
		}

		nodeObject.paramTypeIds.push(typeId)
		nodeObject.paramIds.push(id)
		nodeObject.paramNames.push(name)
		nodeObject.paramDescriptions.push(description)
		nodeObject.parameters.push(parameter)

		paramsSet.createFromJson(JSON.stringify(nodeObject))
		
		return true
	}
	
	function clearParamsSet(){
		if (!paramsSet){
			return false
		}
		
		paramsSet.m_paramIds = []
		paramsSet.m_paramNames = []
		paramsSet.m_paramDescriptions = []
		paramsSet.m_parameters = []
		paramsSet.m_paramTypeIds = []
		
		return true
	}
	
	function getParameterById(id){
		let index = paramsSet.m_paramIds.indexOf(id)
		if (index < 0){
			return ""
		}
		
		return paramsSet.m_parameters[index]
	}

	function getParamJsonByPath(path){
		let nodeObject = JSON.parse(paramsSet.toJson())
		
		for (let i = 0; i < path.length; i++){
			let paramId = path[i]
			
			let index = nodeObject.paramIds.indexOf(paramId)
			if (index >= 0){
				let parameter = nodeObject.parameters[index]
				nodeObject = JSON.parse(parameter)
			}
		}

		return JSON.stringify(nodeObject)
	}
}

