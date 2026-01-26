import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtbaseImtBaseTypesSdl 1.0

QtObject {
	id: root

	property ParamsSet paramsSet: ParamsSet {}
	property ParamsSetController paramsSetController: ParamsSetController {
		paramModel: root.paramsSet
	}
	
	property var supportedParamEditors: ({})
	property var registeredControllers: ({})
	property bool needsRepresentationUpdate: true
	
	property Component textParamEditorComp: Component { TextParamEditor {} }
	property Component textViewParamEditorComp: Component { TextParamView {} }
	property Component urlParamEditorComp: Component { UrlParamEditor {} }
	property Component databaseAccessSettingsEditorComp: Component { DatabaseAccessSettingsEditor {} }
	property Component fileNameParamEditorComp: Component { FileNameParamEditor {}}
	property Component schedulerParamEditorComp: Component { SchedulerParamEditor {}}
	property Component selectionParamEditorComp: Component { SelectionParamEditor {}}
	property Component paramsSetEditorComp: Component {ComposedParamsGui {}}
	property Component passwordParamEditorComp: Component {PasswordParamEditor {}}
	property Component integerParamEditorComp: Component { IntegerParamEditor {}}
	property Component doubleParamEditorComp: Component { DoubleParamEditor {}}
	property Component constrainedDoubleParamEditorComp: Component { ConstrainedDoubleParamEditor{}}
	property Component enableableParamEditorComp: Component { EnableableParamEditor {}}
	property Component serverConnectionParamEditorComp: Component { ServerConnectionParamEditor {}}
	
	Component.onCompleted: {
		registerParamEditor(ParamTypeIdsTypeMetaInfo.s_textParam, textParamEditorComp)
		registerParamEditor(ParamTypeIdsTypeMetaInfo.s_idParam, textParamEditorComp)
		registerParamEditor(ParamTypeIdsTypeMetaInfo.s_urlParam, urlParamEditorComp)
		registerParamEditor(ParamTypeIdsTypeMetaInfo.s_databaseAccessSettings, databaseAccessSettingsEditorComp)
		registerParamEditor(ParamTypeIdsTypeMetaInfo.s_paramsSet, paramsSetEditorComp)
		registerParamEditor(ParamTypeIdsTypeMetaInfo.s_fileNameParam, fileNameParamEditorComp)
		registerParamEditor(ParamTypeIdsTypeMetaInfo.s_schedulerParam, schedulerParamEditorComp)
		registerParamEditor(ParamTypeIdsTypeMetaInfo.s_selectionParam, selectionParamEditorComp)
		registerParamEditor(ParamTypeIdsTypeMetaInfo.s_textView, textViewParamEditorComp)
		registerParamEditor(ParamTypeIdsTypeMetaInfo.s_passwordParam, passwordParamEditorComp)
		registerParamEditor(ParamTypeIdsTypeMetaInfo.s_integerParam, integerParamEditorComp)
		registerParamEditor(ParamTypeIdsTypeMetaInfo.s_doubleParam, doubleParamEditorComp)
		registerParamEditor(ParamTypeIdsTypeMetaInfo.s_constrainedDoubleParam, constrainedDoubleParamEditorComp)
		registerParamEditor(ParamTypeIdsTypeMetaInfo.s_enableableParam, enableableParamEditorComp)
		registerParamEditor(ParamTypeIdsTypeMetaInfo.s_serverConnectionParam, serverConnectionParamEditorComp)
	}
	
	function addParamsSet(id, name, paramsSet){
		if (!paramsSetController){
			return false
		}
		
		if (!paramsSet){
			return false
		}

		paramsSetController.addParam(ParamTypeIdsTypeMetaInfo.s_paramsSet, id, name, "", paramsSet.toJson())

		return true
	}
	
	function registerParamEditor(typeId, paramEditorComp){
		if (typeId in supportedParamEditors){
			return false
		}
		
		supportedParamEditors[typeId] = paramEditorComp
		
		return true
	}
	
	function getParamEditor(typeId){
		return supportedParamEditors[typeId]
	}
	
	function registerParamsSetController(id, name, controller){
		let paramsSetInfo = paramsSetInfoComp.createObject(root)
		
		paramsSetInfo.m_paramId = id
		paramsSetInfo.m_paramName = name
		paramsSetInfo.m_controller = controller
		
		registeredControllers[id] = paramsSetInfo
		needsRepresentationUpdate = true
	}
	
	property Component paramsSetInfoComp: Component {
		QtObject {
			property string m_paramId
			property string m_paramName
			property var m_controller
		}
	}
	
	function createRepresentation(){
		// Only rebuild if controllers changed
		if (!needsRepresentationUpdate){
			return paramsSet
		}
		
		paramsSetController.clearParamsSet()
		
		for (let key in registeredControllers){
			let paramsSetInfo = registeredControllers[key]
			if (paramsSetInfo){
				if (!addParamsSet(paramsSetInfo.m_paramId, paramsSetInfo.m_paramName, paramsSetInfo.m_controller.getParam())){
					console.error("Unable to add params set to settings: ", paramsSetInfo.m_paramId, paramsSetInfo.m_paramName)
				}
			}
		}
		
		needsRepresentationUpdate = false
		return paramsSet
	}
	
	function saveParam(paramId){
		if (registeredControllers[paramId]){
			let controller = registeredControllers[paramId].m_controller
			let parameter = paramsSetController.getParameterById(paramId)
			
			if (controller.createParamFromJson(parameter.m_data, true)){
				controller.saveParam()
			}
			else{
				console.error("Unable to create param from json. Param-ID: ", paramId)
				return false
			}
			
			return true
		}
		
		return false
	}
}

