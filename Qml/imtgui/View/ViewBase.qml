import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

Item {
	id: viewBase;
	
	property string viewId;
	property var model: null;
	
	property Component commandsControllerComp: null;
	property CommandsController commandsController: null;
	
	property Component commandsDelegateComp: Component {
		ViewCommandsDelegateBase {
			view: viewBase;
		}
	}
	
	property ViewCommandsDelegateBase commandsDelegate: null;
	
	property alias commandsView: headerViewLoader.item;
	property alias commandsViewComp: headerViewLoader.sourceComponent;
	
	property bool readOnly: false;
	
	signal commandsModelChanged(var commandsModel)
	signal commandActivated(string commandId);
	
	Connections {
		target: viewBase.commandsDelegate
		function onCommandActivated(commandId){
			viewBase.commandActivated(commandId)
		}
	}
	
	Component {
		id: commandsDecoratorComp;
		
		CommandsPanel {
			onCommandActivated: {
				if (viewBase.commandsDelegate){
					viewBase.commandsDelegate.commandHandle(commandId);
				}
			}
		}
	}
	
	function setAlertPanel(alertPanelComp){
		alertPanel.sourceComponent = alertPanelComp;
	}
	
	Loader {
		id: alertPanel;
		anchors.top: parent.top;
		anchors.left: parent.left;
		anchors.right: parent.right;
		height: visible ? 40: 0;
		visible: alertPanel.item != null && alertPanel.item !== undefined;
		objectName: "ViewBase";
	}
	
	Item {
		id: headerViewItem;
		anchors.top: alertPanel.bottom;
		anchors.left: parent.left;
		anchors.right: parent.right;
		height: visible ? 30 + 2 * Style.sizeMainMargin : 0;
		objectName: "ViewBase";
		visible: headerViewLoader.item && viewBase.commandsController != null;
		
		Loader {
			id: headerViewLoader;
			anchors.verticalCenter: parent.verticalCenter;
			width: parent.width;
			sourceComponent: commandsDecoratorComp;
			onLoaded: {
				if (viewBase.commandsController){
					viewBase.commandsController.commandsView = item;
				}
			}
		}
	}
	
	Rectangle {
		id: separator;
		anchors.top: headerViewItem.bottom;
		width: parent.width;
		height: visible ? 1 : 0;
		color: Style.borderColor;
		visible: headerViewLoader.item && viewBase.commandsController != null;
		objectName: "ViewBase";
	}
	
	Rectangle {
		id: viewContent;
		anchors.top: separator.bottom;
		anchors.left: parent.left;
		anchors.right: parent.right;
		anchors.bottom: parent.bottom;
		objectName: "ViewBase";
		color: Style.backgroundColor2;
	}
	
	Component.onCompleted: {
		for (let i = 0; i < viewBase.children.length; i++){
			if (viewBase.children[i].objectName !== "ViewBase"){
				let element = viewBase.children[i];
				element.parent = viewContent;
				i--;
			}
		}
		
		if (commandsControllerComp){
			commandsController = commandsControllerComp.createObject(viewBase);
		}
		
		if (commandsDelegateComp){
			commandsDelegate = commandsDelegateComp.createObject(viewBase);
		}
	}
	
	onCommandsDelegateChanged: {
		if (commandsDelegate){
			commandsDelegate.view = viewBase;
		}
	}
	
	onReadOnlyChanged: {
		setReadOnly(readOnly);
	}
	
	onVisibleChanged: {
		if (commandsController && visible && internal.localizationChanged){
			commandsController.getCommands()
		}
	}
	
	Connections {
		target: viewBase.commandsController;
		function onCommandsReceived(typeId, commands){
			viewBase.commandsView.commandsModel = commands
			
			if (viewBase.visible || internal.localizationChanged){
				internal.updateStatesFromCache()
				internal.localizationChanged = false
			}
		}
	}
	
	ApplicationEvents {
		onLocalizationChanged: {
			internal.cacheCommandsState();
			
			if (viewBase.visible){
				viewBase.commandsController.getCommands();
			}
			else{
				internal.localizationChanged = true
			}
		}
	}
	
	function onModelChanged(){
		doUpdateGui();
	}
	
	function setReadOnly(readOnly){
	}
	
	// for override
	function updateGui(){}
	
	// for override
	function updateModel(){}
	
	// Update representation model from GUI
	function doUpdateModel()
	{
		if (!model){
			return;
		}
		
		if (readOnly || internal.blockingUpdateModel || internal.blockingUpdateGui){
			return;
		}
		
		console.debug("doUpdateModel", model);
		
		if (model.beginChanges !== undefined){
			model.beginChanges();
		}
		
		internal.blockingUpdateGui = true;
		
		updateModel();
		
		internal.blockingUpdateGui = false;
		
		if (model.endChanges !== undefined){
			model.endChanges();
		}
	}
	
	// Update GUI from representation model
	function doUpdateGui()
	{
		if (!model){
			return;
		}
		
		if (internal.blockingUpdateGui || internal.blockingUpdateModel){
			return;
		}
		
		console.debug("doUpdateGui", model);
		
		internal.blockingUpdateModel = true;
		
		updateGui();
		
		internal.blockingUpdateModel = false;
	}
	
	function setBlockingUpdateModel(value){
		internal.blockingUpdateModel = value;
	}
	
	function setBlockingUpdateGui(value){
		internal.blockingUpdateGui = value;
	}
	
	function guiIsBlocked(){
		return internal.blockingUpdateGui;
	}
	
	QtObject {
		id: internal;
		
		property bool localizationChanged: false;
		property bool blockingUpdateGui: false;
		property bool blockingUpdateModel: false;
		
		property var commandStates: ({})
		
		function cacheCommandsState(){
			commandStates = {}
			if (viewBase.commandsController){
				let commandIds = viewBase.commandsController.getCommandIds()
				for (let i = 0; i < commandIds.length; i++){
					let commandId = commandIds[i]
					commandStates[commandId] = viewBase.commandsController.commandIsEnabled(commandId)
				}
			}
		}
		
		function updateStatesFromCache(){
			if (viewBase.commandsController){
				let keys = Object.keys(commandStates)
				for (let i = 0; i < keys.length; i++){
					let commandId = keys[i]
					let state = commandStates[commandId]
					
					viewBase.commandsController.setCommandIsEnabled(commandId, state)
				}
			}
		}
	}
}


