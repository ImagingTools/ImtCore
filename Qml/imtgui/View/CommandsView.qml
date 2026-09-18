import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtbaseCommandsSdl 1.0

// !!! Commands with priority = -1 always hidden
Item {
	id: commandsItem;
	objectName: "CommandsView"
	width: contentWidth;
	height: Style.controlHeightM;
	
	property alias contentWidth: content_.width;
	property int maximumWidth: -1;
	
	property GuiElementModel commandsModel: GuiElementModel {};
	
	// Reference on the all elements in order of visibility to this view
	property var allElements: []
	
	// Reference on the all elements in order of priority to this view
	property var priorityElements: []

	property alias commandsCount: repeater.count;

	// params["x"] = X coord click
	// params["y"] = Y coord click
	// params["target"] = button reference
	signal commandActivated(string commandId, var params);
	signal updateWidth()

	onCommandsModelChanged: {
		if (commandsModel && commandsModel.m_subElements){
			repeater.model = commandsModel.m_subElements;
		}
	}

	QtObject {
		id: internal
		property bool layoutReady: false
		property var initializedButtons: []

		function checkNeedUpdateWidth(){
			if (internal.initializedButtons.length === repeater.count){
				internal.layoutReady = true
				commandsItem.updateWidth()
			}
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

	function calcVisibleCommandsCount(){
		let retVal = 0

		for (let j = 0; j < allElements.length; j++){
			if (!allElements[j] || !allElements[j].element){
				continue
			}

			if (allElements[j].element.m_visible){
				retVal++
			}
		}

		return retVal
	}

	function hasHiddenCommands(){
		for (let j = 0; j < allElements.length; j++){
			if (!allElements[j] || !allElements[j].element){
				continue
			}

			if (allElements[j].hidden && allElements[j].element.m_visible){
				return true
			}
		}
		
		return false;
	}

	// The function checks whether the commands fit into the maximum width of this view
	// If they do not fit, then we hide the command with the lowest priority
	function checkWidth(){
		if (!internal.layoutReady || commandsCount === 0){
			return
		}

		let maxWidth = Math.max(0, maximumWidth);
		let totalWidth = 0;
		let spacing = 1.3*content_.spacing;

		for (let i = 0; i < priorityElements.length; i++) {
			let item = priorityElements[i];
			if (item.element){
				item.hidden = true
			}
		}

		for (let i = 0; i < priorityElements.length; i++) {
			let item = priorityElements[i];
			if (!item.element || item.priority < 0 || !item.element.m_visible)
				continue;

			let elementWidth = item.maxWidth;
			
			let projectedWidth = totalWidth > 0 ? totalWidth + spacing + elementWidth : elementWidth;
			
			if (projectedWidth <= maxWidth) {
				item.hidden = false
				totalWidth = projectedWidth;
			}
			else {
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
			onCountChanged: {
				internal.checkNeedUpdateWidth()
			}

			delegate: Component { MenuButton {
					id: button;
					objectName: model.item ? model.item.m_elementId + "Button" : "Button"

					isMenuButton: element.m_subElements.count
					mainClickByModel : false

					menuModel: isMenuButton ? element.m_subElements : 0

					displayId: "m_elementName"
					elementIdName: "m_elementId"

					Connections {
						target: model.item
						function onModelChanged(){
							button.checkHidden()
							commandsItem.checkWidth()
						}
					}

					property bool hidden: false
					onHiddenChanged: {
						checkHidden()
					}

					function checkHidden(){
						if (hidden){
							button.visible = false
						}
						else{
							button.visible = model.item.m_visible
						}
					}

					property GuiElementModel element: model.item;
					
					enabled: element ? element.m_enabled: false;

					text: element ? element.m_elementName : "";
					widthFromDecorator: true;
					heightFromDecorator: true;
					checkable: false
					checked: element ? element.m_toggled : false

					iconSource: !element ? "" : element.m_elementItemPath === "" ? "" : element.m_enabled ? "../../../../" + Style.getIconPath(element.m_elementItemPath, Icon.State.On, Icon.Mode.Normal) :
																						"../../../../" + Style.getIconPath(element.m_elementItemPath, Icon.State.Off, Icon.Mode.Disabled);
					decorator: Component {
						TopButtonDecorator {
							property string baseColor: !baseElement ? "transparent" : baseElement.checked ? Style.selectedColor : baseElement.mouseArea.containsMouse ? Style.alternateBaseColor : Style.baseColor
							property string baseTextColor: !baseElement ? "transparent" : baseElement.enabled ? Style.textColor : Style.inactiveTextColor;
							color: baseColor;
							textColor: baseTextColor;
							icon.source: baseElement.iconSource;
							arrowIconAlias.source: "../../../" + Style.getIconPath("Icons/Down", Icon.State.On, Icon.Mode.Normal);
							border.width: baseElement.mouseArea.containsMouse;
						}
					}

					property int priority: element ? element.m_priority : 0;
					property int maxWidth: -1;
					
					visible: !hidden //? false : element && element.m_visible

					onPriorityChanged: {
						hidden = priority === -1
					}

					onAccepted: {
						let params = {}
						params["x"] = mouseArea.mouseX
						params["y"] = mouseArea.mouseY
						params["target"] = button

						let id
						if(buttonId == ""){
							id = element.m_elementId
						}
						else {
							id = buttonId
						}
						commandsItem.commandActivated(id, params);
					}

					onWidthChanged: {
						maxWidth = Math.max(maxWidth, width);
						if (maxWidth > 0 && element){
							if (!internal.initializedButtons.includes(element.m_elementId)){
								internal.initializedButtons.push(element.m_elementId)
								internal.checkNeedUpdateWidth()
							}
						}
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

			onItemRemoved: {
				if (!item || !item.element){
					return;
				}

				let removedIndex = commandsItem.allElements.indexOf(item)
				if (removedIndex >= 0){
					commandsItem.allElements.splice(removedIndex, 1)
				}

				removedIndex = commandsItem.priorityElements.indexOf(item)
				if (removedIndex >= 0){
					commandsItem.priorityElements.splice(removedIndex, 1)
				}
			}
		}
	}
}

