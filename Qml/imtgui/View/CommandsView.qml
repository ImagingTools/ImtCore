import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtbaseCommandsSdl 1.0

// !!! Commands with priority = -1 always hidden
Item {
	id: commandsItem;
	width: contentWidth;
	height: Style.controlHeightM;
	
	property alias contentWidth: content_.width;
	property int maximumWidth: -1;
	
	property GuiElementModel commandsModel: GuiElementModel {};
	
	// Reference on the all elements in order of visibility to this view
	property var allElements: []
	
	// Reference on the all elements in order of priority to this view
	property var priorityElements: []
	
	// Commands with negative accent
	property var negativeAccentCommandIds: []
	
	// Commands with positive accent
	property var positiveAccentCommandIds: []
	
	property string positiveAccentColor: Style.imaginToolsAccentColor;
	property string negativeAccentColor: Style.errorTextColor;
	
	property alias commandsCount: repeater.count;
	
	// params["x"] = X coord click
	// params["y"] = Y coord click
	// params["target"] = button reference
	signal commandActivated(string commandId, var params);
	
	onCommandsModelChanged: {
		if (commandsModel && commandsModel.m_subElements){
			repeater.model = commandsModel.m_subElements;
		}
	}

	// Clear view
	function clear(){
		if (commandsModel.hasSubElements()){
			commandsModel.m_subElements.clear()
		}
		allElements = []
		priorityElements = []
	}
	
	function hasHiddenCommands(){
		for (let j = 0; j < allElements.length; j++){
			if (!allElements[j] || !allElements[j].element){
				continue;
			}

			if (!allElements[j].element["m_visible"]){
				return true;
			}
		}
		
		return false;
	}
	
	// The function checks whether the commands fit into the maximum width of this view
	// If they do not fit, then we hide the command with the lowest priority
	function checkWidth(){
		let maxWidth = Math.max(0, maximumWidth);
		let totalWidth = 0;
		let spacing = content_.spacing;

		for (let i = 0; i < priorityElements.length; i++) {
			let item = priorityElements[i];
			if (item.element) {
				item.element.m_visible = false;
			}
		}

		for (let i = 0; i < priorityElements.length; i++) {
			let item = priorityElements[i];
			if (!item.element || item.priority < 0)
				continue;
			
			let elementWidth = item.maxWidth;
			
			let projectedWidth = totalWidth > 0 ? totalWidth + spacing + elementWidth : elementWidth;
			
			if (projectedWidth <= maxWidth) {
				item.element.m_visible = true;
				totalWidth = projectedWidth;
			} else {
				break;
			}
		}
	}
	
	Row {
		id: content_;
		height: commandsItem.height;
		spacing: Style.marginM;
		
		Repeater {
			id: repeater;
			delegate: Component { Button {
					id: button;
					
					Connections {
						target: model.item
						function onModelChanged(){
							button.checked = model.item.m_toggled
						}
					}

					property GuiElementModel element: model.item;
					
					enabled: element ? element.m_enabled: false;

					text: element ? element.m_elementName : "";
					widthFromDecorator: true;
					heightFromDecorator: true;
					checkable: element ? element.m_toggleable : false;
					checked: element ? element.m_toggled : false;
					iconSource: !element ? "" : element.m_elementItemPath === "" ? "" : element.m_enabled ? "../../../../" + Style.getIconPath(element.m_elementItemPath, Icon.State.On, Icon.Mode.Normal) :
																						"../../../../" + Style.getIconPath(element.m_elementItemPath, Icon.State.Off, Icon.Mode.Disabled);
					decorator: Component {
						TopButtonDecorator {
							property string baseColor: !baseElement ? "transparent" : baseElement.checked ? Style.borderColor : baseElement.mouseArea.containsMouse ? Style.baseColor : Style.backgroundColor2
							property string baseTextColor: !baseElement ? "transparent" : baseElement.enabled ? Style.textColor : Style.inactiveTextColor;
							color: button.isPositiveAccent ? commandsItem.positiveAccentColor :
															 button.isNegativeAccent ? commandsItem.negativeAccentColor : baseColor;
							textColor: button.isPositiveAccent || button.isNegativeAccent ? "white" : baseTextColor;
							icon.source: button.isPositiveAccent || button.isNegativeAccent ?
											 "../../../../" + Style.getIconPath(button.element.m_elementItemPath, Icon.State.Off, Icon.Mode.Disabled)
										   : baseElement.iconSource;
							border.width: button.isPositiveAccent || button.isNegativeAccent ? 0 : baseElement.mouseArea.containsMouse;
							opacity: button.hovered && (button.isPositiveAccent || button.isNegativeAccent) ? 0.85 : 1;
						}
					}
					
					property bool isNegativeAccent: element && element.m_enabled ? commandsItem.negativeAccentCommandIds.includes(element.m_elementId) : false;
					property bool isPositiveAccent: element && element.m_enabled ? commandsItem.positiveAccentCommandIds.includes(element.m_elementId) : false;
					property int priority: element ? element.m_priority : 0;
					property int maxWidth: -1;
					
					visible: !element || priority < 0 ? false : element.m_visible;
					
					onClicked: {
						let params = {}
						params["x"] = mouseArea.mouseX
						params["y"] = mouseArea.mouseY
						params["target"] = button
						commandsItem.commandActivated(element.m_elementId, params);
					}
					
					onWidthChanged: {
						maxWidth = Math.max(maxWidth, width);
					}
				}
			}
			
			onItemAdded: {
				if (!item || !item.element){
					return;
				}
				
				if (!commandsItem.allElements.includes(item)){
					commandsItem.allElements.push(item);
				}
				
				if (!commandsItem.priorityElements.includes(item)){
					commandsItem.priorityElements.push(item);
				}
				
				commandsItem.priorityElements.sort(function(item1, item2){
					if (item1.priority > item2.priority){
						return -1;
					}
					if (item1.priority < item2.priority){
						return 1;
					}
					
					return 0;
				});
			}
		}
	}
}

