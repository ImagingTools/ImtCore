import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtbaseCommandsSdl 1.0

Item {
	id: commandsItem;
	height: Style.controlHeightM;

	signal commandsReady();
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

		commandsReady()
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
		interval: 50
		onTriggered: {
			if (commandsItem.visible){
				commandsItem.checkCommandsWidth();
				commandsItem.checkButtonVisible();
			}
		}
	}

	function checkCommandsWidth(){
		let checkWidthFunc = function (commands, type){
			commands.maximumWidth = commandsItem.computeMaxWidth(
						type,
						leftCommands.calcVisibleCommandsCount(),
						centerCommands.calcVisibleCommandsCount(),
						rightCommands.calcVisibleCommandsCount(),
						commandsItem.width
					)
			commands.checkWidth()
		}

		checkWidthFunc(leftCommands, "left")
		checkWidthFunc(centerCommands, "center")
		checkWidthFunc(rightCommands, "right")
	}
	
	function checkButtonVisible(){
		if (!button || !leftCommands || !centerCommands || !rightCommands){
			return;
		}

		button.visible = leftCommands.hasHiddenCommands() ||
				centerCommands.hasHiddenCommands() ||
				rightCommands.hasHiddenCommands();
	}

	function computeMaxWidth(role, left, center, right, full) {
		let hasLeft = left > 0;
		let hasCenter = center > 0;
		let hasRight = right > 0;
		let visible = (hasLeft ? 1 : 0) + (hasCenter ? 1 : 0) + (hasRight ? 1 : 0);
	
		// Left || Center || Right
		if (visible === 1)
			return full;
	
		// Left && Center && Right
		if (visible === 3) {
			if (role === "center") return full * 0.4;
			return full * 0.3;
		}
	
		// Center && (Left || Right)
		if (hasCenter) {
			if (role === "center") return full * 0.4
			return full * 0.3
		}
	
		// (Left && Right)
		return full * 0.5;
	}

	CommandsView {
		id: leftCommands;
		anchors.left: parent.left;
		anchors.leftMargin: Style.marginM;

		visible: !timer.running
		onCommandActivated: {
			commandsItem.commandActivated(commandId, params);
		}

		onUpdateWidth: {
			timer.restart()
		}
	}
	
	CommandsView {
		id: centerCommands;
		anchors.horizontalCenter: parent.horizontalCenter;

		visible: !timer.running
		onCommandActivated: {
			commandsItem.commandActivated(commandId, params);
		}

		onUpdateWidth: {
			timer.restart()
		}
	}
	
	CommandsView {
		id: rightCommands;
		anchors.right: button.left;
		anchors.rightMargin: Style.marginM;

		visible: !timer.running
		onCommandActivated: {
			commandsItem.commandActivated(commandId, params);
		}

		onUpdateWidth: {
			timer.restart()
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
				if (elements[i].hidden && elements[i].element && elements[i].element.m_visible){
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
