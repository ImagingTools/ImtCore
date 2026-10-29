import QtQuick 2.0
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
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
	// Set by the parent when this set is shown inside a group frame
	property bool grouped: false
	property int spacing: grouped ? 0 : Style.marginXL

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
				let newWidth = Math.min(rootWidth, contentWidth)
				if (width !== newWidth){
					width = newWidth
				}
			}
			
			Repeater {
				id: bodyPanelRepeater
				model: composedParamsGui.paramsSet ? composedParamsGui.paramsSet.m_parameters : 0
				
				delegate: Column {
					id: paramItem
					
					width: column.width;
					
					property var parameter: model.item
					property string paramId: parameter.m_id
					property string paramTypeId: parameter.m_typeId
					property string paramName: parameter.m_name
					property string paramDescription: parameter.m_description
					property string parameterJson: parameter.m_data
					
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
					
					Rectangle {
						width: paramItem.width
						height: 1
						visible: composedParamsGui.grouped && model.index > 0
						opacity: 0.5
						color: Style.borderColor
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
										item.grouped = true
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
							paramItem.updateItemWidth()
						}
					}
					
					function updateItemWidth(){
						if (elementLoader.item){
							elementLoader.item.width = paramItem.width
						}
						
						// stay inside the group frame so its border is not painted over
						if (groupLoader.item){
							groupLoader.item.width = paramItem.width - 2 * groupElementView.border.width
						}
					}
					
					Connections {
						id: itemConnections
						target: paramItem.paramTypeId == ParamTypeIdsTypeMetaInfo.s_paramsSet ? groupLoader.item : elementLoader.item

						function onEditorModelDataChanged(paramId, key){
							let json = target.editorModel.toJson();
							composedParamsGui.paramsSet.m_parameters.get(model.index).item.m_data = json
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

						// inside a group the frame belongs to the group, not to each row
						if (composedParamsGui.grouped && item.sourceItem && item.sourceItem.border){
							item.sourceItem.border.width = 0
							item.sourceItem.radius = 0
						}

						updateItemWidth()
					}
				}
			}
		}//Column
	}
}

