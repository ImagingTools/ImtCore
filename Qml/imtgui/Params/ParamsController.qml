import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtbaseImtBaseTypesSdl 1.0

QtObject {
	id: root
	
	property var registeredParams: ({})
	property var registeredParamEditors: ({})
	
	property Component textParamComp: Component { TextParam {}}
	property Component idParamComp: Component { IdParam {}}
	property Component urlParamComp: Component { UrlParam {}}
	property Component databaseAccessSettingsParamComp: Component { DatabaseAccessSettings {}}
	property Component selectionParamComp: Component { SelectionParam {}}
	property Component schedulerParamComp: Component { SchedulerParam {}}
	property Component backupSettingsParamComp: Component { BackupSettings {}}
	property Component paramsSetComp: Component { ParamsSet {}}
	property Component fileNameParamComp: Component { FileNameParam {}}

	property Component textParamEditorComp: Component { TextParamEditor {} }
	property Component urlParamEditorComp: Component { UrlParamEditor {} }
	property Component databaseAccessSettingsEditorComp: Component { DatabaseAccessSettingsEditor {} }
	property Component paramsSetEditorComp: Component { ComposedParamsGui {} }
	property Component fileNameParamEditorComp: Component { FileNameParamEditor {}}
	property Component schedulerParamEditorComp: Component { SchedulerParamEditor {}}
	property Component selectionParamEditorComp: Component { SelectionParamEditor {}}
	
	Component.onCompleted: {
		registerBaseParams()
	}
	
	function registerBaseParams(){
		registerParam(ParamTypeIdsTypeMetaInfo.s_textParam, textParamComp, textParamEditorComp)
		registerParam(ParamTypeIdsTypeMetaInfo.s_idParam, textParamComp, textParamEditorComp)
		registerParam(ParamTypeIdsTypeMetaInfo.s_urlParam, urlParamComp, urlParamEditorComp)
		registerParam(ParamTypeIdsTypeMetaInfo.s_databaseAccessSettings, databaseAccessSettingsParamComp, databaseAccessSettingsEditorComp)
		registerParam(ParamTypeIdsTypeMetaInfo.s_paramsSet, paramsSetComp, paramsSetEditorComp)
		registerParam(ParamTypeIdsTypeMetaInfo.s_fileNameParam, fileNameParamComp, fileNameParamEditorComp)
		registerParam(ParamTypeIdsTypeMetaInfo.s_schedulerParam, schedulerParamComp, schedulerParamEditorComp)
		registerParam(ParamTypeIdsTypeMetaInfo.s_selectionParam, selectionParamComp, selectionParamEditorComp)
	}
	
	function joinParamToParamsSet(paramsSet){
		
	}
	
	function registerParam(typeId, paramComp, paramEditorComp){
		if (typeId in registeredParams){
			console.error("Parameter already registered")
			return false
		}
		
		registeredParams[typeId] = paramComp
		registeredParamEditors[typeId] = paramEditorComp
		
		return true
	}
	
	function createParam(typeId, json){
		if (!(typeId in registeredParams)){
			console.error("Unable to create parameter. Parameter with this type-ID unregistered")
			return null
		}
		
		if (!registeredParams[typeId]){
			console.error("Unable to create parameter. Registered parameter is invalid")
			return null
		}
		
		let object = registeredParams[typeId].createObject(root)
		if (!object){
			console.error("Unable to create parameter. Create object failed")
			return null
		}
		
		object.createFromJson(json)
		
		return object
	}
	
	function getParamComp(typeId){
		if (!(typeId in registeredParams)){
			return null
		}
		
		return registeredParams[typeId]
	}
	
	function getParamEditorComp(typeId){
		if (!(typeId in registeredParamEditors)){
			return null
		}
		
		return registeredParamEditors[typeId]
	}
	
	function createParamEditor(typeId){
		if (!(typeId in registeredParamEditors)){
			console.error("Unable to create parameter. Parameter with this type-ID unregistered")
			return null
		}
		
		if (!registeredParamEditors[typeId]){
			return null
		}
		
		return registeredParamEditors[typeId].createObject(root)
	}
}

