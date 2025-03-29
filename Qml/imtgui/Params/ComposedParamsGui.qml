import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtbaseImtBaseTypesSdl 1.0

ParamEditorBase {
	id: composedParamsGui
	
	typeId: ParamTypeIdsTypeMetaInfo.s_paramsSet
	property ParamsSet paramsSet: editorModel
	
	paramController: ParamsSetController {}

	editorModelComp: Component {
		ParamsSet {}
	}
	
	property var settingsController: null

	sourceComp: Component {
		Column {
			id: column
			width: composedParamsGui.width;
			spacing: Style.sizeLargeMargin
	
			property int contentWidth: 700

			Repeater {
				id: bodyPanelRepeater
				model: composedParamsGui.paramsSet ? composedParamsGui.paramsSet.m_paramTypeIds : 0
				delegate: Loader {
					id: loader
					
					property string paramId
					property string paramTypeId
					property string paramName
					property string paramDescription
					property string parameterJson
					
					Component.onCompleted: {
						paramId = composedParamsGui.paramsSet.m_paramIds[model.index]
						paramTypeId = composedParamsGui.paramsSet.m_paramTypeIds[model.index]
						paramName = composedParamsGui.paramsSet.m_paramNames[model.index]
						paramDescription = composedParamsGui.paramsSet.m_paramDescriptions[model.index]
						parameterJson = composedParamsGui.paramsSet.m_parameters[model.index]

						if (composedParamsGui.settingsController && paramTypeId in composedParamsGui.settingsController.supportedParamEditors){
							let paramEditorComp = composedParamsGui.settingsController.supportedParamEditors[paramTypeId]
							sourceComponent = paramEditorComp
						}
						else{
							console.error("Param editor with type-ID '", paramTypeId, "' unregistered")
						}
					}

					onLoaded: {
						if (item.paramId != undefined){
							item.paramId = paramId
						}
						
						if (item.name != undefined){
							item.name = paramName
						}
						
						if (item.description != undefined){
							item.description = paramDescription
						}
						
						if (item.paramController){
							if (!item.paramController.createParamFromJson(parameterJson)){
								console.error("Unable to create param from json. Param: ", paramId, paramName)
							}
						}

						checkWidth()
					}
					
					property int rootWidth: composedParamsGui.width
					onRootWidthChanged: {
						checkWidth()
					}

					function checkWidth() {
						if (item) {
							let newWidth = Math.min(rootWidth, column.contentWidth);
							
							if (item.width !== newWidth) {
								item.width = newWidth;
							}
						}
					}
					
					Connections {
						target: loader.item

						function onEditorModelDataChanged(paramId, key){
							let json = loader.item.editorModel.toJson();

							composedParamsGui.paramsSet.m_parameters[model.index] = json
							composedParamsGui.editorModelDataChanged(composedParamsGui.paramId + "/" + paramId, key)
						}
					}
				}
			}
		}//Column
	}
}

