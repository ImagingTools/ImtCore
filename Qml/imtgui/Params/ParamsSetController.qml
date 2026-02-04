import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtbaseImtBaseTypesSdl 1.0

ParamController {
	id: root
	
	property ParamsSet paramsSet: paramModel
	paramFactoryComp: Component {
		ParamsSet {}
	}

	property Component parameterComp: Component{
		Parameter {}
	}

	function addParam(typeId, id, name, description, parameterData){
		if (!paramsSet){
			return false
		}

		let parameterObj = parameterComp.createObject()
		parameterObj.m_typeId = typeId
		parameterObj.m_id = id
		parameterObj.m_name = name
		parameterObj.m_description = description
		parameterObj.m_data = parameterData

		if (!paramsSet.hasParameters()){
			paramsSet.emplaceParameters()
		}

		paramsSet.m_parameters.addElement(parameterObj)

		return true
	}

	function clearParamsSet(){
		if (!paramsSet){
			return false
		}

		if (paramsSet.hasParameters()){
			paramsSet.m_parameters.clear()
		}

		return true
	}
	
	// Return <Parameter> object from params set by ID
	function getParameterById(id){
		if (!paramsSet){
			return null
		}

		if (!paramsSet.hasParameters()){
			return null
		}

		let index = -1
		for (let j = 0; j < paramsSet.m_parameters.count; ++j){
			let parameterInfo = paramsSet.m_parameters.get(j).item
			if (parameterInfo.m_id === id){
				index = j
				break
			}
		}

		if (index < 0){
			return null
		}

		return paramsSet.m_parameters.get(index).item
	}
}

