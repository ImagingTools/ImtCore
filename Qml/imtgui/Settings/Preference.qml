import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtbaseSettingsSdl 1.0
import imtbaseImtBaseTypesSdl 1.0

Item {
	id: root;
	
	property ParamsSet paramsSet: ParamsSet {}
	property SettingsController settingsController: SettingsController {}
	
	signal editorModelDataChanged(string paramId, string key);
	
	Rectangle {
		id: mainPanelBackground;
		
		anchors.top: parent.top;
		anchors.left: parent.left;
		anchors.bottom: parent.bottom;
		anchors.margins: Style.sizeMainMargin;
		
		width: 200;
		height: parent.height;
		
		color: Style.baseColor;
		
		Column {
			id: mainPanel;
			
			anchors.fill: parent;
			anchors.topMargin: Style.sizeMainMargin;
			
			spacing: Style.sizeMainMargin;
			
			ListView {
				id: mainPanelRepeater;
				
				anchors.left: parent.left;
				anchors.leftMargin: Style.sizeMainMargin;
				anchors.right: parent.right;
				anchors.rightMargin: Style.sizeMainMargin;
				
				height: contentHeight;
				
				spacing: Style.sizeMainMargin;
				model: root.paramsSet.m_paramIds;
				
				boundsBehavior: Flickable.StopAtBounds;
				
				currentIndex: 0
				
				delegate: ItemDelegate {
					highlighted: mainPanelRepeater.currentIndex == model.index;
					text: root.paramsSet.m_paramNames[model.index]
					
					onClicked: {
						mainPanelRepeater.currentIndex = model.index;
					}
				}
			}
		}
	}
	
	Repeater {
		id: repeater;
		anchors.left: mainPanelBackground.right
		anchors.right: root.right
		height: root.height
		model: root.paramsSet.m_paramIds;
		
		delegate: Item {
			anchors.fill: repeater
			visible: mainPanelRepeater.currentIndex == model.index
			CustomScrollbar {
				id: scrollbar;
				z: 100;
				anchors.right: parent.right;
				height: root.height
				secondSize: Style.sizeMainMargin;
				targetItem: flickable;
			}
			
			Flickable {
				id: flickable;
				anchors.fill: parent
				anchors.margins: Style.sizeLargeMargin
				clip: true;
				contentWidth: width;
				contentHeight: bodyPanel.height;
				boundsBehavior: Flickable.StopAtBounds;
				
				ComposedParamsGui {
					id: bodyPanel;
					width: parent.width;
					settingsController: root.settingsController
					paramId: root.paramsSet.m_paramIds[model.index]
					typeId: root.paramsSet.m_paramTypeIds[model.index]
					name: root.paramsSet.m_paramNames[model.index]
					description: root.paramsSet.m_paramDescriptions[model.index]
					
					onEditorModelDataChanged: {
						if (mainPanelRepeater.currentIndex >= 0){
							let json = bodyPanel.paramsSet.toJson()
							root.paramsSet.m_parameters[mainPanelRepeater.currentIndex] = json
						}
						
						root.editorModelDataChanged(paramId, key)
					}
					
					Component.onCompleted: {
						if (paramController){
							let json = root.paramsSet.m_parameters[model.index]
							if (!paramController.createParamFromJson(json)){
								console.error("Unable to create params set from JSON")
							}
						}
					}
				}
			}
		}//Flickable
	}
}
