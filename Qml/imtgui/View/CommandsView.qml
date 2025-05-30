import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtbaseCommandsSdl 1.0

// Commands with priority = -1 always hidden
Item {
	id: commandsItem;
	width: contentWidth;
	height: Style.controlHeightM;
	
	property alias contentWidth: content_.width;
	property int maximumWidth: -1;
	
	// Count of the command groups in this view
	property alias countGroups: repeater.count;
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
	
	property int commandsCount: 0;
	
	// params["x"] = X coord click
	// params["y"] = Y coord click
	// params["target"] = button reference
	signal commandActivated(string commandId, var params);
	
	function setCommandData(commandId, key, value){
		if (!commandsModel){
			return;
		}

		for (let j = 0; j < allElements.length; j++){
			if (allElements[j].modelData["m_elementId"] === commandId){
				allElements[j].modelData[key] = value;
				
				break;
			}
		}
	}
	
	function getCommandData(commandId, key){
		for (let j = 0; j < allElements.length; j++){
			if (allElements[j]){
				if (allElements[j].modelData["m_elementId"] === commandId){
					return allElements[j].modelData[key];
				}
			}
		}
		
		return null;
	}
	
	function addCommandGroup(commands){
		if (!commandsModel || !commandsModel.m_subElements){
			return;
		}

		let subElements = commandsModel.m_subElements
		
		subElements.addElement(commands)
		
		repeater.model = subElements;
	}
	
	// Clear view
	function clear(){
		commandsModel.m_subElements.clear()
		allElements = []
		priorityElements = []
	}
	
	function reset(){
		for (let j = 0; j < allElements.length; j++){
			if (allElements[j]){
				allElements[j].modelData.m_visible = false;
			}
		}
	}
	
	function hasHiddenCommands(){
		for (let j = 0; j < allElements.length; j++){
			if (!allElements[j].visible){
				return true;
			}
		}
		
		return false;
	}
	
	function isEmpty(){
		return commandsModel.m_subElements.count === 0;
	}
	
	// The function checks whether the commands fit into the maximum width of this view
	// If they do not fit, then we hide the command with the lowest priority
	function checkWidth(){
		let maxWidth = maximumWidth;
		if (maxWidth < 0){
			maxWidth = 0;
		}
		
		reset();
		
		let visibleCount = 0;
		let tempWidth = 0;
		for (let j = 0; j < priorityElements.length; j++){
			let elementWidth = priorityElements[j].maxWidth;
			tempWidth += elementWidth;
			if (tempWidth + visibleCount * Style.marginM < maxWidth){
				priorityElements[j].modelData.m_visible = true;
				visibleCount++;
			}
			
			if (tempWidth >= maxWidth){
				continue;
			}
		}
	}
	
	Row {
		id: content_;
		height: commandsItem.height;
		spacing: Style.marginM;
		
		Repeater {
			id: repeater;
			
			delegate: Component {
				Item {
					id: itemDelegate;
					width: listView.width;
					height: commandsItem.height;
					property var dataModel: model.item;
					property int priority: model.item["m_priority"] ? model.item["m_priority"]: -1;
					property int commandsCount: repeater2.count;
					
					Row {
						id: listView;
						height: commandsItem.height;
						spacing: Style.marginM;
						
						Repeater {
							id: repeater2;
							model: itemDelegate.dataModel.m_subElements;
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
									property int groupPriority: itemDelegate.priority;
									property var modelData: model.item;
									property var subElements: repeater2.model;
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
								if (!item || !item.modelData){
									return;
								}
								
								if (!commandsItem.allElements.includes(item)){
									commandsItem.allElements.push(item);
								}
								
								if (!commandsItem.priorityElements.includes(item)){
									commandsItem.priorityElements.push(item);
								}
								
								commandsItem.priorityElements.sort(function(item1, item2){
									if (item1.groupPriority > item2.groupPriority){
										return -1;
									}
									if (item1.groupPriority < item2.groupPriority){
										return 1;
									}
									if (item1.priority > item2.priority){
										return -1;
									}
									if (item1.priority < item2.priority){
										return 1;
									}
									
									return 0;
								});
								
								commandsItem.commandsCount++;
							}
						}
					}
				}
			}
		}
	} // Main row
}

