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
		anchors.margins: Style.marginM;
		
		width: Style.sizeHintXXS;
		height: parent.height;
		
		color: Style.baseColor;
		
		Column {
			id: mainPanel;
			
			anchors.fill: parent;
			anchors.topMargin: Style.marginM;
			
			spacing: Style.marginM;
			
			ListView {
				id: mainPanelRepeater;
				anchors.left: parent.left;
				anchors.leftMargin: Style.marginM;
				anchors.right: parent.right;
				anchors.rightMargin: Style.marginM;
				height: contentHeight;
				spacing: Style.marginM;
				model: root.paramsSet.m_parameters;
				boundsBehavior: Flickable.StopAtBounds;
				currentIndex: 0
				delegate: ItemDelegate {
					highlighted: mainPanelRepeater.currentIndex === model.index
					text: model.item.m_name
					onClicked: {
						mainPanelRepeater.currentIndex = model.index
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
		model: root.paramsSet.m_parameters
		
		delegate: Item {
			anchors.fill: repeater
			visible: mainPanelRepeater.currentIndex == model.index
			CustomScrollbar {
				id: scrollbar;
				z: 100;
				anchors.right: parent.right;
				height: root.height
				secondSize: Style.marginM;
				targetItem: flickable;
			}
			
			Flickable {
				id: flickable;
				anchors.fill: parent
				anchors.margins: Style.marginXL
				clip: true;
				contentWidth: width;
				contentHeight: bodyPanel.height;
				boundsBehavior: Flickable.StopAtBounds;
				
				ComposedParamsGui {
					id: bodyPanel;
					width: parent.width;
					settingsController: root.settingsController
					paramId: model.item.m_id
					typeId: model.item.m_typeId
					name: model.item.m_name
					description: model.item.m_description
					
					onEditorModelDataChanged: {
						if (mainPanelRepeater.currentIndex >= 0){
							let json = bodyPanel.paramsSet.toJson()
							root.paramsSet.m_parameters.get(mainPanelRepeater.currentIndex).item.m_data = json
							
							console.log("json", root.paramsSet.m_parameters.get(mainPanelRepeater.currentIndex).item.m_data)
						}

						root.editorModelDataChanged(paramId, key)
					}
					
					Component.onCompleted: {
						if (paramController){
							let json = root.paramsSet.m_parameters.get(model.index).item.m_data
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
