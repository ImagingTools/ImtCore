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
	
	property GuiElementContainer commandsModel: GuiElementContainer {};
	property alias centerCommandsWidth: centerCommands.width;
	// False until the first commands model arrives; a skeleton stands in for the buttons meanwhile.
	property bool commandsLoaded: false;
	onCommandsModelChanged: {
		updateGui()
	}

	function updateGui(){
		if (!commandsModel){
			return;
		}

		commandsLoaded = true;

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
		interval: 0
		onTriggered: {
			if (commandsItem.visible){
				commandsItem.layoutCommands();
			}
		}
	}

	function layoutCommands(){
		if (!button || !leftCommands || !centerCommands || !rightCommands){
			return;
		}

		// The "more" button takes its place only when a command does not fit without it
		let needsButton = !commandsItem.fitCommands(0);
		if (needsButton){
			commandsItem.fitCommands(Style.controlHeightM);
		}

		button.visible = needsButton;
	}

	/*
		Shows the commands of the whole panel by priority, each in its own group (alignment). Among commands
		of the same priority the next one goes to the group that takes the least room. Once a command does
		not fit, its group stops and no command of a lower priority is shown. Returns whether all commands fit.
	*/
	function fitCommands(reserved){
		let views = [leftCommands, centerCommands, rightCommands];
		let sizes = [leftCommands.commandSizes(), centerCommands.commandSizes(), rightCommands.commandSizes()];
		let used = [0, 0, 0];
		let counts = [0, 0, 0];
		let open = [true, true, true];
		// Priority of the first command that did not fit; lower ones are not shown
		let stopPriority = -1;

		while (true){
			let group = -1;
			for (let i = 0; i < 3; i++){
				if (!open[i] || counts[i] >= sizes[i].length || sizes[i][counts[i]].priority < stopPriority){
					continue;
				}
				if (group < 0){
					group = i;
					continue;
				}
				let priority = sizes[i][counts[i]].priority;
				let groupPriority = sizes[group][counts[group]].priority;
				if (priority > groupPriority || (priority === groupPriority && used[i] < used[group])){
					group = i;
				}
			}
			if (group < 0){
				break;
			}

			let size = sizes[group][counts[group]];
			let trial = used.slice();
			trial[group] = used[group] + (counts[group] > 0 ? views[group].spacing : 0) + size.width;
			if (commandsItem.__fits(trial[0], trial[1], trial[2], reserved)){
				used = trial;
				counts[group] += 1;
			}
			else {
				open[group] = false;
				stopPriority = Math.max(stopPriority, size.priority);
			}
		}

		for (let i = 0; i < 3; i++){
			views[i].maximumWidth = used[i];
			views[i].checkWidth();
		}

		return !leftCommands.hasHiddenCommands() && !centerCommands.hasHiddenCommands() && !rightCommands.hasHiddenCommands();
	}

	// The side groups take the panel edges, the center one stays centered between them
	function __fits(left, center, right, reserved){
		if (center > 0){
			return center / 2 + Style.marginM + Math.max(left, right + reserved) <= commandsItem.width / 2 - Style.marginM;
		}

		return left + right + (left > 0 && right > 0 ? Style.marginM : 0) <= commandsItem.width - 2 * Style.marginM - reserved;
	}

	// Shown only when the commands take longer than skeletonDelay.interval, so a quick load does not flash it.
	property bool skeletonDelayElapsed: false;

	Timer {
		id: skeletonDelay;
		interval: 300;
		running: !commandsItem.commandsLoaded;
		onTriggered: {
			commandsItem.skeletonDelayElapsed = true;
		}
	}

	Item {
		id: commandsSkeleton;

		anchors.fill: parent;
		visible: !commandsItem.commandsLoaded && commandsItem.skeletonDelayElapsed;

		Row {
			anchors.left: parent.left;
			anchors.leftMargin: Style.marginM;
			anchors.verticalCenter: parent.verticalCenter;
			spacing: Style.marginS;

			Repeater {
				model: 2;

				delegate: SkeletonBlock {
					width: Style.controlHeightM * (index === 0 ? 2.5 : 2);
					height: Style.controlHeightM - Style.marginXS;
					animated: true;
				}
			}
		}

		Row {
			anchors.horizontalCenter: parent.horizontalCenter;
			anchors.verticalCenter: parent.verticalCenter;
			spacing: Style.marginS;

			Repeater {
				model: 2;

				delegate: SkeletonBlock {
					width: Style.controlHeightM * 2.5;
					height: Style.controlHeightM - Style.marginXS;
					animated: true;
				}
			}
		}

		SkeletonBlock {
			anchors.right: parent.right;
			anchors.rightMargin: Style.marginM;
			anchors.verticalCenter: parent.verticalCenter;
			width: Style.controlHeightM * 3;
			height: Style.controlHeightM - Style.marginXS;
			animated: true;
		}
	}

	CommandsView {
		id: leftCommands;
		anchors.left: parent.left;
		anchors.leftMargin: Style.marginM;

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

		onCommandActivated: {
			commandsItem.commandActivated(commandId, params);
		}

		onUpdateWidth: {
			timer.restart()
		}
	}
	
	ToolButton {
		id: button;
		objectName: "MoreCommandsButton"
		anchors.verticalCenter: parent.verticalCenter;
		anchors.right: parent.right;
		width: visible ? Style.controlHeightM : 0;
		height: width;
		iconSource: "../../../" + Style.getIconPath("Icons/More", Icon.State.On, Icon.Mode.Normal);
		visible: false;
		decorator: Component {
			ToolButtonDecorator {
				color: baseElement && baseElement.hovered  ? Style.toolButtonHoverColor : "transparent";
				radius: height / 2;
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
			
			ModalDialogManager.openDialog(popupMenu, {"x": point.x - Style.sizeHintXXS, "y": point.y, "model": contextMenuModel});
		}
	}
	
	ListModel {
		id: contextMenuModel;
		
		function addModel(elements){
			let result = false;
			for (let i = 0; i < elements.length; i++){
				let element = elements[i].element;
				if (elements[i].hidden && element && element.m_visible){
					insertNewItem({
									  "id": element.m_elementId,
									  "name": element.m_elementName,
									  "icon": element.m_elementItemPath,
									  "isEnabled": element.m_enabled
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
			itemHeight: Style.controlHeightS + Style.spacingXS;
			onFinished: {
				if (commandId !== ""){
					commandsItem.commandActivated(commandId, null)
				}
			}
		}
	}
}
