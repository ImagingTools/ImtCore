import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtbaseCommandsSdl 1.0

QtObject {
	id: root;

	property string typeId;
	property Item view: null;
	property GuiElementContainer commands: GuiElementContainer {}

	property var pendingChanges: ({}) // { commandId: { m_enabled: true, m_visible: false, ... } }

	signal commandsReceived(string typeId, var commands)
	signal commandsReceiveFailed()

	onCommandsReceived: {
		applyPendingChanges()
	}

	function applyPendingChanges(){
		for (let commandId in pendingChanges) {
			let command = getCommandById(commandId)
			if (!command)
				continue
			
			let changes = pendingChanges[commandId]
			for (let key in changes) {
				if (command.hasOwnProperty(key)) {
					command[key] = changes[key]
				}
			}
		}
		pendingChanges = {}
	}

	function setCommandProperty(commandId, propertyName, value){
		let command = getCommandById(commandId)
		if (command){
			command[propertyName] = value
		}
		else{
			if (!pendingChanges[commandId])
				pendingChanges[commandId] = {}
			
			pendingChanges[commandId][propertyName] = value
		}
	}

	function getCommandProperty(commandId, propertyName){
		let command = getCommandById(commandId)
		if (command){
			return command[propertyName]
		}

		return null
	}

	function getCommandIds(){
		let commandIds = []

		getCommandIdsRecursive(commands.m_elements, commandIds)
		
		return commandIds
	}
	
	function getCommandIdsRecursive(commands, retVal){
		if (!commands){
			return
		}
		for (let i = 0; i < commands.count; i++){
			let element = commands.get(i).item;
			if (element){
				let elementId = element.m_elementId
				let subElements = element.m_subElements
				
				if (subElements.count == 0){
					retVal.push(elementId)
				}
				else{
					getCommandIdsRecursive(subElements, retVal);
				}
			}
		}
	}
	
	function getCommandByIndex(index){
		if (index < 0 || !commands){
			return null;
		}

		return commands.m_elements.get(index).item;
	}
	
	function findCommandId(elements, commandId){
		if (!elements){
			return null
		}

		for (let i = 0; i < elements.count; i++){
			let element = elements.get(i).item;
			if (element){
				if (element.m_elementId == commandId){
					return element;
				}
				
				if (element.m_subElements.count > 0){
					let retVal = findCommandId(element.m_subElements, commandId)
					if (retVal){
						return retVal;
					}
				}
			}
		}
		
		return null;
	}
	
	function getCommandById(commandId){
		if (!commands){
			return null;
		}

		return findCommandId(commands.m_elements, commandId);
	}
	
	function commandIsEnabled(commandId){
		let command = getCommandById(commandId);
		if (!command){
			return false;
		}
		
		return command.m_enabled;
	}
	
	function getCommands(){
		commandsReceived(typeId, commands);
	}
	
	function setCommandIsEnabled(commandId, isEnabled){
		setCommandProperty(commandId, "m_enabled", isEnabled)
	}
	
	function setCommandIcon(commandId, icon){
		setCommandProperty(commandId, "m_elementItemPath", icon)
	}
	
	function setCommandName(commandId, commandName){
		setCommandProperty(commandId, "m_elementName", commandName)
	}
	
	function setCommandNotification(commandId, notification){
		setCommandProperty(commandId, "m_elementStatus", notification)
	}
	
	function setCommandVisible(commandId, isVisible){
		setCommandProperty(commandId, "m_visible", isVisible)
		
	}
	
	function getCommandIndex(commandId){
		if(!commands){
			return -1;
		}
		
		for (let i = 0; i < commands.m_elements.count; i++){
			let command = commands.m_elements.get(i).item;
			if (command && command.m_elementId == commandId){
				return i;
			}
		}
		
		return -1;
	}
	
	function getCommandsGroup(groupId){
		if (!commands){
			return null;
		}
		
		for (let i = 0; i < commands.m_elements.count; i++){
			let group = commands.m_elements.get(i).item;
			if (group && group.m_elementId == groupId){
				return group.m_subElements;
			}
		}
		
		return null;
	}
	
	function getLeftCommandsGroup(){
		return getCommandsGroup("Group1");
	}
	
	function getCenterCommandsGroup(){
		return getCommandsGroup("Group2");
	}
	
	function getRightCommandsGroup(){
		return getCommandsGroup("Group3");
	}
	
	function getAdditionalCommandsGroup(){
		return getCommandsGroup("Group4");
	}
	
	function setIsToggleable(commandId, toggleable){
		setCommandProperty(commandId, "m_toggleable", toggleable)
	}
	
	function isToggleable(commandId){
		let command = getCommandById(commandId);
		if (command){
			return command.m_toggleable;
		}
		
		return false;
	}
	
	function isToggled(commandId){
		let command = getCommandById(commandId);
		if (command){
			return command.m_toggled;
		}
		
		return false;
	}
	
	function setToggled(commandId, toggled){
		setCommandProperty(commandId, "m_toggled", toggled)
	}
	
	function commandExists(commandId){
		let command = getCommandById(commandId)
		if (!command){
			return false;
		}
		return true;
	}
}

