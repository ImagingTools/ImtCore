import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtdocgui 1.0
import imtcontrols 1.0

Item {
	id: viewBase;

	property string viewTypeId
	property string viewId
	property var model: null
	
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

	property string contentColor: Style.backgroundColor2

	property bool commandsPanelVisible: true
	property int commandsPanelHeight: Style.controlHeightM + 2 * Style.marginM

	signal commandsModelChanged(var commandsModel)
	signal commandActivated(string commandId)
	signal modelDataChanged(var view, var model)
	signal guiUpdated(var view, var model)
	signal guiVisibleChanged(var view, bool visible)

	Connections {
		target: viewBase.model
		function onModelChanged(){
			if (viewBase.internal__.blockingUpdateModel){
				return
			}

			viewBase.modelDataChanged(viewBase, viewBase.model)
		}
	}
	
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
					viewBase.commandsDelegate.commandHandle(commandId, params);
				}
			}
			
			onCommandsReady: {
				viewBase.commandsModelChanged(commandsModel)
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
		height: visible ? Style.controlHeightL: 0;
		visible: alertPanel.item != null && alertPanel.item !== undefined;
		objectName: "ViewBase";
	}
	
	Item {
		id: headerViewItem;
		anchors.top: alertPanel.bottom;
		anchors.left: parent.left;
		anchors.right: parent.right;
		height: visible ? viewBase.commandsPanelHeight : 0;
		objectName: "ViewBase";
		visible: !viewBase.commandsPanelVisible ? false : headerViewLoader.item && viewBase.commandsController != null;
		
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
		visible: !viewBase.commandsPanelVisible ? false : headerViewLoader.item && viewBase.commandsController != null;
		objectName: "ViewBase";
	}
	
	Rectangle {
		id: viewContent;
		anchors.top: separator.bottom;
		anchors.left: parent.left;
		anchors.right: parent.right;
		anchors.bottom: parent.bottom;
		objectName: "ViewBase";
		color: viewBase.contentColor;
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
		if (commandsController && visible && viewBase.internal__.localizationChanged){
			commandsController.getCommands()
		}

		guiVisibleChanged(viewBase, visible)
	}
	
	Connections {
		target: viewBase.commandsController;
		function onCommandsReceived(typeId, commands){
			viewBase.commandsView.commandsModel = commands
			
			if (viewBase.visible || viewBase.internal__.localizationChanged){
				viewBase.internal__.updateStatesFromCache()
				viewBase.internal__.localizationChanged = false
			}
		}
	}
	
	ApplicationEvents {
		onLocalizationChanged: {
			viewBase.internal__.cacheCommandsState();
			
			if (viewBase.visible && viewBase.commandsController){
				viewBase.commandsController.getCommands();
			}
			else{
				viewBase.internal__.localizationChanged = true
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
		console.debug("doUpdateModel", model, viewBase.internal__);
		if (!model || !viewBase.internal__){
			return;
		}
		
		if (readOnly || viewBase.internal__.blockingUpdateModel || viewBase.internal__.blockingUpdateGui){
			return;
		}
		
		console.debug("doUpdateModel", model);
		
		if (model.beginChanges !== undefined){
			model.beginChanges();
		}
		
		viewBase.internal__.blockingUpdateGui = true;
		
		updateModel();
		
		viewBase.internal__.blockingUpdateGui = false;
		
		if (model.endChanges !== undefined){
			model.endChanges();
		}
	}
	
	// Update GUI from representation model
	function doUpdateGui()
	{
		console.debug("doUpdateGui");
		if (!model || !viewBase.internal__){
			return;
		}
		
		if (viewBase.internal__.blockingUpdateGui || viewBase.internal__.blockingUpdateModel){
			return;
		}
		
		viewBase.internal__.blockingUpdateModel = true;
		
		updateGui();
		
		viewBase.internal__.blockingUpdateModel = false;

		guiUpdated(this, model)
	}
	
	function setBlockingUpdateModel(value){
		viewBase.internal__.blockingUpdateModel = value;
	}
	
	function setBlockingUpdateGui(value){
		if (viewBase.internal__){
			viewBase.internal__.blockingUpdateGui = value;
		}
	}
	
	function guiIsBlocked(){
		if (!viewBase.internal__){
			return false
		}

		return viewBase.internal__.blockingUpdateGui;
	}
	
	property QtObject internal__: QtObject {
		property bool localizationChanged: false;
		property bool blockingUpdateGui: false;
		property bool blockingUpdateModel: false;
		onBlockingUpdateModelChanged: {
			console.log("onBlockingUpdateModelChanged",blockingUpdateModel )
		}
		
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


