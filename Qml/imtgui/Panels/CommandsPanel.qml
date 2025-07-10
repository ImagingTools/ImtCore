import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtbaseCommandsSdl 1.0

Item {
	id: commandsItem;
	height: Style.controlHeightM;

	signal commandActivated(string commandId, var params);
	
	function setNegativeAccentCommandIds(commandIds){
		leftCommands.negativeAccentCommandIds = commandIds;
		centerCommands.negativeAccentCommandIds = commandIds;
		rightCommands.negativeAccentCommandIds = commandIds;
	}
	
	function setPositiveAccentCommandIds(commandIds){
		leftCommands.positiveAccentCommandIds = commandIds;
		centerCommands.positiveAccentCommandIds = commandIds;
		rightCommands.positiveAccentCommandIds = commandIds;
	}
	
	property GuiElementContainer commandsModel: GuiElementContainer {};
	onCommandsModelChanged: {
		updateGui()
	}
	
	function updateGui(){
		if (!commandsModel){
			return;
		}

		clearModel();

		for (let i = 0; i < commandsModel.m_elements.count; i++){
			let group = commandsModel.m_elements.get(i).item;
			if (group){
				let alignment = group.m_alignment;
				if (alignment === 1){
					leftCommands.commandsModel = group
				}
				else if (alignment === 2){
					rightCommands.commandsModel = group
				}
				else if (alignment === 4){
					centerCommands.commandsModel = group
				}
			}
		}
	}
	
	function clearModel(){
		if (!leftCommands || !centerCommands || !rightCommands){
			return;
		}
		
		leftCommands.clear();
		centerCommands.clear();
		rightCommands.clear();
	}

	onVisibleChanged: {
		timer.restart();
	}
	
	onWidthChanged: {
		timer.restart();
	}
	
	Timer {
		id: timer;
		interval: 200;
		onTriggered: {
			if (commandsItem){
				commandsItem.checkCommandsWidth();
				commandsItem.checkButtonVisible();
			}
		}
	}

	function checkCommandsWidth(){
		if (leftCommands){
			leftCommands.checkWidth();
		}
		
		if (centerCommands){
			centerCommands.checkWidth();
		}
		
		if (rightCommands){
			rightCommands.checkWidth();
		}
	}
	
	function checkButtonVisible(){
		if (!button || !leftCommands || !centerCommands || !rightCommands){
			return;
		}

		button.visible = leftCommands.hasHiddenCommands() ||
				centerCommands.hasHiddenCommands() ||
				rightCommands.hasHiddenCommands();
	}
	
	CommandsView {
		id: leftCommands;
		anchors.left: parent.left;
		anchors.leftMargin: Style.marginM;
		maximumWidth: (centerCommands.commandsCount > 0 ?
						   centerCommands.x :
						   rightCommands.commandsCount > 0 ? rightCommands.x : commandsItem.width) - button.width - leftCommands.anchors.leftMargin;
		visible: !timer.running
		onCommandActivated: {
			commandsItem.commandActivated(commandId, params);
		}
		
		onCommandsCountChanged: {
			timer.restart();
		}
	}
	
	CommandsView {
		id: centerCommands;
		anchors.horizontalCenter: parent.horizontalCenter;
		maximumWidth: (rightCommands.commandsCount > 0 ?
						   rightCommands.x - centerCommands.x - (button.visible ? button.width : 0):
						   commandsItem.width - centerCommands.x) - button.width;
		visible: !timer.running
		onCommandActivated: {
			commandsItem.commandActivated(commandId, params);
		}
		
		onCommandsCountChanged: {
			timer.restart();
		}
	}
	
	CommandsView {
		id: rightCommands;
		anchors.right: button.left;
		anchors.rightMargin: Style.marginM;
		maximumWidth: (commandsItem.width - (centerCommands.x + centerCommands.contentWidth)) - button.width;
		visible: !timer.running
		onCommandActivated: {
			commandsItem.commandActivated(commandId, params);
		}
		
		onCommandsCountChanged: {
			timer.restart();
		}
	}
	
	ToolButton {
		id: button;
		anchors.verticalCenter: parent.verticalCenter;
		anchors.right: parent.right;
		width: visible ? Style.controlHeightM : 0;
		height: width;
		iconSource: "../../../" + Style.getIconPath("Icons/More", Icon.Mode.On, Icon.State.Active);
		visible: false;
		decorator: Component {
			ToolButtonDecorator {
				color: baseElement && baseElement.hovered  ? Style.baseColor : "transparent";
			}
		}
		
		onClicked: {
			if (!button || !leftCommands || !centerCommands || !rightCommands){
				return;
			}
			
			var point = button.mapToItem(null, 0, height);
			let empty = {
				"id": "",
				"name": "",
				"icon": "",
				"isEnabled": true
			};
			
			contextMenuModel.clear();
			if (leftCommands.hasHiddenCommands()){
				contextMenuModel.addModel(leftCommands.allElements);
			}
			
			if (centerCommands.hasHiddenCommands()){
				if (contextMenuModel.count > 0){
					contextMenuModel.insertNewItem(empty);
				}
				
				contextMenuModel.addModel(centerCommands.allElements);
			}
			
			if (rightCommands.hasHiddenCommands()){
				if (contextMenuModel.count > 0){
					contextMenuModel.insertNewItem(empty);
				}
				contextMenuModel.addModel(rightCommands.allElements);
			}
			
			ModalDialogManager.openDialog(popupMenu, {"x": point.x - 200, "y": point.y, "model": contextMenuModel});
		}
	}
	
	ListModel {
		id: contextMenuModel;
		
		function addModel(elements){
			let result = false;
			for (let i = 0; i < elements.length; i++){
				if (!elements[i].visible){
					insertNewItem({
									  "id": elements[i].modelData.m_elementId,
									  "name": elements[i].modelData.m_elementName,
									  "icon": elements[i].modelData.m_elementItemPath,
									  "isEnabled": elements[i].modelData.m_enabled
								  }
								  );
					result = true;
				}
			}
			
			return result;
		}
		
		function insertNewItem(params){
			contextMenuModel.append(params);
		}
	}
	
	Component {
		id: popupMenu;
		
		PopupMenuDialog {
			id: popupMenuDialog;
			shownItemsCount: 20;
			itemHeight: Style.controlHeightM;
			onFinished: {
				if (commandId !== ""){
					commandsItem.commandActivated(commandId, null)
				}
			}
		}
	}
}
