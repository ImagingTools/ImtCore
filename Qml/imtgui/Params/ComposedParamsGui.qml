import QtQuick 2.0
import Acf 1.0
import com.imtcore.imtqml 1.0
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
	property int spacing: Style.marginXL

	sourceComp: Component {
		Column {
			id: column
			width: contentWidth;
			spacing: composedParamsGui.spacing
			
			property int contentWidth: Style.sizeHintXXL
			
			property int rootWidth: composedParamsGui.width
			onRootWidthChanged: {
				checkWidth()
			}
			
			function checkWidth(){
				let newWidth = Math.min(rootWidth, contentWidth);
				if (width !== newWidth){
					width = newWidth;
				}
			}
			
			Repeater {
				id: bodyPanelRepeater
				model: composedParamsGui.paramsSet ? composedParamsGui.paramsSet.m_paramTypeIds : 0
				
				delegate: Column {
					id: paramItem
					
					width: column.width;
					
					property string paramId: composedParamsGui.paramsSet.m_paramIds[model.index]
					property string paramTypeId: composedParamsGui.paramsSet.m_paramTypeIds[model.index]
					property string paramName: composedParamsGui.paramsSet.m_paramNames[model.index]
					property string paramDescription: composedParamsGui.paramsSet.m_paramDescriptions[model.index]
					property string parameterJson: composedParamsGui.paramsSet.m_parameters[model.index]
					
					Component.onCompleted: {
						if (composedParamsGui.settingsController && paramTypeId in composedParamsGui.settingsController.supportedParamEditors){
							let paramEditorComp = composedParamsGui.settingsController.supportedParamEditors[paramTypeId]
							
							if (paramTypeId == ParamTypeIdsTypeMetaInfo.s_paramsSet){
								groupLoader.sourceComponent = paramEditorComp
							}
							else{
								elementLoader.sourceComponent = paramEditorComp
							}
						}
						else{
							console.error("Param editor with type-ID '", paramTypeId, "' unregistered")
						}
					}
					
					Loader {
						id: elementLoader
						onLoaded: {
							paramItem.itemOnLoaded(item)
						}
					}
					
					Column {
						id: groupColumn
						width: parent.width
						visible: false
						spacing: composedParamsGui.spacing
						clip: true
						
						GroupHeaderView {
							id: headerView
							width: parent.width
							groupView: groupElementView
							title: paramItem.paramName
						}
						
						GroupElementView {
							id: groupElementView
							width: parent.width
							Column {
								Loader {
									id: groupLoader
									onLoaded: {
										item.spacing = 0
										item.settingsController = composedParamsGui.settingsController
										
										paramItem.itemOnLoaded(item)

										groupColumn.visible = true
									}
								}
							}
						}
					}
					
					// Width from paramSet to params
					Connections {
						target: paramItem
						
						function onWidthChanged(){
							if (elementLoader.item){
								if (elementLoader.item.border){
									elementLoader.item.border.width = 0
								}

								elementLoader.item.width = paramItem.width
							}
							
							if (groupLoader.item){
								if (groupLoader.item.border){
									groupLoader.item.border.width = 0
								}
								
								groupLoader.item.width = paramItem.width
							}
						}
					}
					
					Connections {
						id: itemConnections
						target: paramItem.paramTypeId == ParamTypeIdsTypeMetaInfo.s_paramsSet ? groupLoader.item : elementLoader.item

						function onEditorModelDataChanged(paramId, key){
							let json = target.editorModel.toJson();
							
							composedParamsGui.paramsSet.m_parameters[model.index] = json
							composedParamsGui.editorModelDataChanged(composedParamsGui.paramId + "/" + paramId, key)
						}
					}
					
					function itemOnLoaded(item){
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

						if (item.border){
							item.border.width = 0
						}

						item.width = paramItem.width - 2 * groupElementView.border.width
					}
				}
			}
		}//Column
	}
}

